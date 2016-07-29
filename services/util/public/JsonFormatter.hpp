#ifndef SERVICES_JSON_FORMATTER_HPP
#define SERVICES_JSON_FORMATTER_HPP

#include "infra/stream/public/StringOutputStream.hpp"
#include "infra/util/public/BoundedString.hpp"
#include "infra/util/public/WithStorage.hpp"

namespace services
{
    class JsonFormatter
    {
    public:
        using WithStringStream = infra::WithStorage<JsonFormatter, infra::StringOutputStream>;

        explicit JsonFormatter(infra::TextOutputStream& stream);
        JsonFormatter(const JsonFormatter& other) = delete;
        JsonFormatter(JsonFormatter&& other);
        JsonFormatter& operator=(const JsonFormatter& other) = delete;
        JsonFormatter& operator=(JsonFormatter&& other);
        ~JsonFormatter();

        void Add(const char* tagName, bool tag);
        void Add(const char* tagName, int32_t tag);
        void Add(const char* tagName, uint32_t tag);
        void Add(const char* tagName, const char* tag);
        void Add(const char* tagName, infra::BoundedConstString tag);
        JsonFormatter SubObject(const char* tagName);

        bool HasFailed() const;

    private:
        void InsertSeparation();

    private:
        infra::TextOutputStream* stream;
        bool empty = true;
    };
}

#endif
