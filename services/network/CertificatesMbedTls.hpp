#ifndef SERVICES_CERTIFICATES_MBED_TLS_HPP
#define SERVICES_CERTIFICATES_MBED_TLS_HPP

#include "hal/synchronous_interfaces/SynchronousRandomDataGenerator.hpp"
#include "infra/stream/StringOutputStream.hpp"
#include "infra/syntax/Asn1Formatter.hpp"
#include "infra/util/BoundedString.hpp"
#include "mbedtls/ssl.h"

namespace services
{
	class CertificatesMbedTls
	{
	public:
		CertificatesMbedTls();
		CertificatesMbedTls(const CertificatesMbedTls& other) = delete;
		CertificatesMbedTls& operator=(const CertificatesMbedTls& other) = delete;
		~CertificatesMbedTls();

		void AddCertificateAuthority(const infra::BoundedConstString& certificate);
		void AddOwnCertificate(const infra::BoundedConstString& certificate, const infra::BoundedConstString& privateKey);

		void Config(mbedtls2_ssl_config& sslConfig);

		void GenerateNewKey(hal::SynchronousRandomDataGenerator& randomDataGenerator);
		void WritePrivateKey(infra::BoundedString outputBuffer);
        void WriteOwnCertificate(infra::BoundedString outputBuffer, hal::SynchronousRandomDataGenerator& randomDataGenerator);

	private:
		int32_t ExtractExponent(const mbedtls2_rsa_context& rsaContext) const;

        void X509AddAlgorithm(infra::Asn1Formatter& root, const mbedtls2_x509_buf& oid) const;
        void X509AddName(infra::Asn1Formatter& root, const mbedtls2_x509_name& name) const;
        void X509AddTime(infra::Asn1Formatter& root, const mbedtls2_x509_time& time) const;

	protected:
		mbedtls2_x509_crt caCertificates;
		mbedtls2_x509_crt ownCertificate;
		mbedtls2_pk_context privateKey;
	};
}  //! namespace services

#endif  //! SERVICES_CERTIFICATES_MBED_TLS_HPP
