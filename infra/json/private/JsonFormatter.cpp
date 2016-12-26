#include "infra/json/public/JsonFormatter.hpp"

namespace infra
{
    JsonObjectFormatter::JsonObjectFormatter(infra::TextOutputStream& stream)
        : stream(&stream)
    {
        stream.SetNoFail();
        stream << "{ ";
    }

    JsonObjectFormatter::JsonObjectFormatter(JsonObjectFormatter&& other)
        : stream(other.stream)
    {}

    JsonObjectFormatter& JsonObjectFormatter::operator=(JsonObjectFormatter&& other)
    {
        stream = other.stream;

        return *this;
    }

    JsonObjectFormatter::~JsonObjectFormatter()
    {
        *stream << " }";
    }

    void JsonObjectFormatter::Add(const char* tagName, bool tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << (tag ? "true" : "false");
    }

    void JsonObjectFormatter::Add(const char* tagName, int32_t tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << tag;
    }

    void JsonObjectFormatter::Add(const char* tagName, uint32_t tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << tag;
    }

    void JsonObjectFormatter::Add(const char* tagName, int64_t tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << tag;
    }

    void JsonObjectFormatter::Add(const char* tagName, const char* tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":")" << tag << '"';
    }

    void JsonObjectFormatter::Add(const char* tagName, infra::BoundedConstString tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":")" << tag << '"';
    }

    JsonObjectFormatter JsonObjectFormatter::SubObject(infra::BoundedConstString tagName)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)";

        return JsonObjectFormatter(*stream);
    }

    JsonArrayFormatter JsonObjectFormatter::SubArray(infra::BoundedConstString tagName)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)";

        return JsonArrayFormatter(*stream);
    }

    bool JsonObjectFormatter::HasFailed() const
    {
        return stream->HasFailed();
    }

    void JsonObjectFormatter::InsertSeparation()
    {
        if (!empty)
            *stream << ", ";

        empty = false;
    }

    JsonArrayFormatter::JsonArrayFormatter(infra::TextOutputStream& stream)
        : stream(&stream)
    {
        stream.SetNoFail();
        stream << "[ ";
    }

    JsonArrayFormatter::JsonArrayFormatter(JsonArrayFormatter&& other)
        : stream(other.stream)
    {}

    JsonArrayFormatter& JsonArrayFormatter::operator=(JsonArrayFormatter&& other)
    {
        stream = other.stream;

        return *this;
    }

    JsonArrayFormatter::~JsonArrayFormatter()
    {
        *stream << " ]";
    }

    void JsonArrayFormatter::Add(bool tag)
    {
        InsertSeparation();
        *stream << (tag ? "true" : "false");
    }

    void JsonArrayFormatter::Add(int32_t tag)
    {
        InsertSeparation();
        *stream << tag;
    }

    void JsonArrayFormatter::Add(uint32_t tag)
    {
        InsertSeparation();
        *stream << tag;
    }

    void JsonArrayFormatter::Add(int64_t tag)
    {
        InsertSeparation();
        *stream << tag;
    }

    void JsonArrayFormatter::Add(const char* tag)
    {
        InsertSeparation();
        *stream << '"' << tag << '"';
    }

    void JsonArrayFormatter::Add(infra::BoundedConstString tag)
    {
        InsertSeparation();
        *stream << '"' << tag << '"';
    }

    JsonObjectFormatter JsonArrayFormatter::SubObject()
    {
        InsertSeparation();

        return JsonObjectFormatter(*stream);
    }

    JsonArrayFormatter JsonArrayFormatter::SubArray()
    {
        InsertSeparation();

        return JsonArrayFormatter(*stream);
    }

    bool JsonArrayFormatter::HasFailed() const
    {
        return stream->HasFailed();
    }

    void JsonArrayFormatter::InsertSeparation()
    {
        if (!empty)
            *stream << ", ";

        empty = false;
    }
}
