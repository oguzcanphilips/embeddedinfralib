#ifndef PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP
#define PROTOBUF_PROTO_C_CPP_INFRA_PLUGIN_HPP

#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/descriptor.h"

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
        FieldGenerator(const google::protobuf::FieldDescriptor& descriptor, google::protobuf::io::Printer& printer);
        FieldGenerator(const FieldGenerator& other) = delete;
        FieldGenerator& operator=(const FieldGenerator& other) = delete;
        virtual ~FieldGenerator() = default;

    public:
        virtual void GenerateFieldDeclaration() = 0;
        void GenerateFieldConstant();
        virtual void GenerateSerializer() = 0;
        virtual void GenerateDeserializer() = 0;
        virtual void GenerateConstructorParameter() = 0;

    protected:
        const google::protobuf::FieldDescriptor& descriptor;
        google::protobuf::io::Printer& printer;                                                                         //TICS !INT#002
    };

    class FieldGeneratorString
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration() override;
        virtual void GenerateSerializer() override;
        virtual void GenerateDeserializer() override;
        virtual void GenerateConstructorParameter() override;
    };

    class FieldGeneratorRepeatedString
        : public FieldGenerator
    {
    public:
        FieldGeneratorRepeatedString(const google::protobuf::FieldDescriptor& descriptor, google::protobuf::io::Printer& printer);

        virtual void GenerateFieldDeclaration() override;
        virtual void GenerateSerializer() override;
        virtual void GenerateDeserializer() override;
        virtual void GenerateConstructorParameter() override;

    private:
        uint32_t stringSize = 0;
        uint32_t arraySize = 0;
    };

    class FieldGeneratorUint32
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration() override;
        virtual void GenerateSerializer() override;
        virtual void GenerateDeserializer() override;
        virtual void GenerateConstructorParameter() override;
    };

    class FieldGeneratorMessage
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration() override;
        virtual void GenerateSerializer() override;
        virtual void GenerateDeserializer() override;
        virtual void GenerateConstructorParameter() override;
    };

    class FieldGeneratorRepeatedMessage
        : public FieldGenerator
    {
    public:
        using FieldGenerator::FieldGenerator;

        virtual void GenerateFieldDeclaration() override;
        virtual void GenerateSerializer() override;
        virtual void GenerateDeserializer() override;
        virtual void GenerateConstructorParameter() override;
    };

    class MessageGenerator
    {
    public:
        MessageGenerator(const google::protobuf::Descriptor& descriptor, google::protobuf::io::Printer& printer);
        MessageGenerator(const MessageGenerator& other) = delete;
        MessageGenerator& operator=(const MessageGenerator& other) = delete;
        ~MessageGenerator() = default;

    public:
        void GenerateClassDefinition();
        void GenerateClassImplementation();

    private:
        void CreateFieldGenerators();
        void CreateFieldGenerator(const google::protobuf::FieldDescriptor& fieldDescriptor);
        void GenerateClassHeader();
        void GenerateClassFooter();
        void GenerateNestedMessageHeader();
        void GenerateFields();
        void GenerateFieldConstants();
        void GenerateConstructorImplementations();
        void GenerateSerializer();
        void GenerateSerializerHeader();
        void GenerateSerializerFooter();
        void GenerateSerializerFields();
        void GenerateDeserializer();
        void GenerateDeserializerHeader();
        void GenerateDeserializerFooter();
        void GenerateDeserializerFields();
        void GenerateNestedClassImplementations();
        void GenerateForwardDeclaration();
        std::string FullClassName() const;

    private:
        const google::protobuf::Descriptor& descriptor;
        google::protobuf::io::Printer& printer;
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
        void GenerateNamespaceOpeners();
        void GenerateNamespaceClosers();
        void GenerateMessageClassDefinitions();
        void GenerateMessageClassImplementations();
        void GenerateSourceIncludes();

    private:
        google::protobuf::scoped_ptr<google::protobuf::io::ZeroCopyOutputStream> stream;
        google::protobuf::io::Printer printer;
        const google::protobuf::FileDescriptor* file;

        std::vector<std::unique_ptr<MessageGenerator>> messageGenerators;
    };
}

#endif
