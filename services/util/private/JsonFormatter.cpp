#include "services/util/public/JsonFormatter.hpp"

namespace services
{
    JsonFormatter::JsonFormatter(infra::TextOutputStream& stream)
        : stream(&stream)
    {
        stream.SetSoftFail(true);
        stream << "{ ";
    }

    JsonFormatter::JsonFormatter(JsonFormatter&& other)
        : stream(other.stream)
    {}

    JsonFormatter& JsonFormatter::operator=(JsonFormatter&& other)
    {
        stream = other.stream;

        return *this;
    }

    JsonFormatter::~JsonFormatter()
    {
        *stream << " }";
        stream->HasFailed();
    }

    void JsonFormatter::Add(const char* tagName, bool tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": )" << (tag ? "true" : "false");
    }

    void JsonFormatter::Add(const char* tagName, int32_t tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": )" << tag;
    }

    void JsonFormatter::Add(const char* tagName, uint32_t tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": )" << tag;
    }

    void JsonFormatter::Add(const char* tagName, const char* tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": ")" << tag << '"';
    }

    void JsonFormatter::Add(const char* tagName, infra::BoundedConstString tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": ")" << tag << '"';
    }

    JsonFormatter JsonFormatter::SubObject(const char* tagName)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(": )";

        return JsonFormatter(*stream);
    }

    bool JsonFormatter::HasFailed() const
    {
        return stream->HasFailed();
    }

    void JsonFormatter::InsertSeparation()
    {
        if (!empty)
            *stream << ", ";

        empty = false;
    }
}
