#include "infra/syntax/JsonFormatter.hpp"
#include <tuple>

namespace infra
{
    namespace
    {
        std::size_t EscapedCharacterSize(char c)
        {
            std::array<char, 7> shouldBeEscaped = { '"', '\\', '\b', '\f', '\n', '\r', '\t' };
            if (std::any_of(shouldBeEscaped.begin(), shouldBeEscaped.end(), [c](char shouldBeEscaped) { return c == shouldBeEscaped; }))
                return 2;
            else
                return 6;
        }

        void InsertEscapedCharacter(infra::TextOutputStream& stream, char c)
        {
            switch (c)
            {
                case '"':  stream << "\\\""; break;
                case '\\': stream << "\\\\"; break;
                case '\b': stream << "\\b"; break;
                case '\f': stream << "\\f"; break;
                case '\n': stream << "\\n"; break;
                case '\r': stream << "\\r"; break;
                case '\t': stream << "\\t"; break;
                default:   stream << "\\u" << infra::hex << infra::Width(4, '0') << static_cast<uint8_t>(c); break;
            }
        }

        std::tuple<std::size_t, infra::BoundedConstString> NonEscapedSubString(infra::BoundedConstString string, std::size_t start)
        {
            std::size_t escape = std::min(string.find_first_of("\"\b\f\n\r\t", start), string.size());
            infra::BoundedConstString nonEscapedSubString = string.substr(start, escape - start);

            for (std::size_t control = start; control != escape; ++control)
                if (string[control] < 0x20)
                {
                    escape = control;
                    nonEscapedSubString = string.substr(start, escape - start);
                    break;
                }
            
            return std::make_tuple(escape, nonEscapedSubString);
        }

        void InsertEscapedTag(infra::TextOutputStream& stream, infra::BoundedConstString tag)
        {
            std::size_t start = 0;
            while (start != tag.size())
            {
                std::size_t escape;
                infra::BoundedConstString nonEscapedSubString;
                std::tie(escape, nonEscapedSubString) = NonEscapedSubString(tag, start);

                start = escape;
                if (!nonEscapedSubString.empty())
                    stream << nonEscapedSubString;
                if (escape != tag.size())
                {
                    InsertEscapedCharacter(stream, tag[escape]);

                    ++start;
                }
            }
        }
    }

    std::size_t JsonEscapedStringSize(infra::BoundedConstString string)
    {
        std::size_t start = 0;
        std::size_t size = 0;

        while (start != string.size())
        {
            std::size_t escape;
            infra::BoundedConstString nonEscapedSubString;
            std::tie(escape, nonEscapedSubString) = NonEscapedSubString(string, start);

            start = escape;
            if (!nonEscapedSubString.empty())
                size += nonEscapedSubString.size();
            if (start != string.size())
            {
                size += EscapedCharacterSize(string[start]);
                ++start;
            }
        }

        return size;
    }

    JsonObjectFormatter::JsonObjectFormatter(infra::TextOutputStream& stream)
        : stream(infra::inPlace, stream.Writer(), infra::noFail)
    {
        *this->stream << "{ ";
    }

    JsonObjectFormatter::JsonObjectFormatter(JsonObjectFormatter&& other)
        : stream(other.stream)
    {
        other.stream = infra::none;
    }

    JsonObjectFormatter& JsonObjectFormatter::operator=(JsonObjectFormatter&& other)
    {
        stream = other.stream;
        other.stream = infra::none;

        return *this;
    }

    JsonObjectFormatter::~JsonObjectFormatter()
    {
        if (stream != infra::none)
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
        *stream << '"' << tagName << R"(":")";
        InsertEscapedTag(*stream, tag);
        *stream << '"';
    }

    void JsonObjectFormatter::Add(const char* tagName, infra::BoundedConstString tag)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":")";
        InsertEscapedTag(*stream, tag);
        *stream << '"';
    }

    void JsonObjectFormatter::AddMilliFloat(const char* tagName, uint32_t intValue, uint32_t milliFractionalValue)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << intValue << '.' << infra::Width(3, '0') << milliFractionalValue;
    }

    void JsonObjectFormatter::AddSubObject(const char* tagName, infra::BoundedConstString json)
    {
        InsertSeparation();
        *stream << '"' << tagName << R"(":)" << json;
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

    bool JsonObjectFormatter::Failed() const
    {
        return stream->Failed();
    }

    void JsonObjectFormatter::InsertSeparation()
    {
        if (!empty)
            *stream << ", ";

        empty = false;
    }

    JsonArrayFormatter::JsonArrayFormatter(infra::TextOutputStream& stream)
        : stream(infra::inPlace, stream.Writer(), infra::noFail)
    {
        *this->stream << "[ ";
    }

    JsonArrayFormatter::JsonArrayFormatter(JsonArrayFormatter&& other)
        : stream(other.stream)
    {
        other.stream = infra::none;
    }

    JsonArrayFormatter& JsonArrayFormatter::operator=(JsonArrayFormatter&& other)
    {
        stream = other.stream;
        other.stream = infra::none;

        return *this;
    }

    JsonArrayFormatter::~JsonArrayFormatter()
    {
        if (stream != infra::none)
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
        *stream << '"';
        InsertEscapedTag(*stream, tag);
        *stream << '"';
    }

    void JsonArrayFormatter::Add(infra::BoundedConstString tag)
    {
        InsertSeparation();
        *stream << '"';
        InsertEscapedTag(*stream, tag);
        *stream << '"';
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

    bool JsonArrayFormatter::Failed() const
    {
        return stream->Failed();
    }

    void JsonArrayFormatter::InsertSeparation()
    {
        if (!empty)
            *stream << ", ";

        empty = false;
    }
}
