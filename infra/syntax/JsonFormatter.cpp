#include "infra/syntax/JsonFormatter.hpp"
#include <ctime>

namespace infra
{
    namespace
    {
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

        void InsertEscapedTag(infra::TextOutputStream& stream, infra::BoundedConstString tag)
        {
            std::size_t start = 0;
            while (start != tag.size())
            {
                std::size_t escape = std::min(tag.find_first_of("\"\b\f\n\r\t", start), tag.size());
                infra::BoundedConstString nonEscapedSubString = tag.substr(start, escape - start);

                for (std::size_t control = start; control != escape; ++control)
                    if (tag[control] < 0x20)
                    {
                        escape = control;
                        nonEscapedSubString = tag.substr(start, escape - start);
                        break;
                    }

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

    void JsonObjectFormatter::Add(const char* tagName, infra::Duration duration)
    {
        InsertSeparation();
        if (duration < infra::Duration::zero())
            *stream << '"' << tagName << R"(":")" << "-" << infra::Width(2, '0') << std::chrono::duration_cast<std::chrono::hours>(-duration).count()
            << ":" << std::chrono::duration_cast<std::chrono::minutes>(-duration).count() % 60;
        else
            *stream << '"' << tagName << R"(":")" << "+" << infra::Width(2, '0') << std::chrono::duration_cast<std::chrono::hours>(duration).count()
            << ":" << std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60;
        *stream << '"';
    }

    void JsonObjectFormatter::AddIsoTime(const char* tagName, infra::TimePoint timePoint, infra::Duration offset)
    {
        InsertSeparation();
        auto timeAsTimeType = std::chrono::system_clock::to_time_t(timePoint);
        auto timeAsGmTime = std::gmtime(&timeAsTimeType);
        assert(timeAsGmTime != nullptr);
        std::array<char, 64> buffer = {};

        std::size_t size = std::strftime(buffer.data(), buffer.size(), "%FT%T", timeAsGmTime);

        if (size > 0)
            *stream << '"' << tagName << R"(":")" << buffer.data();
        else
            return;

        if (offset < infra::Duration::zero())
            *stream << "-" << infra::Width(2, '0') << std::chrono::duration_cast<std::chrono::hours>(-offset).count()
            << ":" << std::chrono::duration_cast<std::chrono::minutes>(-offset).count() % 60;
        else
            *stream << "+" << infra::Width(2, '0') << std::chrono::duration_cast<std::chrono::hours>(offset).count()
            << ":" << std::chrono::duration_cast<std::chrono::minutes>(offset).count() % 60;
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
