#ifndef SERVICES_JSON_HPP
#define SERVICES_JSON_HPP

#include "infra/util/public/BoundedString.hpp"
#include "infra/util/public/Optional.hpp"
#include "infra/util/public/ReverseRange.hpp"
#include "infra/util/public/Variant.hpp"

namespace infra
{
    namespace JsonToken
    {
        class End
        {
        public:
            bool operator==(const End& other) const;
            bool operator!=(const End& other) const;
        };

        class Error
        {
        public:
            bool operator==(const Error& other) const;
            bool operator!=(const Error& other) const;
        };

        class Colon
        {
        public:
            bool operator==(const Colon& other) const;
            bool operator!=(const Colon& other) const;
        };

        class Comma
        {
        public:
            bool operator==(const Comma& other) const;
            bool operator!=(const Comma& other) const;
        };

        class LeftBrace
        {
        public:
            LeftBrace(std::size_t index);

            bool operator==(const LeftBrace& other) const;
            bool operator!=(const LeftBrace& other) const;

            std::size_t Index() const;

        private:
            std::size_t index;
        };

        class RightBrace
        {
        public:
            RightBrace(std::size_t index);

            bool operator==(const RightBrace& other) const;
            bool operator!=(const RightBrace& other) const;

            std::size_t Index() const;

        private:
            std::size_t index;
        };

        class LeftBracket
        {
        public:
            LeftBracket(std::size_t index);

            bool operator==(const LeftBracket& other) const;
            bool operator!=(const LeftBracket& other) const;

            std::size_t Index() const;

        private:
            std::size_t index;
        };

        class RightBracket
        {
        public:
            RightBracket(std::size_t index);

            bool operator==(const RightBracket& other) const;
            bool operator!=(const RightBracket& other) const;

            std::size_t Index() const;

        private:
            std::size_t index;
        };

        class String
        {
        public:
            String(infra::BoundedConstString value);

            bool operator==(const String& other) const;
            bool operator!=(const String& other) const;

            infra::BoundedConstString Value() const;

        private:
            infra::BoundedConstString value;
        };

        class Integer
        {
        public:
            Integer(int32_t value);

            bool operator==(const Integer& other) const;
            bool operator!=(const Integer& other) const;

            int32_t Value() const;

        private:
            int32_t value;
        };

        class Boolean
        {
        public:
            Boolean(bool value);

            bool operator==(const Boolean& other) const;
            bool operator!=(const Boolean& other) const;

            bool Value() const;

        private:
            bool value;
        };

        using Token = infra::Variant<End, Error, Colon, Comma, LeftBrace, RightBrace, LeftBracket, RightBracket, String, Integer, Boolean>;
    }

    class JsonTokenizer
    {
    public:
        JsonTokenizer(infra::BoundedConstString objectString);

        JsonToken::Token Token();

        bool operator==(const JsonTokenizer& other) const;
        bool operator!=(const JsonTokenizer& other) const;

    private:
        void SkipWhitespace();
        JsonToken::Token TryCreateStringToken();
        JsonToken::Token TryCreateIntegerToken();
        JsonToken::Token TryCreateIdentifierToken();

    private:
        infra::BoundedConstString objectString;
        std::size_t parseIndex = 0;
    };

    class JsonObjectIterator;
    class JsonArrayIterator;
    class JsonObject;
    class JsonArray;

    using JsonValue = infra::Variant<bool, int32_t, infra::BoundedConstString, JsonObject, JsonArray>;

    class JsonObject
    {
    public:
        JsonObject() = default;
        JsonObject(infra::BoundedConstString objectString);

        infra::BoundedConstString ObjectString() const;

        JsonObjectIterator begin();
        JsonObjectIterator end();

        bool HasKey(infra::BoundedConstString key);

        infra::BoundedConstString GetString(infra::BoundedConstString key);
        bool GetBoolean(infra::BoundedConstString key);
        int32_t GetInteger(infra::BoundedConstString key);
        JsonObject GetObject(infra::BoundedConstString key);
        JsonArray GetArray(infra::BoundedConstString key);
        JsonValue GetValue(infra::BoundedConstString key);

        infra::Optional<infra::BoundedConstString> GetOptionalString(infra::BoundedConstString key);
        infra::Optional<bool> GetOptionalBoolean(infra::BoundedConstString key);
        infra::Optional<JsonObject> GetOptionalObject(infra::BoundedConstString key);
        infra::Optional<JsonArray> GetOptionalArray(infra::BoundedConstString key);

        bool operator==(const JsonObject& other) const;

    public:
        void SetError();
        bool Error() const;

    private:
        template<class T>
            T GetValue(infra::BoundedConstString key);
        template<class T>
            infra::Optional<T> GetOptionalValue(infra::BoundedConstString key);

    private:
        infra::BoundedConstString objectString;
        bool error = false;
    };

    class JsonArray
    {
    public:
        JsonArray() = default;
        JsonArray(infra::BoundedConstString objectString);

        infra::BoundedConstString ObjectString() const;

        JsonArrayIterator begin();
        JsonArrayIterator end();

        bool operator==(const JsonArray& other) const;

    public:
        void SetError();
        bool Error() const;

    private:
        infra::BoundedConstString objectString;
        bool error = false;
    };

    struct JsonKeyValue
    {
        bool operator==(const JsonKeyValue& other) const;
        bool operator!=(const JsonKeyValue& other) const;

        infra::BoundedConstString key = "";
        JsonValue value;
    };

