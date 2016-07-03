#include "hex_compiler_application/Compiler.hpp"
#include "hal_test_doubles/FileSystemStub.hpp"
#include "gtest/gtest.h"

class CompilerTest
    : public testing::Test
{
public:
    CompilerTest()
        : compiler(fileSystem)
    {}

    hal::FileSystemStub fileSystem;
    application::Compiler compiler;
};

TEST_F(CompilerTest, Construction)
{}

TEST_F(CompilerTest, GenerateEmptyOutput)
{
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateEmptyOutputToAnotherFile)
{
    compiler.AddParameter("-output");
    compiler.AddParameter("AnotherOutput.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":00000001ff"}), fileSystem.files["AnotherOutput.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromEmptyInput)
{
    fileSystem.files["Input.hex"] = std::vector<std::string>{":00000001ff"};

    compiler.AddParameter("Input.hex");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromInput)
{
    fileSystem.files["Input.hex"] = std::vector<std::string>{":0100000001fe", ":00000001ff"};

    compiler.AddParameter("Input.hex");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":0100000001fe", ":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, IncorrectUsage)
{
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    EXPECT_THROW(compiler.AddParameter("Output.hex"), application::IncorrectUsageException);
}

TEST_F(CompilerTest, GenerateOutputFromTwoInputs)
{
    fileSystem.files["Input1.hex"] = std::vector<std::string>{":0100000001fe", ":00000001ff"};
    fileSystem.files["Input2.hex"] = std::vector<std::string>{":0100010001fd", ":00000001ff"};

    compiler.AddParameter("Input1.hex");
    compiler.AddParameter("Input2.hex");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":020000000101fc", ":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromInputAtOffset)
{
    fileSystem.files["Input.hex"] = std::vector<std::string>{":0100000001fe", ":00000001ff"};

    compiler.AddParameter("-offset");
    compiler.AddParameter("1");
    compiler.AddParameter("Input.hex");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":0100010001fd", ":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromInputAtHexOffset)
{
    fileSystem.files["Input.hex"] = std::vector<std::string>{":0100000001fe", ":00000001ff"};

    compiler.AddParameter("-offset");
    compiler.AddParameter("0x1");
    compiler.AddParameter("Input.hex");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":0100010001fd", ":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromBinaryInput)
{
    fileSystem.binaryFiles["Input.bin"] = std::vector<uint8_t>{ 1 };

    compiler.AddParameter("-binary");
    compiler.AddParameter("Input.bin");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":0100000001fe", ":00000001ff"}), fileSystem.files["Output.hex"]);
}

TEST_F(CompilerTest, GenerateOutputFromBinaryInputWithOffset)
{
    fileSystem.binaryFiles["Input.bin"] = std::vector<uint8_t>{ 1 };

    compiler.AddParameter("-offset");
    compiler.AddParameter("1");
    compiler.AddParameter("-binary");
    compiler.AddParameter("Input.bin");
    compiler.AddParameter("-output");
    compiler.AddParameter("Output.hex");
    compiler.Compile();

    EXPECT_EQ((std::vector<std::string>{":0100010001fd", ":00000001ff"}), fileSystem.files["Output.hex"]);
}
