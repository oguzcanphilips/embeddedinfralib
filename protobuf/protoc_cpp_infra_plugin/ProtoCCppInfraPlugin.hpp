#ifndef PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP
#define PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP

#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/descriptor.h"
#include "protobuf/protoc_cpp_infra_plugin/CppFormatter.hpp"

namespace application
{
    struct UnsupportedFieldType
    {
        std::string fieldName;
        google::protobuf::FieldDescriptor::Type type;
    };

    struct UnspecifiedStringSize
    {
        std::string fieldName;
    };

    struct UnspecifiedArraySize
    {
        std::string fieldName;
    };

    class CppInfraCodeGenerator
        : public google::protobuf::compiler::CodeGenerator
    {
    public:
        virtual bool Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* generatorContext, std::string* error) const override;
    };

    class FieldGenerator
    {
    public:
        FieldGenerator(const google::protobuf::FieldDescriptor& descriptor);
        FieldGenerator(const FieldGenerator& other) = delete;
        FieldGenerator& operator=(const FieldGenerator& other) = delete;
        virtual ~FieldGenerator() = default;

    public:
        virtual void GenerateFieldDeclaration(Entities& formatter) = 0;
        void GenerateFieldConstant(Entities& formatter);
        virtual void SerializerBody(google::protobuf::io::Printer& printer) = 0;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) = 0;
        virtual void GenerateConstructorParameter(Constructor& constructor) = 0;

    protected:
        const google::protobuf::FieldDescriptor& descriptor;
    };

    class FieldGeneratorString
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration(Entities& formatter) override;
        virtual void SerializerBody(google::protobuf::io::Printer& printer) override;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) override;
        virtual void GenerateConstructorParameter(Constructor& constructor) override;
    };

    class FieldGeneratorRepeatedString
        : public FieldGenerator
    {
    public:
        FieldGeneratorRepeatedString(const google::protobuf::FieldDescriptor& descriptor);

        virtual void GenerateFieldDeclaration(Entities& formatter) override;
        virtual void SerializerBody(google::protobuf::io::Printer& printer) override;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) override;
        virtual void GenerateConstructorParameter(Constructor& constructor) override;

    private:
        uint32_t stringSize = 0;
        uint32_t arraySize = 0;
    };

    class FieldGeneratorUint32
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration(Entities& formatter) override;
        virtual void SerializerBody(google::protobuf::io::Printer& printer) override;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) override;
        virtual void GenerateConstructorParameter(Constructor& constructor) override;
    };

    class FieldGeneratorMessage
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration(Entities& formatter) override;
        virtual void SerializerBody(google::protobuf::io::Printer& printer) override;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) override;
        virtual void GenerateConstructorParameter(Constructor& constructor) override;
    };

    class FieldGeneratorRepeatedMessage
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration(Entities& formatter) override;
        virtual void SerializerBody(google::protobuf::io::Printer& printer) override;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) override;
        virtual void GenerateConstructorParameter(Constructor& constructor) override;
    };

    class MessageGenerator
    {
    public:
        MessageGenerator(const google::protobuf::Descriptor& descriptor, Entities& formatter);
        MessageGenerator(const MessageGenerator& other) = delete;
        MessageGenerator& operator=(const MessageGenerator& other) = delete;
        ~MessageGenerator() = default;

    private:
        void CreateFieldGenerators();
        void CreateFieldGenerator(const google::protobuf::FieldDescriptor& fieldDescriptor);
        void GenerateConstructors();
        void GenerateFunctions();
        void GenerateNestedMessageForwardDeclarations();
        void GenerateNestedMessages();
        void GenerateFieldDeclarations();
        void GenerateFieldConstants();
        std::string SerializerBody();
        std::string DeserializerBody();

    private:
        const google::protobuf::Descriptor& descriptor;
        Class* classFormatter;
        std::vector<std::unique_ptr<MessageGenerator>> messageGenerators;
        std::vector<std::unique_ptr<FieldGenerator>> fieldGenerators;
    };

    class CppInfraGenerator
    {
    public:
        CppInfraGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file);
        CppInfraGenerator(const CppInfraGenerator& other) = delete;
        CppInfraGenerator& operator=(const CppInfraGenerator& other) = delete;
        ~CppInfraGenerator() = default;

    public:
        void GenerateHeader();
        void GenerateSource();

    private:
        void GenerateTopHeaderGuard();
        void GenerateBottomHeaderGuard();

    private:
        google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> stream;
        google::protobuf::io::Printer printer;
        Entities formatter;
        const google::protobuf::FileDescriptor* file;

        std::vector<std::unique_ptr<MessageGenerator>> messageGenerators;
    };
}

#endif
