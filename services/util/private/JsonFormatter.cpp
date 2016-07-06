#include "services/util/public/JsonFormatter.hpp"

namespace services
{
    JsonFormatter::JsonFormatter(infra::TextOutputStream& stream)
        : stream(stream)
    {
        stream << "{ ";
    }

    JsonFormatter::~JsonFormatter()
    {
        stream << " }";
    }

    void JsonFormatter::Add(const char* tagName, bool tag)
    {
        InsertSeparation();
        stream << '"' << tagName << R"(": )" << (tag ? "true" : "false");
    }

    void JsonFormatter::Add(const char* tagName, int32_t tag)
    {
        InsertSeparation();
        stream << '"' << tagName << R"(": )" << tag;
    }

    void JsonFormatter::Add(const char* tagName, uint32_t tag)
    {
        InsertSeparation();
        stream << '"' << tagName << R"(": )" << tag;
    }

    void JsonFormatter::Add(const char* tagName, const char* tag)
    {
        InsertSeparation();
        stream << '"' << tagName << R"(": ")" << tag << '"';
    }

    void JsonFormatter::Add(const char* tagName, infra::BoundedConstString tag)
    {
        InsertSeparation();
        stream << '"' << tagName << R"(": ")" << tag << '"';
    }

    void JsonFormatter::InsertSeparation()
    {
        if (!empty)
            stream << ", ";

        empty = false;
    }
}
