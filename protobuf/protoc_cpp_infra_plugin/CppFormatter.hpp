#ifndef PROTOBUF_CPP_FORMATTER_HPP
#define PROTOBUF_CPP_FORMATTER_HPP

#include "google/protobuf/io/printer.h"
#include <memory>
#include <vector>

namespace application
{
    class Entity
    {
    protected:
        Entity() = default;
        Entity(const Entity& other) = delete;
        Entity& operator=(const Entity& other) = delete;
        
    public:
        virtual ~Entity() = default;

    public:
        virtual void PrintHeader(google::protobuf::io::Printer& printer) const = 0;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const = 0;
    };

    class Entities
        : public Entity
    {
    public:
        explicit Entities(bool insertNewlineBetweenEntities);

        void Add(std::unique_ptr<Entity>&& newEntity);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        bool insertNewlineBetweenEntities;
        std::vector<std::unique_ptr<Entity>> entities;
    };

    class Class
        : public Entities
    {
    public:
        Class(const std::string& name);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string name;
    };

    class Access
        : public Entities
    {
    public:
        Access(const std::string& level);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;

    private:
        std::string level;
    };

    class Namespace
        : public Entities
    {
    public:
        Namespace(const std::string& name);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string name;
    };

    class Function
        : public Entity
    {
    public:
        static const uint32_t fConst = 1;
        static const uint32_t fVirtual = 2;
        static const uint32_t fAbstract = 4;
        static const uint32_t fOverride = 8;

        Function(const std::string& name, const std::string& body, const std::string& result, uint32_t flags);

        void Parameter(const std::string& parameter);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string Parameters() const;

    private:
        std::string name;
        std::string body;
        std::string result;
        uint32_t flags;

        std::vector<std::string> parameters;
    };

    class Constructor
        : public Entity
    {
    public:
        static const uint32_t cDefault = 1;
        static const uint32_t cDelete = 2;

        Constructor(const std::string& name, const std::string& body, uint32_t flags);

        void Parameter(const std::string& parameter);
        void Initializer(const std::string& initializer);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string Parameters() const;
        void PrintInitializers(google::protobuf::io::Printer& printer) const;

    private:
        std::string name;
        std::string body;
        uint32_t flags;

        std::vector<std::string> parameters;
        std::vector<std::string> initializers;
    };

    class DataMember
        : public Entity
    {
    public:
        DataMember(const std::string& name, const std::string& type);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string name;
        std::string type;
    };

    class IncludeByHeader
        : public Entity
    {
    public:
        IncludeByHeader(const std::string& path);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string path;
    };

    class IncludeBySource
        : public Entity
    {
    public:
        IncludeBySource(const std::string& path);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string path;
    };

    class ClassForwardDeclaration
        : public Entity
    {
    public:
        ClassForwardDeclaration(const std::string& name);

        virtual void PrintHeader(google::protobuf::io::Printer& printer) const override;
        virtual void PrintSource(google::protobuf::io::Printer& printer, const std::string& scope) const override;

    private:
        std::string name;
    };
}

#endif
