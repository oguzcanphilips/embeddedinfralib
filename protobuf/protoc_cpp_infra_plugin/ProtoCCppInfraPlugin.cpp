#include "generated/proto_cpp/field_size.pb.h"
#include "google/protobuf/compiler/cpp/cpp_helpers.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/stubs/strutil.h"
#include "protobuf/protoc_cpp_infra_plugin/ProtoCCppInfraPlugin.hpp"
#include <string>

namespace application
{
    bool CppInfraCodeGenerator::Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter,
        google::protobuf::compiler::GeneratorContext* generatorContext, std::string* error) const
    {
        try
        {
            std::string basename = google::protobuf::compiler::cpp::StripProto(file->name()) + ".pb";

            CppInfraGenerator headerGenerator(generatorContext, basename + ".hpp", file);
            headerGenerator.GenerateHeader();
            CppInfraGenerator sourceGenerator(generatorContext, basename + ".cpp", file);
            sourceGenerator.GenerateSource();

            return true;
        }
        catch (UnsupportedFieldType& exception)
        {
            *error = "Unsupported field type " + google::protobuf::SimpleItoa(exception.type) + " of field " + exception.fieldName;
            return false;
        }
        catch (UnspecifiedStringSize& exception)
        {
            *error = "Field " + exception.fieldName + " needs a string_size specifying its maximum number of characters";
            return false;
        }
        catch (UnspecifiedArraySize& exception)
        {
            *error = "Field " + exception.fieldName + " needs an array_size specifying its maximum number of elements";
            return false;
        }
    }

    FieldGenerator::FieldGenerator(const google::protobuf::FieldDescriptor& descriptor, google::protobuf::io::Printer& printer)
        : descriptor(descriptor)
        , printer(printer)
    {}

    void FieldGenerator::GenerateFieldConstant()
    {
        printer.Print("    static const uint32_t $name$ = $index$;\n"
            , "name", google::protobuf::compiler::cpp::FieldConstantName(&descriptor)
            , "index", google::protobuf::SimpleItoa(descriptor.number()));
    }

    void FieldGeneratorString::GenerateFieldDeclaration()
    {
        uint32_t stringSize = descriptor.options().GetExtension(string_size);
        if (stringSize == 0)
            throw UnspecifiedStringSize{ descriptor.name() };

        printer.Print("    infra::BoundedString::WithStorage<$size$> $name$;\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "size", google::protobuf::SimpleItoa(stringSize));
    }

    void FieldGeneratorString::GenerateSerializer()
    {
        printer.Print("    formatter.PutStringField($name$, $constant$);\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorString::GenerateDeserializer()
    {
        printer.Print(R"(            case $constant$:
                field.first.Get<services::ProtoLengthDelimited>().GetString($name$);
                break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorString::GenerateConstructorParameter()
    {
        printer.Print("infra::BoundedConstString $name$", "name", google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    FieldGeneratorRepeatedString::FieldGeneratorRepeatedString(const google::protobuf::FieldDescriptor& descriptor, google::protobuf::io::Printer& printer)
        : FieldGenerator(descriptor, printer)
        , stringSize(descriptor.options().GetExtension(string_size))
        , arraySize(descriptor.options().GetExtension(array_size))
    {
        if (stringSize == 0)
            throw UnspecifiedStringSize{ descriptor.name() };
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };
    }

    void FieldGeneratorRepeatedString::GenerateFieldDeclaration()
    {
        printer.Print("    infra::BoundedVector<infra::BoundedString::WithStorage<$string_size$>>::WithMaxSize<$array_size$> $name$;\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "string_size", google::protobuf::SimpleItoa(stringSize)
            , "array_size", google::protobuf::SimpleItoa(arraySize));
    }

    void FieldGeneratorRepeatedString::GenerateSerializer()
    {
        printer.Print(R"(    for (auto& subField : $name$)
        formatter.PutStringField(subField, $constant$);
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedString::GenerateDeserializer()
    {
        printer.Print(R"(            case $constant$:
                $name$.emplace_back();
                field.first.Get<services::ProtoLengthDelimited>().GetString($name$.back());
                break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedString::GenerateConstructorParameter()
    {
        printer.Print("const infra::BoundedVector<infra::BoundedString::WithStorage<$string_size$>>& $name$"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "string_size", google::protobuf::SimpleItoa(stringSize));
    }

    void FieldGeneratorUint32::GenerateFieldDeclaration()
    {
        printer.Print("    uint32_t $name$;\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    void FieldGeneratorUint32::GenerateSerializer()
    {
        printer.Print("    formatter.PutVarIntField($name$, $constant$);\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorUint32::GenerateDeserializer()
    {
        printer.Print(R"(            case $constant$:
                $name$ = static_cast<uint32_t>(field.first.Get<uint64_t>());
                break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorUint32::GenerateConstructorParameter()
    {
        printer.Print("uint32_t $name$", "name", google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    void FieldGeneratorMessage::GenerateFieldDeclaration()
    {
        printer.Print("    $type$ $name$;\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "type", descriptor.message_type()->name());
    }

    void FieldGeneratorMessage::GenerateSerializer()
    {
        printer.Print(R"(    {
        services::ProtoLengthDelimitedFormatter nestedMessage(formatter, $constant$);
        $name$.Serialize(formatter);
    }
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorMessage::GenerateDeserializer()
    {
        printer.Print(R"(            case $constant$:
                $name$.Deserialize(parser);
                break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorMessage::GenerateConstructorParameter()
    {
        printer.Print("const $type$& $name$"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "type", descriptor.message_type()->name());
    }

    void FieldGeneratorRepeatedMessage::GenerateFieldDeclaration()
    {
        uint32_t arraySize = descriptor.options().GetExtension(array_size);
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };

        printer.Print("    infra::BoundedVector<$type$>::WithMaxSize<$size$> $name$;\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "type", descriptor.message_type()->name()
            , "size", google::protobuf::SimpleItoa(arraySize));
    }

    void FieldGeneratorRepeatedMessage::GenerateSerializer()
    {
        printer.Print(R"(    if (!$name$.empty())
    {
        services::ProtoLengthDelimitedFormatter subFormatter = formatter.LengthDelimitedFormatter($constant$);

        for (auto& subField : $name$)
            subField.Serialize(formatter);
    }
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedMessage::GenerateDeserializer()
    {
        printer.Print(R"(            case $constant$:
                $name$.emplace_back(field.first.Get<services::ProtoLengthDelimited>().Parser());
                break;
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedMessage::GenerateConstructorParameter()
    {
        printer.Print("const infra::BoundedVector<$type$>& $name$"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "type", descriptor.message_type()->name());
    }

    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor& descriptor, google::protobuf::io::Printer& printer)
        : descriptor(descriptor)
        , printer(printer)
    {
        for (int i = 0; i != descriptor.nested_type_count(); ++i)
            messageGenerators.emplace_back(std::make_unique<MessageGenerator>(*descriptor.nested_type(i), printer));

        CreateFieldGenerators();
    }

    void MessageGenerator::GenerateClassDefinition()
    {
        GenerateClassHeader();
        printer.Indent(); printer.Indent();
        GenerateNestedMessageHeader();
        printer.Outdent(); printer.Outdent();
        GenerateFields();
        GenerateFieldConstants();
        GenerateClassFooter();
    }

    void MessageGenerator::GenerateClassImplementation()
    {
        GenerateConstructorImplementations();
        GenerateSerializer();
        GenerateDeserializer();
        GenerateNestedClassImplementations();
    }

    void MessageGenerator::CreateFieldGenerators()
    {
        for (int i = 0; i != descriptor.field_count(); ++i)
            CreateFieldGenerator(*descriptor.field(i));
    }

    void MessageGenerator::CreateFieldGenerator(const google::protobuf::FieldDescriptor& fieldDescriptor)
    {
        if (fieldDescriptor.label() == google::protobuf::FieldDescriptor::LABEL_REPEATED)
            switch (fieldDescriptor.type())
            {
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    fieldGenerators.emplace_back(std::make_unique<FieldGeneratorRepeatedString>(fieldDescriptor, printer));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    fieldGenerators.emplace_back(std::make_unique<FieldGeneratorRepeatedMessage>(fieldDescriptor, printer));
                    break;
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
        else
            switch (fieldDescriptor.type())
            {
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    fieldGenerators.emplace_back(std::make_unique<FieldGeneratorString>(fieldDescriptor, printer));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    fieldGenerators.emplace_back(std::make_unique<FieldGeneratorMessage>(fieldDescriptor, printer));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    fieldGenerators.emplace_back(std::make_unique<FieldGeneratorUint32>(fieldDescriptor, printer));
                    break;
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
    }

    void MessageGenerator::GenerateClassHeader()
    {
        printer.Print(R"(class $classname$
{
)", "classname", descriptor.name());
        if (!messageGenerators.empty())
        {
            printer.Print("public:\n");
            printer.Indent(); printer.Indent();

            for (auto& messageGenerator : messageGenerators)
                messageGenerator->GenerateForwardDeclaration();

            printer.Outdent(); printer.Outdent();
        }

        printer.Print(R"(public:
    $classname$() = default;
)", "classname", descriptor.name());

        printer.Print("    $classname$(", "classname", descriptor.name());

        for (auto& fieldGenerator : fieldGenerators)
        {
            if (&fieldGenerator != &fieldGenerators.front())
                printer.Print(", ");

            fieldGenerator->GenerateConstructorParameter();
        }

        printer.Print(");\n");

        printer.Print(R"(    $classname$(services::ProtoParser& parser);

    void Serialize(services::ProtoFormatter& formatter);
    void Deserialize(services::ProtoParser& parser);

public:
)", "classname", descriptor.name());
    }

    void MessageGenerator::GenerateClassFooter()
    {
        printer.Print("};\n\n");
    }

    void MessageGenerator::GenerateNestedMessageHeader()
    {
        for (auto& messageGenerator : messageGenerators)
            messageGenerator->GenerateClassDefinition();
    }

    void MessageGenerator::GenerateFields()
    {
        for (auto& fieldGenerator : fieldGenerators)
            fieldGenerator->GenerateFieldDeclaration();
    }

    void MessageGenerator::GenerateFieldConstants()
    {
        printer.Print("\n");

        for (auto& fieldGenerator : fieldGenerators)
            fieldGenerator->GenerateFieldConstant();
    }

    void MessageGenerator::GenerateConstructorImplementations()
    {
        printer.Print(R"($fullclassname$::$classname$(services::ProtoParser& parser)
{
    Deserialize(parser);
}

)"
            , "classname", descriptor.name()
            , "fullclassname", FullClassName());
    }

    void MessageGenerator::GenerateSerializer()
    {
        GenerateSerializerHeader();
        GenerateSerializerFields();
        GenerateSerializerFooter();
    }

    void MessageGenerator::GenerateSerializerHeader()
    {
        printer.Print(R"(void $fullclassname$::Serialize(services::ProtoFormatter& formatter)
{
)"
            , "fullclassname", FullClassName());
    }

    void MessageGenerator::GenerateSerializerFooter()
    {
        printer.Print("}\n\n");
    }

    void MessageGenerator::GenerateSerializerFields()
    {
        for (auto& fieldGenerator : fieldGenerators)
            fieldGenerator->GenerateSerializer();
    }

    void MessageGenerator::GenerateDeserializer()
    {
        GenerateDeserializerHeader();
        GenerateDeserializerFields();
        GenerateDeserializerFooter();
    }

    void MessageGenerator::GenerateNestedClassImplementations()
    {
        for (auto& messageGenerator : messageGenerators)
            messageGenerator->GenerateClassImplementation();
    }

    void MessageGenerator::GenerateForwardDeclaration()
    {
        printer.Print("class $classname$;\n", "classname", descriptor.name());
    }

    void MessageGenerator::GenerateDeserializerHeader()
    {
        printer.Print(R"(void $fullclassname$::Deserialize(services::ProtoParser& parser)
{
    while (!parser.Empty())
    {
        services::ProtoParser::Field field = parser.GetField();

        switch (field.second)
        {
)"
            , "fullclassname", FullClassName());
    }

    void MessageGenerator::GenerateDeserializerFooter()
    {
        printer.Print(R"(            default:
                if (field.first.Is<services::ProtoLengthDelimited>())
                    field.first.Get<services::ProtoLengthDelimited>().SkipEverything();
                break;
        }
    }
}

)");
    }

    void MessageGenerator::GenerateDeserializerFields()
    {
        for (auto& fieldGenerator : fieldGenerators)
            fieldGenerator->GenerateDeserializer();
    }

    std::string MessageGenerator::FullClassName() const
    {
        std::string result = descriptor.name();

        for (const google::protobuf::Descriptor* parent = descriptor.containing_type(); parent != nullptr; parent = parent->containing_type())
            result = parent->name() + "::" + result;

        return result;
    }

    CppInfraGenerator::CppInfraGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file)
        : stream(generatorContext->Open(name))
        , printer(stream.get(), '$', nullptr)
        , file(file)
    {
        for (int i = 0; i != file->message_type_count(); ++i)
            messageGenerators.emplace_back(std::make_unique<MessageGenerator>(*file->message_type(i), printer));
    }

    void CppInfraGenerator::GenerateHeader()
    {
        GenerateTopHeaderGuard();
        GenerateNamespaceOpeners();
        GenerateMessageClassDefinitions();
        GenerateNamespaceClosers();
        GenerateBottomHeaderGuard();
    }

    void CppInfraGenerator::GenerateSource()
    {
        GenerateSourceIncludes();
        GenerateNamespaceOpeners();
        GenerateMessageClassImplementations();
        GenerateNamespaceClosers();
    }

    void CppInfraGenerator::GenerateTopHeaderGuard()
    {
        std::string filename_identifier = google::protobuf::compiler::cpp::FilenameIdentifier(file->name());

        printer.Print(R"(// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: $filename$

$hash$ifndef PROTOBUF_CPP_INFRA_$filename_identifier$
$hash$define PROTOBUF_CPP_INFRA_$filename_identifier$

$hash$include "infra/util/BoundedString.hpp"
$hash$include "infra/util/BoundedVector.hpp"
$hash$include "protobuf/protobuf_cpp_infra/ProtoFormatter.hpp"
$hash$include "protobuf/protobuf_cpp_infra/ProtoParser.hpp")"
            , "filename", file->name(), "filename_identifier", filename_identifier, "hash" , "#");
        printer.Print("\n");
    }

    void CppInfraGenerator::GenerateBottomHeaderGuard()
    {
        printer.Print("#endif\n");
    }

    void CppInfraGenerator::GenerateNamespaceOpeners()
    {
        std::vector<std::string> packageParts = google::protobuf::Split(file->package(), ".", true);

        if (!packageParts.empty())
            printer.Print("\n");

        for (std::size_t i = 0; i != packageParts.size(); ++i)
        {
            printer.Print("namespace $part$\n{\n", "part", packageParts[i]);
            printer.Indent(); printer.Indent();
        }
    }

    void CppInfraGenerator::GenerateNamespaceClosers()
    {
        std::vector<std::string> packageParts = google::protobuf::Split(file->package(), ".", true);

        for (std::size_t i = 0; i != packageParts.size(); ++i)
        {
            printer.Outdent(); printer.Outdent();
            printer.Print("}\n", "part", packageParts[i]);
        }

        if (!packageParts.empty())
            printer.Print("\n");
    }

    void CppInfraGenerator::GenerateMessageClassDefinitions()
    {
        for (auto& messageGenerator : messageGenerators)
            messageGenerator->GenerateClassDefinition();
    }

    void CppInfraGenerator::GenerateMessageClassImplementations()
    {
        for (auto& messageGenerator : messageGenerators)
            messageGenerator->GenerateClassImplementation();
    }

    void CppInfraGenerator::GenerateSourceIncludes()
    {
        printer.Print(R"(#include "generated/proto_cpp_infra/$name$.pb.hpp")""\n", "name", google::protobuf::compiler::cpp::StripProto(file->name()));
    }
}
