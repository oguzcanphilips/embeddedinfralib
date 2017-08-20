#include "protobuf/protoc_cpp_infra_plugin/CppFormatter.hpp"
#include <functional>

namespace application
{
    template<class C>
    void ForEach(C& elements, std::function<void(typename C::value_type&)> each, std::function<void()> between)
    {
        if (!elements.empty())
        {
            each(elements.front());

            for (auto element = std::next(elements.begin()); element != elements.end(); ++element)
            {
                between();
                each(*element);
            }
        }
    }

    template<class C>
    void ForEach(const C& elements, std::function<void(const typename C::value_type&)> each, std::function<void()> between)
    {
        if (!elements.empty())
        {
            each(elements.front());

            for (auto element = std::next(elements.begin()); element != elements.end(); ++element)
            {
                between();
                each(*element);
            }
        }
    }

    void Entities::Add(std::unique_ptr<Entity>&& newEntity)
    {
        entities.push_back(std::move(newEntity));
    }

    void Entities::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        ForEach(entities, [&printer](const std::unique_ptr<Entity>& entity) { entity->PrintHeader(printer); }, [&printer]() { printer.Print("\n"); });
    }

    void Entities::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        ForEach(entities, [&printer, &scope](const std::unique_ptr<Entity>& entity) { entity->PrintSource(printer, scope); }, [&printer]() { printer.Print("\n"); });
    }

    Class::Class(const std::string& name)
        : name(name)
    {}

    void Class::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print(R"(class $name$
{
)", "name", name);
        printer.Indent();
        Entities::PrintHeader(printer);
        printer.Outdent();
        printer.Print(R"(};
)");
    }

    void Class::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        Entities::PrintSource(printer, scope + name + "::");
    }

    Access::Access(const std::string& level)
        : level(level)
    {}

    void Access::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("$level$:\n", "level", level);
        Entities::PrintHeader(printer);
    }

    Namespace::Namespace(const std::string& name)
        : name(name)
    {}

    void Namespace::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("namespace $name$\n{\n", "name", name);
        printer.Indent();
        Entities::PrintHeader(printer);
        printer.Outdent();
        printer.Print("}\n");
    }

    void Namespace::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        printer.Print("namespace $name$\n{\n", "name", name);
        printer.Indent();
        Entities::PrintSource(printer, scope);
        printer.Outdent();
        printer.Print("}\n");
    }

    Function::Function(const std::string& name, const std::string& body, const std::string& result, uint32_t flags)
        : name(name)
        , body(body)
        , result(result)
        , flags(flags)
    {}

    void Function::Parameter(const std::string& parameter)
    {
        parameters.push_back(parameter);
    }

    void Function::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("$virtual$$result$ $name$($parameters$)$const$$override$$abstract$;\n"
            , "result", result
            , "name", name
            , "parameters", Parameters()
            , "const", (flags & fConst) != 0 ? " const" : ""
            , "virtual", (flags & fVirtual) != 0 ? "virtual " : ""
            , "abstract", (flags & fAbstract) != 0 ? " = 0" : ""
            , "override", (flags & fOverride) != 0 ? " override" : "");
    }

    void Function::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        printer.Print(R"($result$ $scope$$name$($parameters$)$const$
{
    $body$}
)"
            , "result", result
            , "scope", scope
            , "name", name
            , "parameters", Parameters()
            , "const", (flags & fConst) != 0 ? " const" : ""
            , "body", body);
    }

    std::string Function::Parameters() const
    {
        std::string result;
        ForEach(parameters, [&result](const std::string& parameter) { result += parameter; }, [&result]() { result += ", "; });
        return result;
    }

    Constructor::Constructor(const std::string& name, const std::string& body, uint32_t flags)
        : name(name)
        , body(body)
        , flags(flags)
    {}

    void Constructor::Parameter(const std::string& parameter)
    {
        parameters.push_back(parameter);
    }

    void Constructor::Initializer(const std::string& initializer)
    {
        initializers.push_back(initializer);
    }

    void Constructor::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("$name$($parameters$)$default$$delete$;\n"
            , "name", name
            , "parameters", Parameters()
            , "default", (flags & cDefault) != 0 ? " = default" : ""
            , "delete", (flags & cDelete) != 0 ? " = delete" : "");
    }

    void Constructor::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        if ((flags & cDefault) == 0 && (flags & cDelete) == 0)
        {
            printer.Print(R"($scope$$name$($parameters$)
)"
                , "scope", scope
                , "name", name
                , "parameters", Parameters());

            printer.Indent();
            PrintInitializers(printer);
            printer.Outdent();
        
            printer.Print(R"({
    $body$}
)"
                , "body", body);
        };
    }

    std::string Constructor::Parameters() const
    {
        std::string result;
        ForEach(parameters, [&result](const std::string& parameter) { result += parameter; }, [&result]() { result += ", "; });
        return result;
    }

    void Constructor::PrintInitializers(google::protobuf::io::Printer& printer) const
    {
        std::string separator = ": ";
        
        for (auto& initializer : initializers)
        {
            printer.Print("$separator$$initializer$\n", "separator", separator, "initializer", initializer);
            separator = ", ";
        }
    }

    DataMember::DataMember(const std::string& name, const std::string& type)
        : name(name)
        , type(type)
    {}

    void DataMember::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("$type$ $name$;\n", "type", type, "name", name);
    }

    void DataMember::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {}

    IncludeByHeader::IncludeByHeader(const std::string& path)
        : path(path)
    {}

    void IncludeByHeader::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print(R"(#include "$path$"
)", "path", path);
    }

    void IncludeByHeader::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {}

    IncludeBySource::IncludeBySource(const std::string& path)
        : path(path)
    {}

    void IncludeBySource::PrintHeader(google::protobuf::io::Printer& printer) const
    {}

    void IncludeBySource::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {
        printer.Print(R"(#include "$path$"
)", "path", path);
    }

    ClassForwardDeclaration::ClassForwardDeclaration(const std::string& name)
        : name(name)
    {}

    void ClassForwardDeclaration::PrintHeader(google::protobuf::io::Printer& printer) const
    {
        printer.Print("class $name$;\n", "name", name);
    }

    void ClassForwardDeclaration::PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const
    {}
}
