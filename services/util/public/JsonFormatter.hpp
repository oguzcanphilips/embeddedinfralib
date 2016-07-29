#ifndef SERVICES_JSON_FORMATTER_HPP
#define SERVICES_JSON_FORMATTER_HPP

#include "infra/stream/public/StringOutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace services
{
    class JsonObjectFormatter
    {
    public:
        using WithStringStream = infra::WithStorage<JsonObjectFormatter, infra::StringOutputStream>;

        explicit JsonObjectFormatter(infra::TextOutputStream& stream);
        JsonObjectFormatter(const JsonObjectFormatter& other) = delete;
        JsonObjectFormatter(JsonObjectFormatter&& other);
        JsonObjectFormatter& operator=(const JsonObjectFormatter& other) = delete;
        JsonObjectFormatter& operator=(JsonObjectFormatter&& other);
        ~JsonObjectFormatter();

        void Add(const char* tagName, bool tag);
        void Add(const char* tagName, int32_t tag);
        void Add(const char* tagName, uint32_t tag);
        void Add(const char* tagName, const char* tag);
        void Add(const char* tagName, infra::BoundedConstString tag);
        JsonObjectFormatter SubObject(const char* tagName);

        bool HasFailed() const;

    private:
        void InsertSeparation();

    private:
        infra::TextOutputStream* stream;
        bool empty = true;
    };

    class JsonArrayFormatter
    {
    public:
        using WithStringStream = infra::WithStorage<JsonArrayFormatter, infra::StringOutputStream>;

        explicit JsonArrayFormatter(infra::TextOutputStream& stream);
        JsonArrayFormatter(const JsonArrayFormatter& other) = delete;
        JsonArrayFormatter(JsonArrayFormatter&& other);
        JsonArrayFormatter& operator=(const JsonArrayFormatter& other) = delete;
        JsonArrayFormatter& operator=(JsonArrayFormatter&& other);
        ~JsonArrayFormatter();

        void Add(bool tag);
        void Add(int32_t tag);
        void Add(uint32_t tag);
        void Add(const char* tag);
        void Add(infra::BoundedConstString tag);
        JsonObjectFormatter SubObject();

        bool HasFailed() const;

    private:
        void InsertSeparation();

    private:
        infra::TextOutputStream* stream;
        bool empty = true;
    };
}

#endif
