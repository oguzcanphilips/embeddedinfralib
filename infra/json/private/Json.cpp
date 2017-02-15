#include "infra/json/public/Json.hpp"
#include <cctype>

namespace
{
    template<class Iterator>
    bool Equal(Iterator leftBegin, Iterator leftEnd, Iterator rightBegin, Iterator rightEnd)
    {
        while (leftBegin != leftEnd && rightBegin != rightEnd)
        {
            if (*leftBegin != *rightBegin)
                return false;

            ++leftBegin;
            ++rightBegin;
        }

        return leftBegin == leftEnd && rightBegin == rightEnd;
    }
}

namespace infra
{
    namespace JsonToken
    {
        bool End::operator==(const End& other) const
        {
            return true;
        }

        bool End::operator!=(const End& other) const
        {
            return false;
        }

        bool Error::operator==(const Error& other) const
        {
            return true;
        }

        bool Error::operator!=(const Error& other) const
        {
            return false;
        }

        bool Colon::operator==(const Colon& other) const
        {
            return true;
        }

        bool Colon::operator!=(const Colon& other) const
        {
            return false;
        }

        bool Comma::operator==(const Comma& other) const
        {
            return true;
        }

        bool Comma::operator!=(const Comma& other) const
        {
            return false;
        }

        bool Dot::operator==(const Dot& other) const
        {
            return true;
        }

        bool Dot::operator!=(const Dot& other) const
        {
            return false;
        }

        LeftBrace::LeftBrace(std::size_t index)
            : index(index)
        {}

        bool LeftBrace::operator==(const LeftBrace& other) const
        {
            return index == other.index;
        }

        bool LeftBrace::operator!=(const LeftBrace& other) const
        {
            return index != other.index;
        }

        std::size_t LeftBrace::Index() const
        {
            return index;
        }

        RightBrace::RightBrace(std::size_t index)
            : index(index)
        {}

        bool RightBrace::operator==(const RightBrace& other) const
        {
            return index == other.index;
        }

        bool RightBrace::operator!=(const RightBrace& other) const
        {
            return index != other.index;
        }

        std::size_t RightBrace::Index() const
        {
            return index;
        }

        LeftBracket::LeftBracket(std::size_t index)
            : index(index)
        {}

        bool LeftBracket::operator==(const LeftBracket& other) const
        {
            return index == other.index;
        }

        bool LeftBracket::operator!=(const LeftBracket& other) const
        {
            return index != other.index;
        }

        std::size_t LeftBracket::Index() const
        {
            return index;
        }

        RightBracket::RightBracket(std::size_t index)
            : index(index)
        {}

        bool RightBracket::operator==(const RightBracket& other) const
        {
            return index == other.index;
        }

        bool RightBracket::operator!=(const RightBracket& other) const
        {
            return index != other.index;
        }

        std::size_t RightBracket::Index() const
        {
            return index;
        }

        String::String(infra::BoundedConstString value)
            : value(value)
        {}

        bool String::operator==(const String& other) const
        {
            return value == other.value;
        }

        bool String::operator!=(const String& other) const
        {
            return value != other.value;
        }

        infra::BoundedConstString String::Value() const
        {
            return value;
        }

        Integer::Integer(int32_t value)
            : value(value)
        {}

        bool Integer::operator==(const Integer& other) const
        {
            return value == other.value;
        }

        bool Integer::operator!=(const Integer& other) const
        {
            return value != other.value;
        }

        int32_t Integer::Value() const
        {
            return value;
        }

        Boolean::Boolean(bool value)
            : value(value)
        {}

        bool Boolean::operator==(const Boolean& other) const
        {
            return value == other.value;
        }

        bool Boolean::operator!=(const Boolean& other) const
        {
            return value != other.value;
        }

        bool Boolean::Value() const
        {
            return value;
        }
    }

    JsonTokenizer::JsonTokenizer(infra::BoundedConstString objectString)
        : objectString(objectString)
    {}

