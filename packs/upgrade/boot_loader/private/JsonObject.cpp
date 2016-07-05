#include "packs/upgrade/boot_loader/public/JsonObject.hpp"

namespace application
{
    JsonObject::JsonObject(infra::BoundedConstString object)
        : object(object)
    {}

    bool JsonObject::Valid() const
    {
        return valid;
    }

    infra::BoundedConstString JsonObject::GetString(const char* key)
    {
        std::size_t stringStart = ObjectStart(key);

        if (stringStart == infra::BoundedConstString::npos
            || object[stringStart] != '"')
        {
            valid = false;
            return infra::BoundedConstString::WithStorage<0>();
        }

        std::size_t index = ++stringStart;

        bool escape = false;
        while (index != object.size() && !escape && object[index] != '"')
        {
            if (escape)
                escape = false;
            else if (object[index] == '\\')
                    escape = true;

            ++index;
        }

        return object.substr(stringStart, index - stringStart);
    }

    uint32_t JsonObject::GetInt(const char* key)
    {
        std::size_t valueStart = ObjectStart(key);

        if (valueStart == infra::BoundedConstString::npos)
        {
            valid = false;
            return 0;
        }

        uint32_t result = 0;
        std::size_t index = valueStart;

        while (index != object.size() && object[index] >= '0' && object[index] <= '9')
        {
            result = result * 10 + (object[index] - '0');
            ++index;
        }

        if (index == valueStart)
            valid = false;

        return result;
    }

    bool JsonObject::GetOptionalBool(const char* key, bool defaultValue)
    {
        std::size_t valueStart = ObjectStart(key);

        if (valueStart == infra::BoundedConstString::npos)
            return defaultValue;

        if (object.substr(valueStart, strlen("true")) == "true")
            return true;
        if (object.substr(valueStart, strlen("false")) == "false")
            return false;

        valid = false;
        return defaultValue;
    }

    uint32_t JsonObject::GetOptionalInt(const char* key, uint32_t defaultValue)
    {
        std::size_t valueStart = ObjectStart(key);

        if (valueStart == infra::BoundedConstString::npos)
            return defaultValue;

        uint32_t result = 0;
        std::size_t index = valueStart;

        while (index != object.size() && object[index] >= '0' && object[index] <= '9')
        {
            result = result * 10 + (object[index] - '0');
            ++index;
        }

        if (index == valueStart)
            valid = false;

        return result;
    }

    std::size_t JsonObject::ObjectStart(const char* key) const
    {
        std::size_t keyLength = strlen(key);
        std::size_t index = object.find(key);

        while (index != infra::BoundedConstString::npos)
        {
            if (index > 1 && object[index - 1] == '"' && index + keyLength + 1 < object.size() && object[index + keyLength] == '"')
            {
                index = object.find_first_not_of(" \t", index + keyLength + 1);
                if (index != infra::BoundedConstString::npos && object[index] == ':')
                    return object.find_first_not_of(" \t", index + 1);
            }

            index = object.find(key, index + 1);
        }

        return infra::BoundedConstString::npos;
    }
}
