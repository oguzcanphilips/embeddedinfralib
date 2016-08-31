#include "hal/windows/public/FileSystemWin.hpp"
#include "packs/hex_compiler/application/public/Compiler.hpp"
#include <iostream>
#include <iomanip>

namespace
{
    void AddParameters(application::Compiler& compiler, int argc, const char* argv[])
    {
        for (int i = 1; i != argc; ++i)
            compiler.AddParameter(argv[i]);
    }

    void Compile(int argc, const char* argv[])
    {
        hal::FileSystemWin fileSystem;
        application::Compiler compiler(fileSystem);
        AddParameters(compiler, argc, argv);
        compiler.Compile();
    }
}

int main(int argc, const char* argv[])
{
    try
    {
        Compile(argc, argv);
        return EXIT_SUCCESS;
    }
    catch (application::IncorrectUsageException&)
    {
        std::cout << "Usage: " << argv[0] << " ([-offset <number>] <input file>)* -output <output file>" << std::endl;
    }
    catch (application::IncorrectCrcException& exception)
    {
        std::cout << "Incorrect CRC in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::NoEndOfFileException& exception)
    {
        std::cout << "No end of file found in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::DataAfterEndOfFileException& exception)
    {
        std::cout << "Data found after end of file in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::UnknownRecordException& exception)
    {
        std::cout << "Unknown record in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::RecordTooShortException& exception)
    {
        std::cout << "Record too short in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::RecordTooLongException& exception)
    {
        std::cout << "Record too long in file " << exception.file << " at line " << exception.line << std::endl;
    }
    catch (application::OverwriteException& exception)
    {
        std::cout << "Contents specified twice for memory location at address 0x" << std::hex << std::setw(8) << std::setfill('0') << exception.position << std::endl;
    }
    catch (hal::CannotOpenFileException& exception)
    {
        std::cout << "Cannot open file " << exception.name << std::endl;
    }
    catch (std::ios::failure& exception)
    {
        std::cout << "I/O failure: " << exception.what() << std::endl;
    }
    catch (std::runtime_error& exception)
    {
        std::cout << "Runtime error: " << exception.what() << std::endl;
    }

    return EXIT_FAILURE;
}