    JsonToken::Token JsonTokenizer::Token()
    {
        SkipWhitespace();

        if (parseIndex == objectString.size())
            return JsonToken::End();
        else
        {
            if (objectString[parseIndex] == '"')
                return TryCreateStringToken();
            else if (objectString[parseIndex] == ':')
            {
                ++parseIndex;
                return JsonToken::Colon();
            }
            else if (objectString[parseIndex] == ',')
            {
                ++parseIndex;
                return JsonToken::Comma();
            }
            else if (objectString[parseIndex] == '.')
            {
                ++parseIndex;
                return JsonToken::Dot();
            }
            else if (objectString[parseIndex] == '{')
                return JsonToken::LeftBrace(parseIndex++);
            else if (objectString[parseIndex] == '}')
                return JsonToken::RightBrace(parseIndex++);
            else if (objectString[parseIndex] == '[')
                return JsonToken::LeftBracket(parseIndex++);
            else if (objectString[parseIndex] == ']')
                return JsonToken::RightBracket(parseIndex++);
            else if (std::isdigit(objectString[parseIndex]) != 0)
                return TryCreateIntegerToken();
            else if (std::isalpha(objectString[parseIndex]) != 0)
                return TryCreateIdentifierToken();
            else
                return JsonToken::Error();
        }
    }

    bool JsonTokenizer::operator==(const JsonTokenizer& other) const
    {
        return parseIndex == other.parseIndex;
    }

    bool JsonTokenizer::operator!=(const JsonTokenizer& other) const
    {
        return !(*this == other);
    }

    void JsonTokenizer::SkipWhitespace()
    {
        while (parseIndex != objectString.size() && std::isspace(objectString[parseIndex]))                         //TICS !CON#007
            ++parseIndex;
    }

    JsonToken::Token JsonTokenizer::TryCreateStringToken()
    {
        ++parseIndex;
        std::size_t tokenStart = parseIndex;

        bool escape = false;
        while (escape || objectString[parseIndex] != '"')
        {
            escape = !escape && objectString[parseIndex] == '\\';
            ++parseIndex;

            if (parseIndex == objectString.size())
                return JsonToken::Error();
        }

        ++parseIndex;

        return JsonToken::String(objectString.substr(tokenStart, parseIndex - tokenStart - 1));
    }

    JsonToken::Token JsonTokenizer::TryCreateIntegerToken()
    {
        std::size_t tokenStart = parseIndex;

        while (parseIndex != objectString.size() && std::isdigit(objectString[parseIndex]))                         //TICS !CON#007
            ++parseIndex;

        infra::BoundedConstString integer = objectString.substr(tokenStart, parseIndex - tokenStart);

        return JsonToken::Integer(std::atoi(integer.data()));
    }

    JsonToken::Token JsonTokenizer::TryCreateIdentifierToken()
    {
        std::size_t tokenStart = parseIndex;

        while (parseIndex != objectString.size() && std::isalpha(objectString[parseIndex]))                         //TICS !CON#007
            ++parseIndex;

        infra::BoundedConstString identifier = objectString.substr(tokenStart, parseIndex - tokenStart);
        if (identifier == "true")
            return JsonToken::Boolean(true);
        else if (identifier == "false")
            return JsonToken::Boolean(false);
        else
            return JsonToken::Error();
    }

    JsonObject::JsonObject(infra::BoundedConstString objectString)
        : objectString(objectString)
    {}

    infra::BoundedConstString JsonObject::ObjectString() const
    {
        return objectString;
    }

    JsonObjectIterator JsonObject::begin()
    {
        return JsonObjectIterator(*this);
    }

    JsonObjectIterator JsonObject::end()
    {
        return JsonObjectIterator();
    }

    bool JsonObject::HasKey(infra::BoundedConstString key)
    {
        for (auto& keyValue : *this)
        {
            if (keyValue.key == key)
                return true;
        }

        return false;
    }

