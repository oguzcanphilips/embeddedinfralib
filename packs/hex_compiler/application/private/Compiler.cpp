#include "packs/hex_compiler/application/public/Compiler.hpp"
#include "packs/hex_compiler/application/public/HexOutput.hpp"
#include "infra/stream/public/StdStringInputStream.hpp"

namespace application
{
    Compiler::Compiler(hal::FileSystem& fileSystem)
        : fileSystem(fileSystem)
    {
        SetInitialState();
    }

    void Compiler::AddParameter(const std::string& parameter)
    {
        if (parameter.find("-") == 0)
        {
            if (parameter == "-output")
                ReadOutputSwitch();
            else if (parameter == "-offset")
                ReadOffsetSwitch();
            else if (parameter == "-binary")
                ReadBinarySwitch();
            else if (parameter == "-hex")
                ReadHexSwitch();
            else
                throw IncorrectUsageException();
        }
        else
        {
            if (GetCurrentState() == CommandLineParser_ReadingOffset)
            {
                infra::StdStringInputStream::WithStorage stream(infra::inPlace, parameter);
                if (parameter.find("0x") == 0)
                    stream >> "0x" >> infra::hex >> number;
                else
                    stream >> number;

                if (!stream.Storage().empty())
                    throw IncorrectUsageException();

                ReadNumber();
            }
            else
            {
                fileName = parameter;
                ReadFileName();
            }
        }
    }

    void Compiler::Compile()
    {
        ReadEndOfInput();
    }

    void Compiler::AddInputFile()
    {
        if (hexFormat)
            object.AddHex(fileSystem.ReadFile(fileName), offset, fileName);
        else
            object.AddBinary(fileSystem.ReadBinaryFile(fileName), offset, fileName);
        offset = 0;
    }

    void Compiler::SetOutputFile()
    {
        outputFileName = fileName;
    }

    void Compiler::ProcessFiles()
    {
        fileSystem.WriteFile(outputFileName, HexOutput(object.Memory()));
    }

    void Compiler::SetOffset()
    {
        offset = number;
    }

    void Compiler::ReportUsage()
    {
        throw IncorrectUsageException();
    }

    void Compiler::SetHexFormat()
    {
        hexFormat = true;
    }

    void Compiler::SetBinaryFormat()
    {
        hexFormat = false;
    }
}