    class JsonIterator
    {
    protected:
        JsonIterator(infra::BoundedConstString objectString);

        infra::Optional<JsonValue> ReadValue(JsonToken::Token token);

    private:
        infra::Optional<JsonValue> ReadObjectValue(JsonToken::Token token);
        infra::Optional<JsonValue> ReadArrayValue(JsonToken::Token token);
        infra::Optional<JsonToken::RightBrace> SearchObjectEnd();
        infra::Optional<JsonToken::RightBracket> SearchArrayEnd();

    protected:
        infra::BoundedConstString objectString;
        JsonTokenizer tokenizer;
    };

    class JsonObjectIterator
        : public std::iterator<std::forward_iterator_tag, JsonKeyValue>
        , private JsonIterator
    {
    private:
        friend class JsonObject;

        JsonObjectIterator();
        JsonObjectIterator(JsonObject& object);

    public:
        bool operator==(const JsonObjectIterator& other) const;
        bool operator!=(const JsonObjectIterator& other) const;

        JsonKeyValue& operator*();
        const JsonKeyValue& operator*() const;
        JsonKeyValue* operator->();
        const JsonKeyValue* operator->() const;

        JsonObjectIterator& operator++();
        JsonObjectIterator operator++(int);

    private:
        void SetError();

    private:
        JsonObject* object = nullptr;
        JsonKeyValue keyValue;

        enum
        {
            readObjectStart,
            readKeyOrEnd,
            readKey,
            readColon,
            readValue,
            readCommaOrObjectEnd,
            end
        } state = end;
    };

    class JsonArrayIterator
        : public std::iterator<std::forward_iterator_tag, JsonValue>
        , private JsonIterator
    {
    private:
        friend class JsonArray;

        template<class T>
            friend class JsonValueArrayIterator;

        JsonArrayIterator();
        JsonArrayIterator(JsonArray& jsonArray);

    public:
        bool operator==(const JsonArrayIterator& other) const;
        bool operator!=(const JsonArrayIterator& other) const;

        JsonValue& operator*();
        const JsonValue& operator*() const;
        JsonValue* operator->();
        const JsonValue* operator->() const;

        JsonArrayIterator& operator++();
        JsonArrayIterator operator++(int);

    private:
        void TryReadValue(JsonToken::Token token);
        void SetError();

    private:
        JsonArray* jsonArray = nullptr;
        JsonValue value;

        enum
        {
            readArrayStart,
            readValueOrEnd,
            readValue,
            readCommaOrArrayEnd,
            end
        } state = end;
    };

    template<class T>
    class JsonValueArrayIterator
        : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        JsonValueArrayIterator() = default;
        JsonValueArrayIterator(JsonArrayIterator arrayIterator, JsonArrayIterator arrayEndIterator);

    public:
        bool operator==(const JsonValueArrayIterator& other) const;
        bool operator!=(const JsonValueArrayIterator& other) const;

        T operator*() const;
        const T* operator->() const;

        JsonValueArrayIterator& operator++();
        JsonValueArrayIterator operator++(int);

    private:
        JsonArrayIterator arrayIterator;
        JsonArrayIterator arrayEndIterator;
    };

    detail::DoublePair<JsonValueArrayIterator<bool>> JsonBooleanArray(JsonArray& array);
    detail::DoublePair<JsonValueArrayIterator<int32_t>> JsonIntegerArray(JsonArray& array);
    detail::DoublePair<JsonValueArrayIterator<infra::BoundedConstString>> JsonStringArray(JsonArray& array);
    detail::DoublePair<JsonValueArrayIterator<JsonObject>> JsonObjectArray(JsonArray& array);
    detail::DoublePair<JsonValueArrayIterator<JsonArray>> JsonArrayArray(JsonArray& array);

    ////    Implementation    ////

    template<class T>
    JsonValueArrayIterator<T>::JsonValueArrayIterator(JsonArrayIterator arrayIterator, JsonArrayIterator arrayEndIterator)
        : arrayIterator(arrayIterator)
        , arrayEndIterator(arrayEndIterator)
    {
        while (this->arrayIterator != arrayEndIterator && !this->arrayIterator->template Is<T>())
        {
            arrayIterator.SetError();
            ++this->arrayIterator;
        }
    }

    template<class T>
    bool JsonValueArrayIterator<T>::operator==(const JsonValueArrayIterator<T>& other) const
    {
        return arrayIterator == other.arrayIterator && arrayEndIterator == other.arrayEndIterator;
    }

    template<class T>
    bool JsonValueArrayIterator<T>::operator!=(const JsonValueArrayIterator<T>& other) const
    {
        return !(*this == other);
    }

    template<class T>
    T JsonValueArrayIterator<T>::operator*() const
    {
        return arrayIterator->Get<T>();
    }

    template<class T>
    const T* JsonValueArrayIterator<T>::operator->() const
    {
        return &arrayIterator->Get<T>();
    }

    template<class T>
    JsonValueArrayIterator<T>& JsonValueArrayIterator<T>::operator++()
    {
        ++arrayIterator;

        while (arrayIterator != arrayEndIterator && !arrayIterator->Is<T>())
        {
            arrayIterator.SetError();
            ++this->arrayIterator;
        }

        return *this;
    }

    template<class T>
    JsonValueArrayIterator<T> JsonValueArrayIterator<T>::operator++(int)
    {
        JsonValueArrayIterator<T> result(*this);
        ++*this;
        return result;
    }
}

#endif