    infra::BoundedConstString JsonObject::GetString(infra::BoundedConstString key)
    {
        return GetValue<infra::BoundedConstString>(key);
    }

    bool JsonObject::GetBoolean(infra::BoundedConstString key)
    {
        return GetValue<bool>(key);
    }

    int32_t JsonObject::GetInteger(infra::BoundedConstString key)
    {
        return GetValue<int32_t>(key);
    }

    JsonObject JsonObject::GetObject(infra::BoundedConstString key)
    {
        return GetValue<JsonObject>(key);
    }

    JsonArray JsonObject::GetArray(infra::BoundedConstString key)
    {
        return GetValue<JsonArray>(key);
    }

    JsonValue JsonObject::GetValue(infra::BoundedConstString key)
    {
        for (auto& keyValue : *this)
        {
            if (keyValue.key == key)
                return keyValue.value;
        }

        SetError();
        return JsonValue();
    }

    infra::Optional<infra::BoundedConstString> JsonObject::GetOptionalString(infra::BoundedConstString key)
    {
        return GetOptionalValue<infra::BoundedConstString>(key);
    }

    infra::Optional<bool> JsonObject::GetOptionalBoolean(infra::BoundedConstString key)
    {
        return GetOptionalValue<bool>(key);
    }

    infra::Optional<int32_t> JsonObject::GetOptionalInteger(infra::BoundedConstString key)
    {
        return GetOptionalValue<int32_t>(key);
    }

    infra::Optional<JsonObject> JsonObject::GetOptionalObject(infra::BoundedConstString key)
    {
        return GetOptionalValue<JsonObject>(key);
    }

    infra::Optional<JsonArray> JsonObject::GetOptionalArray(infra::BoundedConstString key)
    {
        return GetOptionalValue<JsonArray>(key);
    }

    bool JsonObject::operator==(const JsonObject& other) const
    {
        JsonObject left(*this);
        JsonObject right(other);

        bool result = Equal(left.begin(), left.end(), right.begin(), right.end());
        return result && !left.Error() && !right.Error();
    }

    bool JsonObject::operator!=(const JsonObject& other) const
    {
        return !(*this == other);
    }

    void JsonObject::SetError()
    {
        error = true;
    }

    bool JsonObject::Error() const
    {
        return error;
    }

    template<class T>
    T JsonObject::GetValue(infra::BoundedConstString key)
    {
        for (auto& keyValue : *this)
        {
            if (keyValue.key == key && keyValue.value.Is<T>())
                return keyValue.value.Get<T>();
        }

        SetError();
        return T();
    }

    template<class T>
    infra::Optional<T> JsonObject::GetOptionalValue(infra::BoundedConstString key)
    {
        for (auto& keyValue : *this)
        {
            if (keyValue.key == key && keyValue.value.Is<T>())
                return infra::MakeOptional(keyValue.value.Get<T>());
        }

        return infra::none;
    }

    JsonArray::JsonArray(infra::BoundedConstString objectString)
        : objectString(objectString)
    {}

    infra::BoundedConstString JsonArray::ObjectString() const
    {
        return objectString;
    }

    void JsonArray::SetError()
    {
        error = true;
    }

    bool JsonArray::Error() const
    {
        return error;
    }

    JsonArrayIterator JsonArray::begin()
    {
        return JsonArrayIterator(*this);
    }

    JsonArrayIterator JsonArray::end()
    {
        return JsonArrayIterator();
    }

    bool JsonArray::operator==(const JsonArray& other) const
    {
        JsonArray left(*this);
        JsonArray right(other);

        bool result = Equal(left.begin(), left.end(), right.begin(), right.end());
        return result && !left.Error() && !right.Error();
    }

    bool JsonArray::operator!=(const JsonArray& other) const
    {
        return !(*this == other);
    }

