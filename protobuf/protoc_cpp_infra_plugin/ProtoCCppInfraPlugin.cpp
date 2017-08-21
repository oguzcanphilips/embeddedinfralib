#include "generated/proto_cpp/field_size.pb.h"
#include "google/protobuf/compiler/cpp/cpp_helpers.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/stubs/strutil.h"
#include "protobuf/protoc_cpp_infra_plugin/ProtoCCppInfraPlugin.hpp"
#include <sstream>

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

    FieldGenerator::FieldGenerator(const google::protobuf::FieldDescriptor& descriptor)
        : descriptor(descriptor)
    {}

    void FieldGenerator::GenerateFieldConstant(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldConstantName(&descriptor) + " = " + google::protobuf::SimpleItoa(descriptor.number()), "static const uint32_t"));
    }

    void FieldGeneratorString::GenerateFieldDeclaration(Entities& entities)
    {
        uint32_t stringSize = descriptor.options().GetExtension(string_size);
        if (stringSize == 0)
            throw UnspecifiedStringSize{ descriptor.name() };

        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), "infra::BoundedString::WithStorage<" + google::protobuf::SimpleItoa(stringSize) + ">"));
    }

    void FieldGeneratorString::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print("formatter.PutStringField($name$, $constant$);\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorString::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    field.first.Get<services::ProtoLengthDelimited>().GetString($name$);
    break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorString::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("infra::BoundedConstString " + google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    FieldGeneratorRepeatedString::FieldGeneratorRepeatedString(const google::protobuf::FieldDescriptor& descriptor)
        : FieldGenerator(descriptor)
        , stringSize(descriptor.options().GetExtension(string_size))
        , arraySize(descriptor.options().GetExtension(array_size))
    {
        if (stringSize == 0)
            throw UnspecifiedStringSize{ descriptor.name() };
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };
    }

    void FieldGeneratorRepeatedString::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "infra::BoundedVector<infra::BoundedString::WithStorage<" + google::protobuf::SimpleItoa(stringSize) + ">>::WithMaxSize<" + google::protobuf::SimpleItoa(arraySize) + ">"));
    }

    void FieldGeneratorRepeatedString::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(for (auto& subField : $name$)
    formatter.PutStringField(subField, $constant$);
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedString::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    $name$.emplace_back();
    field.first.Get<services::ProtoLengthDelimited>().GetString($name$.back());
    break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedString::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("const infra::BoundedVector<infra::BoundedString::WithStorage<" + google::protobuf::SimpleItoa(stringSize) + ">>& " + google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    void FieldGeneratorUint32::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), "uint32_t"));
    }

    void FieldGeneratorUint32::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print("formatter.PutVarIntField($name$, $constant$);\n"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorUint32::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    $name$ = static_cast<uint32_t>(field.first.Get<uint64_t>());
    break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorUint32::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("uint32_t " + google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    void FieldGeneratorMessage::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), descriptor.message_type()->name()));
    }

    void FieldGeneratorMessage::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"({
    services::ProtoLengthDelimitedFormatter nestedMessage(formatter, $constant$);
    $name$.Serialize(formatter);
}
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorMessage::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    $name$.Deserialize(parser);
    break;
)"
            , "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorMessage::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("const " + descriptor.message_type()->name() + "& " + google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    void FieldGeneratorRepeatedMessage::GenerateFieldDeclaration(Entities& entities)
    {
        uint32_t arraySize = descriptor.options().GetExtension(array_size);
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };

        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "infra::BoundedVector<" + descriptor.message_type()->name() + ">::WithMaxSize<" + google::protobuf::SimpleItoa(arraySize) + ">"));
    }

    void FieldGeneratorRepeatedMessage::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(if (!$name$.empty())
{
    services::ProtoLengthDelimitedFormatter subFormatter = formatter.LengthDelimitedFormatter($constant$);

    for (auto& subField : $name$)
        subField.Serialize(formatter);
}
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedMessage::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    $name$.emplace_back(field.first.Get<services::ProtoLengthDelimited>().Parser());
    break;
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedMessage::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("const infra::BoundedVector<" + descriptor.message_type()->name() + ">& " + google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor& descriptor, Entities& formatter)
        : descriptor(descriptor)
    {
        auto class_ = std::make_shared<Class>(descriptor.name());
        classFormatter = class_.get();
        formatter.Add(std::move(class_));

        CreateFieldGenerators();
        GenerateNestedMessageForwardDeclarations();
        GenerateConstructors();
        GenerateFunctions();
        GenerateNestedMessages();
        GenerateFieldDeclarations();
        GenerateFieldConstants();
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
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorRepeatedString>(fieldDescriptor));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorRepeatedMessage>(fieldDescriptor));
                    break;
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
        else
            switch (fieldDescriptor.type())
            {
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorString>(fieldDescriptor));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorMessage>(fieldDescriptor));
                    break;
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorUint32>(fieldDescriptor));
                    break;
                default:
                    throw UnsupportedFieldType{ fieldDescriptor.name(), fieldDescriptor.type() };
            }
    }

    void MessageGenerator::GenerateConstructors()
    {
        auto constructors = std::make_shared<Access>("public");
        constructors->Add(std::make_shared<Constructor>(descriptor.name(), "", Constructor::cDefault));

        if (!fieldGenerators.empty())
        {
            auto constructByMembers = std::make_shared<Constructor>(descriptor.name(), "", 0);
            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->GenerateConstructorParameter(*constructByMembers);
            constructors->Add(std::move(constructByMembers));
        }

        auto constructByProtoParser = std::make_shared<Constructor>(descriptor.name(), "Deserialize(parser);\n", 0);
        constructByProtoParser->Parameter("services::ProtoParser& parser");
        constructors->Add(std::move(constructByProtoParser));
        classFormatter->Add(std::move(constructors));
    }

    void MessageGenerator::GenerateFunctions()
    {
        auto functions = std::make_shared<Access>("public");
        auto serialize = std::make_shared<Function>("Serialize", SerializerBody(), "void", 0);
        serialize->Parameter("services::ProtoFormatter& formatter");
        functions->Add(std::move(serialize));

        auto deserialize = std::make_shared<Function>("Deserialize", DeserializerBody(), "void", 0);
        deserialize->Parameter("services::ProtoParser& parser");
        functions->Add(std::move(deserialize));
        classFormatter->Add(std::move(functions));
    }

    void MessageGenerator::GenerateNestedMessageForwardDeclarations()
    {
        if (descriptor.nested_type_count() != 0)
        {
            auto forwardDeclarations = std::make_shared<Access>("public");

            for (int i = 0; i != descriptor.nested_type_count(); ++i)
                forwardDeclarations->Add(std::make_shared<ClassForwardDeclaration>(descriptor.nested_type(i)->name()));

            classFormatter->Add(std::move(forwardDeclarations));
        }
    }

    void MessageGenerator::GenerateNestedMessages()
    {
        if (descriptor.nested_type_count() != 0)
        {
            auto nestedMessages = std::make_shared<Access>("public");

            for (int i = 0; i != descriptor.nested_type_count(); ++i)
                MessageGenerator(*descriptor.nested_type(i), *nestedMessages);

            classFormatter->Add(std::move(nestedMessages));
        }
    }

    void MessageGenerator::GenerateFieldDeclarations()
    {
        if (!fieldGenerators.empty())
        {
            auto fields = std::make_shared<Access>("public");

            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->GenerateFieldDeclaration(*fields);

            classFormatter->Add(std::move(fields));
        }
    }

    void MessageGenerator::GenerateFieldConstants()
    {
        if (!fieldGenerators.empty())
        {
            auto fields = std::make_shared<Access>("public");

            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->GenerateFieldConstant(*fields);

            classFormatter->Add(std::move(fields));
        }
    }

    std::string MessageGenerator::SerializerBody()
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->SerializerBody(printer);
        }
     
        return result.str();
    }

    std::string MessageGenerator::DeserializerBody()
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            printer.Print(R"(while (!parser.Empty())
{
    services::ProtoParser::Field field = parser.GetField();

    switch (field.second)
    {
)");

            printer.Indent(); printer.Indent();
            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->DeserializerBody(printer);
            printer.Outdent(); printer.Outdent();
        
            printer.Print(R"(        default:
            if (field.first.Is<services::ProtoLengthDelimited>())
                field.first.Get<services::ProtoLengthDelimited>().SkipEverything();
            break;
    }
}
)");
        }

        return result.str();
    }

    CppInfraGenerator::CppInfraGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file)
        : stream(generatorContext->Open(name))
        , printer(stream.get(), '$', nullptr)
        , formatter(true)
        , file(file)
    {
        auto includesByHeader = std::make_shared<IncludesByHeader>();
        includesByHeader->Path("infra/util/BoundedString.hpp");
        includesByHeader->Path("infra/util/BoundedVector.hpp");
        includesByHeader->Path("protobuf/protobuf_cpp_infra/ProtoFormatter.hpp");
        includesByHeader->Path("protobuf/protobuf_cpp_infra/ProtoParser.hpp");
        formatter.Add(std::move(includesByHeader));
        auto includesBySource = std::make_shared<IncludesBySource>();
        includesBySource->Path("generated/proto_cpp_infra/" + google::protobuf::compiler::cpp::StripProto(file->name()) + ".pb.hpp");
        formatter.Add(std::move(includesBySource));

        Entities* currentEntity = &formatter;
        std::vector<std::string> packageParts = google::protobuf::Split(file->package(), ".", true);
        for (auto& package : packageParts)
        {
            auto newNamespace = std::make_shared<Namespace>(package);
            auto newEntity = newNamespace.get();
            currentEntity->Add(std::move(newNamespace));
            currentEntity = newEntity;
        }

        for (int i = 0; i != file->message_type_count(); ++i)
            messageGenerators.emplace_back(std::make_shared<MessageGenerator>(*file->message_type(i), *currentEntity));
    }

    void CppInfraGenerator::GenerateHeader()
    {
        GenerateTopHeaderGuard();
        formatter.PrintHeader(printer);
        GenerateBottomHeaderGuard();
    }

    void CppInfraGenerator::GenerateSource()
    {
        printer.Print(R"(// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: $filename$

)"
, "filename", file->name());

        formatter.PrintSource(printer, "");
    }

    void CppInfraGenerator::GenerateTopHeaderGuard()
    {
        std::string filename_identifier = google::protobuf::compiler::cpp::FilenameIdentifier(file->name());

        printer.Print(R"(// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: $filename$

#ifndef PROTOBUF_CPP_INFRA_$filename_identifier$
#define PROTOBUF_CPP_INFRA_$filename_identifier$

)"
            , "filename", file->name(), "filename_identifier", filename_identifier);
    }

    void CppInfraGenerator::GenerateBottomHeaderGuard()
    {
        printer.Print("\n#endif\n");
    }
}
