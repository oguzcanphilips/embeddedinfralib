#include "gtest/gtest.h"
#include "hal/interfaces/test_doubles/public/FileSystemStub.hpp"
#include "upgrade_pack_builder_library/InputHex.hpp"
#include "upgrade_pack_builder_test/ImageSecurityNone.hpp"
#include <algorithm>

class TestInputHex
    : public testing::Test
{
public:
    TestInputHex()
        : fileSystem("fileName", std::vector<std::string>{ ":020000040001f9", ":0100000001fe", ":00000001FF" })
        , input("main", "fileName", fileSystem, imageSecurity)
    {}

    hal::FileSystemStub fileSystem;
    application::ImageSecurityNone imageSecurity;
    application::InputHex input;
};

TEST_F(TestInputHex, ImageSize)
{
    std::vector<uint8_t> image = input.Image();
    application::ImageHeaderPrologue& header = reinterpret_cast<application::ImageHeaderPrologue&>(image.front());

    EXPECT_EQ(25, image.size());
}