    bool JsonKeyValue::operator==(const JsonKeyValue& other) const
    {
        return key == other.key && value == other.value;
    }

    bool JsonKeyValue::operator!=(const JsonKeyValue& other) const
    {
        return !(*this == other);
    }

    JsonIterator::JsonIterator(infra::BoundedConstString objectString)
        : objectString(objectString)
        , tokenizer(objectString)
    {}

    infra::Optional<JsonValue> JsonIterator::ConvertValue(JsonToken::Token token)
    {
        if (token.Is<JsonToken::String>())
            return infra::MakeOptional(JsonValue(token.Get<JsonToken::String>().Value()));
        else if (token.Is<JsonToken::Integer>())
            return infra::MakeOptional(JsonValue(token.Get<JsonToken::Integer>().Value()));
        else if (token.Is<JsonToken::Boolean>())
            return infra::MakeOptional(JsonValue(token.Get<JsonToken::Boolean>().Value()));
        else if (token.Is<JsonToken::LeftBrace>())
            return ReadObjectValue(token);
        else if (token.Is<JsonToken::LeftBracket>())
            return ReadArrayValue(token);
        else
            return infra::none;
    }

    infra::Optional<JsonValue> JsonIterator::ReadObjectValue(JsonToken::Token token)
    {
        infra::Optional<JsonToken::RightBrace> objectEnd = SearchObjectEnd();

        if (objectEnd)
            return infra::MakeOptional(JsonValue(JsonObject(objectString.substr(token.Get<JsonToken::LeftBrace>().Index(), objectEnd->Index() + 1 - token.Get<JsonToken::LeftBrace>().Index()))));
        else
            return infra::none;
    }

    infra::Optional<JsonValue> JsonIterator::ReadArrayValue(JsonToken::Token token)
    {
        infra::Optional<JsonToken::RightBracket> arrayEnd = SearchArrayEnd();

        if (arrayEnd)
            return infra::MakeOptional(JsonValue(JsonArray(objectString.substr(token.Get<JsonToken::LeftBracket>().Index(), arrayEnd->Index() + 1 - token.Get<JsonToken::LeftBracket>().Index()))));
        else
            return infra::none;
    }

    infra::Optional<JsonToken::RightBrace> JsonIterator::SearchObjectEnd()
    {
        std::size_t nested = 0;
        JsonToken::Token token = tokenizer.Token();
        while ((nested != 0 || !token.Is<JsonToken::RightBrace>()) && !token.Is<JsonToken::End>() && !token.Is<JsonToken::Error>())
        {
            if (token.Is<JsonToken::LeftBrace>())
                --nested;
            if (token.Is<JsonToken::RightBrace>())
                ++nested;

            token = tokenizer.Token();
        }

        if (token.Is<JsonToken::RightBrace>())
            return infra::MakeOptional(token.Get<JsonToken::RightBrace>());
        return
            infra::none;
    }

    infra::Optional<JsonToken::RightBracket> JsonIterator::SearchArrayEnd()
    {
        std::size_t nested = 0;
        JsonToken::Token token = tokenizer.Token();
        while ((nested != 0 || !token.Is<JsonToken::RightBracket>()) && !token.Is<JsonToken::End>() && !token.Is<JsonToken::Error>())
        {
            if (token.Is<JsonToken::LeftBracket>())
                --nested;
            if (token.Is<JsonToken::RightBracket>())
                ++nested;

            token = tokenizer.Token();
        }

        if (token.Is<JsonToken::RightBracket>())
            return infra::MakeOptional(token.Get<JsonToken::RightBracket>());
        else
            return infra::none;
    }

    JsonObjectIterator::JsonObjectIterator()
        : JsonIterator("")
    {}

    JsonObjectIterator::JsonObjectIterator(JsonObject& object)
        : JsonIterator(object.ObjectString())
        , object(&object)
        , state(readObjectStart)
    {
        ++*this;
    }

