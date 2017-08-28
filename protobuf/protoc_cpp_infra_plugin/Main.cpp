#include "google/protobuf/compiler/plugin.h"
#include "protobuf/protoc_cpp_infra_plugin/ProtoCCppInfraPlugin.hpp"
#include <crtdbg.h>

int main(int argc, char* argv[])
{
    //_CrtDbgBreak();
    application::CppInfraCodeGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
