#ifndef UPGRADE_JSON_OBJECT_HPP
#define UPGRADE_JSON_OBJECT_HPP

#include "infra/util/public/BoundedString.hpp"

namespace application
{
    class JsonObject
    {
    public:
        JsonObject(infra::BoundedConstString object);

        bool Valid() const;

        infra::BoundedConstString GetString(const char* key);
        uint32_t GetInt(const char* key);

        bool GetOptionalBool(const char* key, bool defaultValue);
        uint32_t GetOptionalInt(const char* key, uint32_t defaultValue);

    private:
        std::size_t ObjectStart(const char* key) const;

    private:
        infra::BoundedConstString object;
        bool valid = true;
    };
}

#endif