    bool JsonObjectIterator::operator==(const JsonObjectIterator& other) const
    {
        return state == other.state && (state == end || tokenizer == other.tokenizer);
    }

    bool JsonObjectIterator::operator!=(const JsonObjectIterator& other) const
    {
        return !(*this == other);
    }

    JsonKeyValue& JsonObjectIterator::operator*()
    {
        assert(state != end);
        return keyValue;
    }

    const JsonKeyValue& JsonObjectIterator::operator*() const
    {
        assert(state != end);
        return keyValue;
    }

    JsonKeyValue* JsonObjectIterator::operator->()
    {
        assert(state != end);
        return &keyValue;
    }

    const JsonKeyValue* JsonObjectIterator::operator->() const
    {
        assert(state != end);
        return &keyValue;
    }

    JsonObjectIterator& JsonObjectIterator::operator++()
    {
        do
        {
            ParseNextToken();
        } while (state != readCommaOrObjectEnd && state != end);

        return *this;
    }

    void JsonObjectIterator::ParseNextToken()
    {
        JsonToken::Token token = tokenizer.Token();
        switch (state)
        {
            case readObjectStart:
                ReadObjectStart(token);
                break;
            case readKeyOrEnd:
                ReadKeyOrEnd(token);
                break;
            case readKey:
                ReadKey(token);
                break;
            case readColon:
                ReadColon(token);
                break;
            case readValue:
                ReadValue(token);
                break;
            case readCommaOrObjectEnd:
                ReadCommaOrObjectEnd(token);
                break;
            case end:
                std::abort();
        }
    }

    void JsonObjectIterator::ReadObjectStart(JsonToken::Token token)
    {
        state = readKeyOrEnd;
        if (!token.Is<JsonToken::LeftBrace>())
            SetError();
    }

    void JsonObjectIterator::ReadKeyOrEnd(JsonToken::Token token)
    {
        state = readColon;
        if (token.Is<JsonToken::RightBrace>())
            state = end;
        else if (token.Is<JsonToken::String>())
            keyValue.key = token.Get<JsonToken::String>().Value();
        else
            SetError();
    }

    void JsonObjectIterator::ReadKey(JsonToken::Token token)
    {
        state = readColon;
        if (token.Is<JsonToken::String>())
            keyValue.key = token.Get<JsonToken::String>().Value();
        else
            SetError();
    }

    void JsonObjectIterator::ReadColon(JsonToken::Token token)
    {
        state = readValue;
        if (!token.Is<JsonToken::Colon>())
            SetError();
    }

    void JsonObjectIterator::ReadValue(JsonToken::Token token)
    {
        state = readCommaOrObjectEnd;
        infra::Optional<JsonValue> readValue = ConvertValue(token);
        if (readValue)
            keyValue.value = *readValue;
        else
            SetError();
    }

    void JsonObjectIterator::ReadCommaOrObjectEnd(JsonToken::Token token)
    {
        if (token.Is<JsonToken::Comma>())
            state = readKey;
        else if (token.Is<JsonToken::RightBrace>())
            state = end;
        else
            SetError();
    }

    void JsonObjectIterator::SetError()
    {
        object->SetError();
        state = end;
    }

    JsonObjectIterator JsonObjectIterator::operator++(int)
    {
        JsonObjectIterator result(*this);
        ++*this;
        return result;
    }

    JsonArrayIterator::JsonArrayIterator()
        : JsonIterator("")
    {}

    JsonArrayIterator::JsonArrayIterator(JsonArray& jsonArray)
        : JsonIterator(jsonArray.ObjectString())
        , jsonArray(&jsonArray)
        , state(readArrayStart)
    {
        ++*this;
    }

    bool JsonArrayIterator::operator==(const JsonArrayIterator& other) const
    {
        return state == other.state && (state == end || tokenizer == other.tokenizer);
    }

    bool JsonArrayIterator::operator!=(const JsonArrayIterator& other) const
    {
        return !(*this == other);
    }

