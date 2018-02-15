#include "infra/stream/ByteOutputStream.hpp"
#include "infra/stream/StringOutputStream.hpp"
#include "services/network/CertificatesMbedTls.hpp"
#include "mbedtls/pk.h"
#include "mbedtls/oid.h"

namespace
{
	int RandomDataGeneratorWrapper(void* data, unsigned char* output, std::size_t size)
	{
		reinterpret_cast<hal::SynchronousRandomDataGenerator*>(data)->GenerateRandomData(infra::ByteRange(reinterpret_cast<uint8_t*>(output), reinterpret_cast<uint8_t*>(output) + size));
		return 0;
	}

	infra::ByteRange MakeByteRange(const mbedtls2_mpi& number)
	{
		return infra::ByteRange(reinterpret_cast<unsigned char*>(number.p), reinterpret_cast<unsigned char*>(number.p + number.n));
	}

    infra::ConstByteRange MakeConstByteRange(const mbedtls2_x509_buf& buffer)
    {
        return infra::ConstByteRange(buffer.p, buffer.p + buffer.len);
    }
}

namespace services
{
	CertificatesMbedTls::CertificatesMbedTls()
	{
		mbedtls2_x509_crt_init(&caCertificates);
		mbedtls2_x509_crt_init(&ownCertificate);
		mbedtls2_pk_init(&privateKey);
	}

	CertificatesMbedTls::~CertificatesMbedTls()
	{
		mbedtls2_pk_free(&privateKey);
		mbedtls2_x509_crt_free(&caCertificates);
		mbedtls2_x509_crt_free(&ownCertificate);
	}

	void CertificatesMbedTls::AddCertificateAuthority(const infra::BoundedConstString& certificate)
	{
		int result = mbedtls2_x509_crt_parse(&caCertificates, reinterpret_cast<const unsigned char*>(certificate.data()), certificate.size());
		assert(result == 0);
	}

	void CertificatesMbedTls::AddOwnCertificate(const infra::BoundedConstString& certificate, const infra::BoundedConstString& key)
	{
		int result = mbedtls2_x509_crt_parse(&ownCertificate, reinterpret_cast<const unsigned char*>(certificate.data()), certificate.size());
		assert(result == 0);
		result = mbedtls2_pk_parse_key(&privateKey, reinterpret_cast<const unsigned char*>(key.data()), key.size(), NULL, 0);
		assert(result == 0);
	}

	void CertificatesMbedTls::Config(mbedtls2_ssl_config& sslConfig)
	{
		mbedtls2_ssl_conf_ca_chain(&sslConfig, &caCertificates, nullptr);
		int result = mbedtls2_ssl_conf_own_cert(&sslConfig, &ownCertificate, &privateKey);
		assert(result == 0);
	}

	void CertificatesMbedTls::GenerateNewKey(hal::SynchronousRandomDataGenerator& randomDataGenerator)
	{
		if (mbedtls2_pk_get_type(&ownCertificate.pk) != MBEDTLS_PK_RSA)
			return;

		mbedtls2_rsa_context* rsaContext = mbedtls2_pk_rsa(ownCertificate.pk);
		assert(rsaContext != nullptr);

		size_t keySizeInBits = mbedtls2_pk_get_bitlen(&ownCertificate.pk);
		int32_t exponent = ExtractExponent(*rsaContext);

		mbedtls2_rsa_gen_key(rsaContext, &RandomDataGeneratorWrapper, &randomDataGenerator, keySizeInBits, exponent);
	}

	void CertificatesMbedTls::WritePrivateKey(infra::BoundedString outputBuffer)
	{
		infra::ByteOutputStream::WithStorage<2048> contentsStream;
		infra::Asn1Formatter formatter(contentsStream);
		{
			auto sequence = formatter.StartSequence();

			mbedtls2_rsa_context& rsaContext = *mbedtls2_pk_rsa(privateKey);

			sequence.Add(uint8_t(rsaContext.ver));
			sequence.AddBigNumber(MakeByteRange(rsaContext.N));
			sequence.AddBigNumber(MakeByteRange(rsaContext.E));
			sequence.AddBigNumber(MakeByteRange(rsaContext.D));
			sequence.AddBigNumber(MakeByteRange(rsaContext.P));
			sequence.AddBigNumber(MakeByteRange(rsaContext.Q));
			sequence.AddBigNumber(MakeByteRange(rsaContext.DP));
			sequence.AddBigNumber(MakeByteRange(rsaContext.DQ));
			sequence.AddBigNumber(MakeByteRange(rsaContext.QP));
		}

		outputBuffer.clear();
		infra::StringOutputStream stream(outputBuffer);
		stream << "-----BEGIN RSA PRIVATE KEY-----\r\n";
		stream << infra::AsBase64(contentsStream.Writer().Processed());
		stream << "-----END RSA PRIVATE KEY-----\r\n";
		stream << '\0';
	}

