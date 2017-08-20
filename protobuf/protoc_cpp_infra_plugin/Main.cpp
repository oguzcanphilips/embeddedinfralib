#include "google/protobuf/compiler/plugin.h"
#include "protobuf/protoc_cpp_infra_plugin/ProtoCCppInfraPlugin.hpp"

int main(int argc, char* argv[])
{
    application::CppInfraCodeGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
