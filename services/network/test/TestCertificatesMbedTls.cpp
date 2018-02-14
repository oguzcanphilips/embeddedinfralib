#include "gmock/gmock.h"
#include "hal/windows/SynchronousRandomDataGeneratorWin.hpp"
#include "mbedtls/certs.h"
#include "services/network/CertificatesMbedTls.hpp"

bool CompareStringIgnoreNewline(const std::string& expected, const std::string& actual, std::string ignore = "\r\n")
{
    size_t expectedIndex = 0;
    size_t actualIndex = 0;

    while (true)
    {
        while (expectedIndex < expected.size() && ignore.find_first_of(expected[expectedIndex]) != std::string::npos)
            expectedIndex++;
        while (actualIndex < actual.size() && ignore.find_first_of(actual[actualIndex]) != std::string::npos)
            actualIndex++;

        if (expectedIndex >= expected.size())
            return actualIndex >= actual.size();
        if (actualIndex >= actual.size())
            return false;

        if (expected[expectedIndex++] != actual[actualIndex++])
            return false;
    }

    return true;
}

class CertificatesMbedTlsTest
	: public testing::Test
{
public:
    CertificatesMbedTlsTest()
    {
        certificates.AddOwnCertificate(infra::BoundedConstString(mbedtls2_test_srv_crt, mbedtls2_test_srv_crt_len), infra::BoundedConstString(mbedtls2_test_srv_key, mbedtls2_test_srv_key_len));
    }

	services::CertificatesMbedTls certificates;
	hal::SynchronousRandomDataGeneratorWin randomDataGenerator;
};

TEST_F(CertificatesMbedTlsTest, write_private_key)
{
	infra::BoundedString::WithStorage<2048> privateKey;
	certificates.WritePrivateKey(privateKey);

    EXPECT_TRUE(CompareStringIgnoreNewline(mbedtls2_test_srv_key, privateKey.data()));
}

TEST_F(CertificatesMbedTlsTest, write_certificate)
{
    infra::BoundedString::WithStorage<2048> ownCertificate;
    certificates.WriteOwnCertificate(ownCertificate);

    std::cout << mbedtls2_test_srv_crt << std::endl;
    std::cout << ownCertificate.data() << std::endl;

    //EXPECT_TRUE(CompareStringIgnoreNewline(mbedtls2_test_srv_crt, ownCertificate.data()));
}

TEST_F(CertificatesMbedTlsTest, generate_new_key)
{
	certificates.GenerateNewKey(randomDataGenerator);

	infra::BoundedString::WithStorage<2048> privateKey;
	certificates.WritePrivateKey(privateKey);

    EXPECT_STRNE(privateKey.data(), mbedtls2_test_srv_key);
}