    void CertificatesMbedTls::WriteOwnCertificate(infra::BoundedString outputBuffer, hal::SynchronousRandomDataGenerator& randomDataGenerator)
    {
        infra::ByteOutputStream::WithStorage<2048> contentsStream;

        infra::Asn1Formatter formatter(contentsStream);
        {
            infra::ByteRange tbsBegin;
            infra::ByteRange tbsEnd;

            auto certificateSequence = formatter.StartSequence();
            {
                tbsBegin = contentsStream.Writer().Processed();
                {
                    auto tbsSequence = certificateSequence.StartSequence();

                    // Version
                    {
                        auto versionContext = tbsSequence.StartContextSpecific();
                        versionContext.Add(uint8_t(ownCertificate.version - 1));
                    }

                    // Serial
                    tbsSequence.AddSerial(MakeConstByteRange(ownCertificate.serial));

                    // Signature Object ID 1
                    X509AddAlgorithm(tbsSequence, ownCertificate.sig_oid);

                    // Issuer Name
                    {
                        auto issuerSequence = tbsSequence.StartSequence();
                        X509AddName(issuerSequence, ownCertificate.issuer);
                    }

                    // Validity
                    {
                        auto timeSequence = tbsSequence.StartSequence();
                        X509AddTime(timeSequence, ownCertificate.valid_from);
                        X509AddTime(timeSequence, ownCertificate.valid_to);
                    }

                    // Subject Name
                    {
                        auto subjectSequence = tbsSequence.StartSequence();
                        X509AddName(subjectSequence, ownCertificate.subject);
                    }

                    // PublicKeyInfo
                    {
                        auto publicKeyInfoSequence = tbsSequence.StartSequence();
                        mbedtls2_x509_buf pk_oid;

                        mbedtls2_oid_get_oid_by_pk_alg(mbedtls2_pk_get_type(&ownCertificate.pk), const_cast<const char**>(reinterpret_cast<char**>(&pk_oid.p)), &pk_oid.len);

                        X509AddAlgorithm(publicKeyInfoSequence, pk_oid);

                        {
                            auto publicKeyBitString = publicKeyInfoSequence.StartBitString();
                            {
                                auto rsaPublicKeySequence = publicKeyBitString.StartSequence();
                                mbedtls2_rsa_context* rsaContext = mbedtls2_pk_rsa(ownCertificate.pk);
                                assert(rsaContext != nullptr);

                                rsaPublicKeySequence.AddBigNumber(MakeByteRange(rsaContext->N));
                                rsaPublicKeySequence.AddBigNumber(MakeByteRange(rsaContext->E));
                            }
                        }
                    }

                    //  v3 Extensions
                    if (ownCertificate.version == 3)
                        tbsSequence.AddContextSpecific(3, MakeConstByteRange(ownCertificate.v3_ext));
                }

                tbsEnd = contentsStream.Writer().Processed();

                // Sign new certificate
                unsigned char hash[64] = {};

                if (mbedtls2_md(mbedtls2_md_info_from_type(ownCertificate.sig_md), tbsBegin.cend(), std::distance(tbsBegin.cend(), tbsEnd.cend()), hash) != 0)
                    std::abort();

                unsigned char signature[MBEDTLS_MPI_MAX_SIZE];
                size_t signatureLength = 0;

                if (mbedtls2_pk_sign(&privateKey, ownCertificate.sig_md, hash, 0, signature, &signatureLength, &RandomDataGeneratorWrapper, &randomDataGenerator) != 0)
                    std::abort();

                X509AddAlgorithm(certificateSequence, ownCertificate.sig_oid);
                certificateSequence.AddBitString(infra::ConstByteRange(signature, signature + signatureLength));
            }
        }

        outputBuffer.clear();
        infra::StringOutputStream stream(outputBuffer);
        stream << "-----BEGIN CERTIFICATE-----\r\n";
        stream << infra::AsBase64(contentsStream.Writer().Processed());
        stream << "-----END CERTIFICATE-----\r\n";
        stream << '\0';
    }

	int32_t CertificatesMbedTls::ExtractExponent(const mbedtls2_rsa_context& rsaContext) const
	{
		uint32_t exponent = 0;
		mbedtls2_mpi_write_binary(&rsaContext.E, reinterpret_cast<unsigned char*>(&exponent), sizeof(uint32_t));
		exponent = (exponent << 16) | (exponent >> 16);
		exponent = ((exponent & 0x00ff00ff) << 8) | ((exponent & 0xff00ff00) >> 8);

		return exponent;
	}

    void CertificatesMbedTls::X509AddAlgorithm(infra::Asn1Formatter& root, const mbedtls2_x509_buf& oid) const
    {
        auto algorithmSequence = root.StartSequence();
        algorithmSequence.AddObjectId(MakeConstByteRange(oid));
        algorithmSequence.AddOptional<uint8_t>(infra::none);
    }

    void CertificatesMbedTls::X509AddName(infra::Asn1Formatter& root, const mbedtls2_x509_name& name) const
    {
        const mbedtls2_x509_name* node = &name;

        while (node)
        {
            auto name = root.StartSet();
            {
                auto attributeTypeAndValueSequence = name.StartSequence();
                attributeTypeAndValueSequence.AddObjectId(MakeConstByteRange(node->oid));
                attributeTypeAndValueSequence.AddPrintableString(MakeConstByteRange(node->val));
            }

            node = node->next;
        }
    }

    void CertificatesMbedTls::X509AddTime(infra::Asn1Formatter& root, const mbedtls2_x509_time& time) const
    {
        root.AddTime(time.year, time.mon, time.day, time.hour, time.min, time.sec);
    }
}
