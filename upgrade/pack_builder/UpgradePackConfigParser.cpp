#include "UpgradePackConfigParser.hpp"

namespace
{
    std::string StdString(infra::BoundedConstString str)
    {
        return std::string(str.data(), str.size());
    }

    std::pair<std::string, std::string> StdStringPair(infra::BoundedConstString str1, infra::BoundedConstString str2)
    {
        return std::pair<std::string, std::string>(StdString(str1), StdString(str2));
    }
}

namespace application
{
    UpgradePackConfigParser::UpgradePackConfigParser(infra::JsonObject& json)
        : json(json)
    {
        CheckValidJson();
        CheckMandatoryKeys();
    }

    void UpgradePackConfigParser::CheckValidJson()
    {
        for (auto it : json)
        {}

        if (json.Error())
            throw ParseException("ConfigParser error: invalid JSON");
    }

    void UpgradePackConfigParser::CheckMandatoryKeys()
    {
        std::vector<std::string> mandatoryKeys = { "components" };
        std::for_each(mandatoryKeys.begin(), mandatoryKeys.end(), [this](const std::string& key)
        {
            infra::BoundedConstString s = key.data();
            if (!json.HasKey(s))
                throw ParseException("ConfigParser error: required key " + key + " missing");
        });
    }

    std::vector<std::pair<std::string, std::string>> UpgradePackConfigParser::GetComponents()
    {
        infra::Optional<infra::JsonObject> components = json.GetOptionalObject("components");

        if (components == infra::none)
            throw ParseException(std::string("ConfigParser error: components list should be an object"));

        std::vector<std::pair<std::string, std::string>> result;

        for (infra::JsonObjectIterator it = components->begin(); it != components->end() ; ++it)
        {
            if (it->value.Is<infra::BoundedConstString>())
                result.push_back(StdStringPair(it->key, it->value.Get<infra::BoundedConstString>()));
            else
                throw ParseException("ConfigParser error: invalid value for component: " + StdString(it->key));
        }

        return result;
    }

    std::vector<std::pair<std::string, std::string>> UpgradePackConfigParser::GetOptions()
    {
        std::vector<std::pair<std::string, std::string>> result;

        if (!json.HasKey("options"))
            return result;

        infra::Optional<infra::JsonObject> options = json.GetOptionalObject("options");
        if (options == infra::none)
            throw ParseException(std::string("ConfigParser error: options list should be an object"));

        for (infra::JsonObjectIterator it = options->begin(); it != options->end(); ++it)
        {
            if (it->value.Is<infra::BoundedConstString>())
                result.push_back(StdStringPair(it->key, it->value.Get<infra::BoundedConstString>()));
            else
                throw ParseException("ConfigParser error: invalid value for option: " + StdString(it->key));
        }

        return result;
    }

    std::string UpgradePackConfigParser::GetOutputFilename()
    {
        if (json.HasKey("output_filename"))
        {
            if (json.GetOptionalString("output_filename") == infra::none)
                throw ParseException(std::string("ConfigParser error: output filename should be a string"));
            else
                return StdString(json.GetString("output_filename"));
        }

        return "";
    }
}