    JsonValue& JsonArrayIterator::operator*()
    {
        assert(state != end);
        return value;
    }

    const JsonValue& JsonArrayIterator::operator*() const
    {
        assert(state != end);
        return value;
    }

    JsonValue* JsonArrayIterator::operator->()
    {
        assert(state != end);
        return &value;
    }

    const JsonValue* JsonArrayIterator::operator->() const
    {
        assert(state != end);
        return &value;
    }

    JsonArrayIterator& JsonArrayIterator::operator++()
    {
        do
        {
            JsonToken::Token token = tokenizer.Token();
            switch (state)
            {
                case readArrayStart:
                    state = readValueOrEnd;
                    if (!token.Is<JsonToken::LeftBracket>())
                        SetError();
                    break;
                case readValueOrEnd:
                    state = readCommaOrArrayEnd;
                    if (token.Is<JsonToken::RightBracket>())
                        state = end;
                    else
                        ReadValue(token);
                    break;
                case readValue:
                    state = readCommaOrArrayEnd;
                    ReadValue(token);
                    break;
                case readCommaOrArrayEnd:
                    if (token.Is<JsonToken::Comma>())
                        state = readValue;
                    else if (token.Is<JsonToken::RightBracket>())
                        state = end;
                    else
                        SetError();
                    break;
                case end:
                    std::abort();
            }
        } while (state != readCommaOrArrayEnd && state != end);

        return *this;
    }

    JsonArrayIterator JsonArrayIterator::operator++(int)
    {
        JsonArrayIterator result(*this);
        ++*this;
        return result;
    }

    void JsonArrayIterator::ReadValue(JsonToken::Token token)
    {
        infra::Optional<JsonValue> readValue = ConvertValue(token);
        if (readValue)
            value = *readValue;
        else
            SetError();
    }

    void JsonArrayIterator::SetError()
    {
        jsonArray->SetError();
        state = end;
    }

    infra::detail::DoublePair<JsonValueArrayIterator<bool>> JsonBooleanArray(JsonArray& array)
    {
        return infra::detail::DoublePair<JsonValueArrayIterator<bool>>(
            JsonValueArrayIterator<bool>(array.begin(), array.end()),
            JsonValueArrayIterator<bool>(array.end(), array.end()));
    }

    infra::detail::DoublePair<JsonValueArrayIterator<int32_t>> JsonIntegerArray(JsonArray& array)
    {
        return infra::detail::DoublePair<JsonValueArrayIterator<int32_t>>(
            JsonValueArrayIterator<int32_t>(array.begin(), array.end()),
            JsonValueArrayIterator<int32_t>(array.end(), array.end()));
    }

    infra::detail::DoublePair<JsonValueArrayIterator<infra::BoundedConstString>> JsonStringArray(JsonArray& array)
    {
        return infra::detail::DoublePair<JsonValueArrayIterator<infra::BoundedConstString>>(
            JsonValueArrayIterator<infra::BoundedConstString>(array.begin(), array.end()),
            JsonValueArrayIterator<infra::BoundedConstString>(array.end(), array.end()));
    }

    infra::detail::DoublePair<JsonValueArrayIterator<JsonObject>> JsonObjectArray(JsonArray& array)
    {
        return infra::detail::DoublePair<JsonValueArrayIterator<JsonObject>>(
            JsonValueArrayIterator<JsonObject>(array.begin(), array.end()),
            JsonValueArrayIterator<JsonObject>(array.end(), array.end()));
    }

    infra::detail::DoublePair<JsonValueArrayIterator<JsonArray>> JsonArrayArray(JsonArray& array)
    {
        return infra::detail::DoublePair<JsonValueArrayIterator<JsonArray>>(
            JsonValueArrayIterator<JsonArray>(array.begin(), array.end()),
            JsonValueArrayIterator<JsonArray>(array.end(), array.end()));
    }
}
