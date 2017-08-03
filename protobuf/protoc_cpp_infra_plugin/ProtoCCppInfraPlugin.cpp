#include "protobuf/protoc_cpp_infra_plugin/ProtoCCppInfraPlugin.hpp"
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/compiler/cpp/cpp_helpers.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include <string>

namespace application
{
    class CppInfraCodeGenerator
        : public google::protobuf::compiler::CodeGenerator
    {
    public:
        virtual bool Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* generatorContext, std::string* error) const override
        {
            std::string basename = google::protobuf::compiler::cpp::StripProto(file->name()) + ".pb";

            google::protobuf::io::ZeroCopyOutputStream* headerStream = generatorContext->Open(basename + ".h");
            delete headerStream;

            google::protobuf::io::ZeroCopyOutputStream* sourceStream = generatorContext->Open(basename + ".cc");
            delete sourceStream;

            return true;
        }
    };
}

int main(int argc, char* argv[])
{
    application::CppInfraCodeGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
