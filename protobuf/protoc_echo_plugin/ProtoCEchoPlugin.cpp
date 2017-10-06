#include "generated/proto_cpp/echo_attributes.pb.h"
#include "google/protobuf/compiler/cpp/cpp_helpers.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/stubs/strutil.h"
#include "protobuf/protoc_echo_plugin/ProtoCEchoPlugin.hpp"
#include <sstream>

namespace application
{
    namespace
    {
        uint32_t MaxVarIntSize(uint32_t value)
        {
            uint32_t result = 1;

            while (value > 127)
            {
                value >>= 7;
                ++result;
            }

            return result;
        }
    }

    bool CppInfraCodeGenerator::Generate(const google::protobuf::FileDescriptor* file, const std::string& parameter,
        google::protobuf::compiler::GeneratorContext* generatorContext, std::string* error) const
    {
        try
        {
            std::string basename = google::protobuf::compiler::cpp::StripProto(file->name()) + ".pb";

            EchoGenerator headerGenerator(generatorContext, basename + ".hpp", file);
            headerGenerator.GenerateHeader();
            EchoGenerator sourceGenerator(generatorContext, basename + ".cpp", file);
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
        catch (UnspecifiedServiceId& exception)
        {
            *error = "Field " + exception.service + " needs a service_id specifying its id";
            return false;
        }
        catch (UnspecifiedMethodId& exception)
        {
            *error = "Field " + exception.service + "." + exception.method + " needs a method_id specifying its id";
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

    void FieldGenerator::CompareEqualBody(google::protobuf::io::Printer& printer)
    {
        printer.Print("\n&& $name$ == other.$name$", "name", google::protobuf::compiler::cpp::FieldName(&descriptor));
    }

    FieldGeneratorString::FieldGeneratorString(const google::protobuf::FieldDescriptor& descriptor)
        : FieldGenerator(descriptor)
        , stringSize(descriptor.options().GetExtension(string_size))
    {
        if (stringSize == 0)
            throw UnspecifiedStringSize{ descriptor.name() };
    }

    void FieldGeneratorString::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), "infra::BoundedString::WithStorage<" + google::protobuf::SimpleItoa(stringSize) + ">"));
    }

    std::string FieldGeneratorString::MaxMessageSize() const
    {
        return google::protobuf::SimpleItoa(stringSize + MaxVarIntSize(stringSize) + MaxVarIntSize((descriptor.number() << 3) | 2));
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
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
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

    std::string FieldGeneratorRepeatedString::MaxMessageSize() const
    {
        return google::protobuf::SimpleItoa(arraySize * (stringSize + MaxVarIntSize(stringSize) + MaxVarIntSize((descriptor.number() << 3) | 2)));
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
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
    }

    void FieldGeneratorUint32::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), "uint32_t"));
    }

    std::string FieldGeneratorUint32::MaxMessageSize() const
    {
        return google::protobuf::SimpleItoa(MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((descriptor.number() << 3) | 2));
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
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
    }

    FieldGeneratorRepeatedUint32::FieldGeneratorRepeatedUint32(const google::protobuf::FieldDescriptor& descriptor)
        : FieldGenerator(descriptor)
        , arraySize(descriptor.options().GetExtension(array_size))
    {
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };
    }

    void FieldGeneratorRepeatedUint32::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "infra::BoundedVector<uint32_t>::WithMaxSize<" + google::protobuf::SimpleItoa(arraySize) + ">"));
    }

    std::string FieldGeneratorRepeatedUint32::MaxMessageSize() const
    {
        return google::protobuf::SimpleItoa(arraySize * (MaxVarIntSize(std::numeric_limits<uint32_t>::max()) + MaxVarIntSize((descriptor.number() << 3) | 2)));
    }

    void FieldGeneratorRepeatedUint32::SerializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(for (auto& subField : $name$)
    formatter.PutVarIntField(subField, $constant$);
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedUint32::DeserializerBody(google::protobuf::io::Printer& printer)
    {
        printer.Print(R"(case $constant$:
    $name$.push_back(static_cast<uint32_t>(field.first.Get<uint64_t>()));
    break;
)"
, "name", google::protobuf::compiler::cpp::FieldName(&descriptor)
, "constant", google::protobuf::compiler::cpp::FieldConstantName(&descriptor));
    }

    void FieldGeneratorRepeatedUint32::GenerateConstructorParameter(Constructor& constructor)
    {
        constructor.Parameter("const infra::BoundedVector<uint32_t>& " + google::protobuf::compiler::cpp::FieldName(&descriptor));
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
    }

    void FieldGeneratorMessage::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor), descriptor.message_type()->name()));
    }

    std::string FieldGeneratorMessage::MaxMessageSize() const
    {
        return descriptor.message_type()->name() + "::maxMessageSize + " + google::protobuf::SimpleItoa(MaxVarIntSize((descriptor.number() << 3) | 2));
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
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
    }

    FieldGeneratorRepeatedMessage::FieldGeneratorRepeatedMessage(const google::protobuf::FieldDescriptor& descriptor)
        : FieldGenerator(descriptor)
        , arraySize(descriptor.options().GetExtension(array_size))
    {
        if (arraySize == 0)
            throw UnspecifiedArraySize{ descriptor.name() };
    }

    void FieldGeneratorRepeatedMessage::GenerateFieldDeclaration(Entities& entities)
    {
        entities.Add(std::make_shared<DataMember>(google::protobuf::compiler::cpp::FieldName(&descriptor)
            , "infra::BoundedVector<" + descriptor.message_type()->name() + ">::WithMaxSize<" + google::protobuf::SimpleItoa(arraySize) + ">"));
    }

    std::string FieldGeneratorRepeatedMessage::MaxMessageSize() const
    {
        return google::protobuf::SimpleItoa(arraySize) + " * " + descriptor.message_type()->name() + "::maxMessageSize + " + 
            google::protobuf::SimpleItoa(arraySize * MaxVarIntSize((descriptor.number() << 3) | 2));
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
        constructor.Initializer(google::protobuf::compiler::cpp::FieldName(&descriptor) + "(" + google::protobuf::compiler::cpp::FieldName(&descriptor) + ")");
    }

    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor& descriptor, Entities& formatter)
        : descriptor(descriptor)
    {
        auto class_ = std::make_shared<Class>(descriptor.name());
        classFormatter = class_.get();
        formatter.Add(class_);

        CreateFieldGenerators();
        GenerateNestedMessageForwardDeclarations();
        GenerateConstructors();
        GenerateFunctions();
        GenerateNestedMessages();
        GenerateFieldDeclarations();
        GenerateFieldConstants();
        GenerateMaxMessageSize();
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
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    fieldGenerators.emplace_back(std::make_shared<FieldGeneratorRepeatedUint32>(fieldDescriptor));
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
            constructors->Add(constructByMembers);
        }

        auto constructByProtoParser = std::make_shared<Constructor>(descriptor.name(), "Deserialize(parser);\n", 0);
        constructByProtoParser->Parameter("services::ProtoParser& parser");
        constructors->Add(constructByProtoParser);
        classFormatter->Add(constructors);
    }

    void MessageGenerator::GenerateFunctions()
    {
        auto functions = std::make_shared<Access>("public");

        auto serialize = std::make_shared<Function>("Serialize", SerializerBody(), "void", Function::fConst);
        serialize->Parameter("services::ProtoFormatter& formatter");
        functions->Add(serialize);

        auto deserialize = std::make_shared<Function>("Deserialize", DeserializerBody(), "void", 0);
        deserialize->Parameter("services::ProtoParser& parser");
        functions->Add(deserialize);

        auto compareEqual = std::make_shared<Function>("operator==", CompareEqualBody(), "bool", Function::fConst);
        compareEqual->Parameter("const " + descriptor.name() + "& other");
        functions->Add(compareEqual);

        auto compareUnEqual = std::make_shared<Function>("operator!=", CompareUnEqualBody(), "bool", Function::fConst);
        compareUnEqual->Parameter("const " + descriptor.name() + "& other");
        functions->Add(compareUnEqual);

        classFormatter->Add(functions);
    }

    void MessageGenerator::GenerateNestedMessageForwardDeclarations()
    {
        if (descriptor.nested_type_count() != 0)
        {
            auto forwardDeclarations = std::make_shared<Access>("public");

            for (int i = 0; i != descriptor.nested_type_count(); ++i)
                forwardDeclarations->Add(std::make_shared<ClassForwardDeclaration>(descriptor.nested_type(i)->name()));

            classFormatter->Add(forwardDeclarations);
        }
    }

    void MessageGenerator::GenerateNestedMessages()
    {
        if (descriptor.nested_type_count() != 0)
        {
            auto nestedMessages = std::make_shared<Access>("public");

            for (int i = 0; i != descriptor.nested_type_count(); ++i)
                MessageGenerator(*descriptor.nested_type(i), *nestedMessages);

            classFormatter->Add(nestedMessages);
        }
    }

    void MessageGenerator::GenerateFieldDeclarations()
    {
        if (!fieldGenerators.empty())
        {
            auto fields = std::make_shared<Access>("public");

            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->GenerateFieldDeclaration(*fields);

            classFormatter->Add(fields);
        }
    }

    void MessageGenerator::GenerateFieldConstants()
    {
        if (!fieldGenerators.empty())
        {
            auto fields = std::make_shared<Access>("public");

            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->GenerateFieldConstant(*fields);

            classFormatter->Add(fields);
        }
    }

    void MessageGenerator::GenerateMaxMessageSize()
    {
        auto fields = std::make_shared<Access>("public");

        std::string maxMessageSize = "0";
        for (auto& fieldGenerator : fieldGenerators)
        {
            maxMessageSize += " + " + fieldGenerator->MaxMessageSize();
        }

        fields->Add(std::make_shared<DataMember>("maxMessageSize = " + maxMessageSize, "static const uint32_t"));
        
        classFormatter->Add(fields);
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

)");
            if (!fieldGenerators.empty())
            {
                printer.Print(R"(    switch (field.second)
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
            else
                printer.Print(R"(    if (field.first.Is<services::ProtoLengthDelimited>())
        field.first.Get<services::ProtoLengthDelimited>().SkipEverything();
}
)");
        }

        return result.str();
    }

    std::string MessageGenerator::CompareEqualBody() const
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            printer.Print("return true");

            printer.Indent();
            for (auto& fieldGenerator : fieldGenerators)
                fieldGenerator->CompareEqualBody(printer);
            printer.Outdent();

            printer.Print(";\n");
        }

        return result.str();
    }

    std::string MessageGenerator::CompareUnEqualBody() const
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            printer.Print("return !(*this == other);\n");
        }

        return result.str();
    }

    ServiceGenerator::ServiceGenerator(const google::protobuf::ServiceDescriptor& service, Entities& formatter)
        : service(service)
        , serviceId(service.options().GetExtension(service_id))
    {
        if (serviceId == 0)
            throw UnspecifiedServiceId{ service.name() };

        auto serviceClass = std::make_shared<Class>(service.name());
        serviceClass->Parent("public services::Service");
        serviceFormatter = serviceClass.get();
        formatter.Add(serviceClass);

        auto serviceProxyClass = std::make_shared<Class>(service.name() + "Proxy");
        serviceProxyClass->Parent("public services::ServiceProxy");
        serviceProxyFormatter = serviceProxyClass.get();
        formatter.Add(serviceProxyClass);

        GenerateServiceConstructors();
        GenerateServiceProxyConstructors();
        GenerateServiceFunctions();
        GenerateServiceProxyFunctions();
        GenerateFieldConstants();
    }

    void ServiceGenerator::GenerateServiceConstructors()
    {
        auto constructors = std::make_shared<Access>("public");
        auto constructor = std::make_shared<Constructor>(service.name(), "", 0);
        constructor->Parameter("services::Echo& echo");
        constructor->Initializer("services::Service(echo, serviceId)");

        constructors->Add(constructor);
        serviceFormatter->Add(constructors);
    }

    void ServiceGenerator::GenerateServiceProxyConstructors()
    {
        auto constructors = std::make_shared<Access>("public");
        auto constructor = std::make_shared<Constructor>(service.name() + "Proxy", "", 0);
        constructor->Parameter("services::Echo& echo");
        constructor->Initializer("services::ServiceProxy(echo, serviceId, " + MaxMessageSize() + ")");

        constructors->Add(constructor);
        serviceProxyFormatter->Add(constructors);
    }

    void ServiceGenerator::GenerateServiceFunctions()
    {
        auto functions = std::make_shared<Access>("public");

        for (int i = 0; i != service.method_count(); ++i)
        {
            auto serviceMethod = std::make_shared<Function>(service.method(i)->name(), "", "void", Function::fVirtual | Function::fAbstract);
            serviceMethod->Parameter("const " + QualifiedName(*service.method(i)->input_type()) + "& argument");
            functions->Add(serviceMethod);
        }

        auto handle = std::make_shared<Function>("Handle", HandleBody(), "void", Function::fVirtual | Function::fOverride);
        handle->Parameter("uint32_t methodId");
        handle->Parameter("services::ProtoParser& parser");
        functions->Add(handle);

        serviceFormatter->Add(functions);
    }

    void ServiceGenerator::GenerateServiceProxyFunctions()
    {
        auto functions = std::make_shared<Access>("public");

        for (int i = 0; i != service.method_count(); ++i)
        {
            auto serviceMethod = std::make_shared<Function>(service.method(i)->name(), ProxyMethodBody(*service.method(i)), "void", 0);
            serviceMethod->Parameter("const " + QualifiedName(*service.method(i)->input_type()) + "& argument");
            functions->Add(serviceMethod);
        }

        serviceProxyFormatter->Add(functions);
    }

    void ServiceGenerator::GenerateFieldConstants()
    {
        auto fields = std::make_shared<Access>("private");

        fields->Add(std::make_shared<DataMember>("serviceId = " + google::protobuf::SimpleItoa(serviceId), "static const uint32_t"));

        for (int i = 0; i != service.method_count(); ++i)
        {
            uint32_t methodId = service.method(i)->options().GetExtension(method_id);
            if (methodId == 0)
                throw UnspecifiedMethodId{ service.name(), service.method(i)->name() };

            fields->Add(std::make_shared<DataMember>("id" + service.method(i)->name() + " = " + google::protobuf::SimpleItoa(methodId), "static const uint32_t"));
        }

        serviceFormatter->Add(fields);
        serviceProxyFormatter->Add(fields);
    }

    std::string ServiceGenerator::MaxMessageSize() const
    {
        std::string result = "0";
        
        for (int i = 0; i != service.method_count(); ++i)
        {
            uint32_t methodId = service.method(i)->options().GetExtension(method_id);
            if (methodId == 0)
                throw UnspecifiedMethodId{ service.name(), service.method(i)->name() };

            result = "std::max<uint32_t>(" + google::protobuf::SimpleItoa(MaxVarIntSize(serviceId) + MaxVarIntSize((methodId << 3) | 2)) + " + " + QualifiedName(*service.method(i)->input_type()) + "::maxMessageSize, " + result + ")";
        }

        return result;
    }

    std::string ServiceGenerator::HandleBody() const
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            printer.Print("switch (methodId)\n{\n");

            for (int i = 0; i != service.method_count(); ++i)
            {
                printer.Print(R"(    case id$name$:
    {
        $argument$ argument(parser);
        $name$(argument);
    }
)", "name", service.method(i)->name(), "argument", QualifiedName(*service.method(i)->input_type()));
            }

            printer.Print("}\n");
        }

        return result.str();
    }

    std::string ServiceGenerator::ProxyMethodBody(const google::protobuf::MethodDescriptor& descriptor) const
    {
        std::ostringstream result;
        {
            google::protobuf::io::OstreamOutputStream stream(&result);
            google::protobuf::io::Printer printer(&stream, '$', nullptr);

            printer.Print(R"(services::ProtoFormatter formatter(Rpc().SendStream());
formatter.PutVarInt(serviceId);
{
    services::ProtoLengthDelimitedFormatter argumentFormatter = formatter.LengthDelimitedFormatter(id$name$);
    argument.Serialize(formatter);
}
Rpc().Send();
)", "name", descriptor.name());
        }

        return result.str();
    }

    std::string ServiceGenerator::QualifiedName(const google::protobuf::Descriptor& descriptor) const
    {
        if (descriptor.containing_type() != nullptr)
            return QualifiedName(*descriptor.containing_type()) + "::" + descriptor.name();
        else
            return descriptor.name();
    }

    EchoGenerator::EchoGenerator(google::protobuf::compiler::GeneratorContext* generatorContext, const std::string& name, const google::protobuf::FileDescriptor* file)
        : stream(generatorContext->Open(name))
        , printer(stream.get(), '$', nullptr)
        , formatter(true)
        , file(file)
    {
        auto includesByHeader = std::make_shared<IncludesByHeader>();
        includesByHeader->Path("infra/util/BoundedString.hpp");
        includesByHeader->Path("infra/util/BoundedVector.hpp");
        includesByHeader->Path("protobuf/echo/Echo.hpp");
        includesByHeader->Path("protobuf/echo/ProtoFormatter.hpp");
        includesByHeader->Path("protobuf/echo/ProtoParser.hpp");
        formatter.Add(includesByHeader);
        auto includesBySource = std::make_shared<IncludesBySource>();
        includesBySource->Path("generated/echo/" + google::protobuf::compiler::cpp::StripProto(file->name()) + ".pb.hpp");
        formatter.Add(includesBySource);

        Entities* currentEntity = &formatter;
        std::vector<std::string> packageParts = google::protobuf::Split(file->package(), ".", true);
        for (auto& package : packageParts)
        {
            auto newNamespace = std::make_shared<Namespace>(package);
            auto newEntity = newNamespace.get();
            currentEntity->Add(newNamespace);
            currentEntity = newEntity;
        }

        for (int i = 0; i != file->message_type_count(); ++i)
            messageGenerators.emplace_back(std::make_shared<MessageGenerator>(*file->message_type(i), *currentEntity));

        for (int i = 0; i != file->service_count(); ++i)
            serviceGenerators.emplace_back(std::make_shared<ServiceGenerator>(*file->service(i), *currentEntity));
    }

    void EchoGenerator::GenerateHeader()
    {
        GenerateTopHeaderGuard();
        formatter.PrintHeader(printer);
        GenerateBottomHeaderGuard();
    }

    void EchoGenerator::GenerateSource()
    {
        printer.Print(R"(// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: $filename$

)"
, "filename", file->name());

        formatter.PrintSource(printer, "");
    }

    void EchoGenerator::GenerateTopHeaderGuard()
    {
        std::string filename_identifier = google::protobuf::compiler::cpp::FilenameIdentifier(file->name());

        printer.Print(R"(// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: $filename$

#ifndef echo_$filename_identifier$
#define echo_$filename_identifier$

)"
            , "filename", file->name(), "filename_identifier", filename_identifier);
    }

    void EchoGenerator::GenerateBottomHeaderGuard()
    {
        printer.Print("\n#endif\n");
    }
}
