#ifndef PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP
#define PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP

#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/descriptor.h"
#include "protobuf/protoc_echo_plugin/CppFormatter.hpp"

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

    struct UnspecifiedServiceId
    {
        std::string service;
    };

    struct UnspecifiedMethodId
    {
        std::string service;
        std::string method;
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
        explicit FieldGenerator(const google::protobuf::FieldDescriptor& descriptor);
        FieldGenerator(const FieldGenerator& other) = delete;
        FieldGenerator& operator=(const FieldGenerator& other) = delete;
        virtual ~FieldGenerator() = default;

    public:
        virtual void GenerateFieldDeclaration(Entities& formatter) = 0;
        void GenerateFieldConstant(Entities& formatter);
        virtual void SerializerBody(google::protobuf::io::Printer& printer) = 0;
        virtual void DeserializerBody(google::protobuf::io::Printer& printer) = 0;
        void CompareEqualBody(google::protobuf::io::Printer& printer);
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
        explicit FieldGeneratorRepeatedString(const google::protobuf::FieldDescriptor& descriptor);

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
        std::string CompareEqualBody() const;
        std::string CompareUnEqualBody() const;

    private:
        const google::protobuf::Descriptor& descriptor;
        Class* classFormatter;
        std::vector<std::shared_ptr<MessageGenerator>> messageGenerators;
        std::vector<std::shared_ptr<FieldGenerator>> fieldGenerators;
    };

    class ServiceGenerator
    {
    public:
        ServiceGenerator(const google::protobuf::ServiceDescriptor& service, Entities& formatter);
        ServiceGenerator(const ServiceGenerator& other) = delete;
        ServiceGenerator& operator=(const ServiceGenerator& other) = delete;
        ~ServiceGenerator() = default;

    private:
        void GenerateServiceConstructors();
        void GenerateServiceProxyConstructors();
        void GenerateServiceFunctions();
        void GenerateServiceProxyFunctions();
        void GenerateFieldConstants();

        std::string HandleBody() const;
        std::string ProxyMethodBody(const google::protobuf::MethodDescriptor& descriptor) const;
        std::string QualifiedName(const google::protobuf::Descriptor& descriptor) const;

    private:
        const google::protobuf::ServiceDescriptor& service;
        Class* serviceFormatter;
        Class* serviceProxyFormatter;
    };

    class EchoGenerator
    {
    public:
        EchoGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file);
        EchoGenerator(const EchoGenerator& other) = delete;
        EchoGenerator& operator=(const EchoGenerator& other) = delete;
        ~EchoGenerator() = default;

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

        std::vector<std::shared_ptr<MessageGenerator>> messageGenerators;
        std::vector<std::shared_ptr<ServiceGenerator>> serviceGenerators;
    };
}

#endif
