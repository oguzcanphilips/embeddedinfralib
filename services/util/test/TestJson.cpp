#include "gtest/gtest.h"
#include "services/util/public/Json.hpp"

TEST(BasicUsageTest, object_with_some_values)
{
    services::JsonObject object(R"({ "key" : "value", "enabled" : true, "subobject" : { "nested": true } })");

    EXPECT_EQ("value", object.GetString("key"));
    EXPECT_EQ(true, object.GetBoolean("enabled"));
    EXPECT_EQ(true, object.GetObject("subobject").GetBoolean("nested"));
}

TEST(BasicUsageTest, array_with_strings)
{
    services::JsonArray array(R"([ "first", "second", "third" ])");

    for (auto string : JsonStringArray(array))
        EXPECT_TRUE(string == "first" || string == "second" || string == "third");
}

TEST(JsonTokenizerTest, get_end_token)
{
    services::JsonTokenizer tokenizer(R"()");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::End()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_empty_string_token)
{
    services::JsonTokenizer tokenizer(R"("")");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::String("")), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_string_token)
{
    services::JsonTokenizer tokenizer(R"("string")");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::String("string")), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_escaped_string_token)
{
    services::JsonTokenizer tokenizer(R"("str\"ing")");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::String(R"(str\"ing)")), tokenizer.Token());
}

TEST(JsonTokenizerTest, unclosed_string_results_in_error_token)
{
    services::JsonTokenizer tokenizer(R"("str)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Error()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_end_token_after_string_token)
{
    services::JsonTokenizer tokenizer(R"("string")");

    ASSERT_EQ(services::JsonToken::Token(services::JsonToken::String("string")), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::End()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_int_token)
{
    services::JsonTokenizer tokenizer("42");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Integer(42)), tokenizer.Token());
}

TEST(JsonTokenizerTest, skip_whitespace_before_end)
{
    services::JsonTokenizer tokenizer(R"( )");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::End()), tokenizer.Token());
}

TEST(JsonTokenizerTest, skip_tab_whitespace_before_end)
{
    services::JsonTokenizer tokenizer("\t");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::End()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_true_token)
{
    services::JsonTokenizer tokenizer(R"(true)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Boolean(true)), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_false_token)
{
    services::JsonTokenizer tokenizer(R"(false)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Boolean(false)), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_colon_token)
{
    services::JsonTokenizer tokenizer(R"(:)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Colon()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_comma_token)
{
    services::JsonTokenizer tokenizer(R"(,)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Comma()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_left_brace_token)
{
    services::JsonTokenizer tokenizer(R"({)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::LeftBrace(0)), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_right_brace_token)
{
    services::JsonTokenizer tokenizer(R"(})");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::RightBrace(0)), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_left_bracket_token)
{
    services::JsonTokenizer tokenizer(R"([)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::LeftBracket(0)), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_right_bracket_token)
{
    services::JsonTokenizer tokenizer(R"(])");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::RightBracket(0)), tokenizer.Token());
}

TEST(JsonTokenizerTest, unknown_character_results_in_error_token)
{
    services::JsonTokenizer tokenizer(R"(~)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Error()), tokenizer.Token());
}

TEST(JsonTokenizerTest, unknown_identifier_results_in_error_token)
{
    services::JsonTokenizer tokenizer(R"(identifier)");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Error()), tokenizer.Token());
}

TEST(JsonTokenizerTest, get_multiple_tokens)
{
    services::JsonTokenizer tokenizer(R"({ "key" : "value" })");

    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::LeftBrace(0)), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::String("key")), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::Colon()), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::String("value")), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::RightBrace(18)), tokenizer.Token());
    EXPECT_EQ(services::JsonToken::Token(services::JsonToken::End()), tokenizer.Token());
}

TEST(JsonObjectIteratorTest, empty_object_iterator_compares_equal_to_end)
{
    services::JsonObject object(R"({ })");
    services::JsonObjectIterator iterator(object.begin());
    services::JsonObjectIterator endIterator(object.end());

    EXPECT_EQ(endIterator, iterator);
}

TEST(JsonObjectIteratorTest, nonempty_object_iterator_does_not_compare_equal_to_end)
{
    services::JsonObject object(R"({ "key" : "value" })");
    services::JsonObjectIterator iterator(object.begin());
    services::JsonObjectIterator endIterator(object.end());

    EXPECT_NE(endIterator, iterator);
}

TEST(JsonObjectIteratorTest, get_key_from_iterator)
{
    services::JsonObject object(R"({ "key" : "value" })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ("key", iterator->key);
}

TEST(JsonObjectIteratorTest, after_next_iterator_is_end)
{
    services::JsonObject object(R"({ "key" : "value" })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(object.end(), ++iterator);
}

TEST(JsonObjectIteratorTest, get_second_key_from_iterator)
{
    services::JsonObject object(R"({ "key" : "value", "second_key" : "second_value" })");
    services::JsonObjectIterator iterator(object.begin());

    ++iterator;
    EXPECT_EQ("second_key", iterator->key);
}

TEST(JsonObjectIteratorTest, get_second_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : "value", "second_key" : "second_value" })");
    services::JsonObjectIterator iterator(object.begin());

    ++iterator;
    EXPECT_EQ("second_value", iterator->value.Get<infra::BoundedConstString>());
}

TEST(JsonObjectIteratorTest, get_string_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : "value" })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ("value", iterator->value.Get<infra::BoundedConstString>());
}

TEST(JsonObjectIteratorTest, get_integer_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : 42 })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(42, iterator->value.Get<int32_t>());
}

TEST(JsonObjectIteratorTest, get_true_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : true })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(true, iterator->value.Get<bool>());
}

TEST(JsonObjectIteratorTest, get_false_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : false })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(false, iterator->value.Get<bool>());
}

TEST(JsonObjectIteratorTest, get_object_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : { "bla" } })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(R"({ "bla" })", iterator->value.Get<services::JsonObject>().ObjectString());
}

TEST(JsonObjectIteratorTest, get_object_value_with_nested_object_from_iterator)
{
    services::JsonObject object(R"({ "key" : { "bla" : { } } })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(R"({ "bla" : { } })", iterator->value.Get<services::JsonObject>().ObjectString());
}

TEST(JsonObjectIteratorTest, get_array_value_from_iterator)
{
    services::JsonObject object(R"({ "key" : [ "bla" ] })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(R"([ "bla" ])", iterator->value.Get<services::JsonArray>().ObjectString());
}

TEST(JsonObjectIteratorTest, get_object_value_from_iterator_with_error)
{
    services::JsonObject object(R"({ "key" : { "bla" : ~ } })");

    EXPECT_EQ(object.end(), object.begin());
}

TEST(JsonObjectIteratorTest, get_array_value_from_iterator_with_error)
{
    services::JsonObject object(R"({ "key" : [ "bla", ~ ] })");

    EXPECT_EQ(object.end(), object.begin());
}

TEST(JsonObjectIteratorTest, get_array_value_with_nested_array_from_iterator)
{
    services::JsonObject object(R"({ "key" : [ "bla", [ ] ] })");
    services::JsonObjectIterator iterator(object.begin());

    EXPECT_EQ(R"([ "bla", [ ] ])", iterator->value.Get<services::JsonArray>().ObjectString());
}

TEST(JsonObjectTest, empty_object_construction)
{
    services::JsonObject object(R"({ })");

    EXPECT_EQ(object.end(), object.begin());
}

TEST(JsonObjectTest, nonempty_object_construction)
{
    services::JsonObject object(R"({ "key" : "value" })");

    EXPECT_NE(object.end(), object.begin());
}

TEST(JsonObjectTest, has_key_when_value_exists)
{
    services::JsonObject object(R"({ "key" : "value" })");

    EXPECT_TRUE(object.HasKey("key"));
}

TEST(JsonObjectTest, has_key_when_value_does_not_exist)
{
    services::JsonObject object(R"({ })");

    EXPECT_FALSE(object.HasKey("key"));
}

TEST(JsonObjectTest, iterate_over_object)
{
    services::JsonObject object(R"({ "key" : "value" })");

    for (auto keyValue : object)
        EXPECT_EQ("value", keyValue.value.Get<infra::BoundedConstString>());
}

TEST(JsonObjectTest, incorrect_object_sets_error)
{
    std::vector<infra::BoundedConstString> errorObjects = { R"(~)", R"({~})", R"({ true })", R"({ "key" ~})", R"({ "key" : ~})", R"({ "key" : [})", R"({ "key" : true ~})" };

    for (auto errorObject : errorObjects)
    {
        services::JsonObject object(errorObject);

        for (auto keyValue : object)
        {}

        EXPECT_TRUE(object.Error());
    }
}

TEST(JsonObjectTest, get_string)
{
    services::JsonObject object(R"({ "key" : "value" })");

    EXPECT_EQ("value", object.GetString("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_boolean)
{
    services::JsonObject object(R"({ "key" : true })");

    EXPECT_EQ(true, object.GetBoolean("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_integer)
{
    services::JsonObject object(R"({ "key" : 5 })");

    EXPECT_EQ(5, object.GetInteger("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_object)
{
    services::JsonObject object(R"({ "key" : { "bla" } })");

    EXPECT_EQ(R"({ "bla" })", object.GetObject("key").ObjectString());
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_array)
{
    services::JsonObject object(R"({ "key" : [ "bla" ] })");

    EXPECT_EQ(R"([ "bla" ])", object.GetArray("key").ObjectString());
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_optional_string)
{
    services::JsonObject object(R"({ "key" : "value" })");

    EXPECT_EQ("value", *object.GetOptionalString("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_none_when_optional_string_is_absent)
{
    services::JsonObject object(R"({ })");

    EXPECT_EQ(infra::none, object.GetOptionalString("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_optional_boolean)
{
    services::JsonObject object(R"({ "key" : true })");

    EXPECT_EQ(true, *object.GetOptionalBoolean("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_none_when_optional_boolean_is_absent)
{
    services::JsonObject object(R"({ })");

    EXPECT_EQ(infra::none, object.GetOptionalBoolean("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_none_when_optional_boolean_is_absent_but_key_is_present)
{
    services::JsonObject object(R"({ "key" : "value" })");

    EXPECT_EQ(infra::none, object.GetOptionalBoolean("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_optional_object)
{
    services::JsonObject object(R"({ "key" : { "bla" } })");

    EXPECT_EQ(R"({ "bla" })", object.GetOptionalObject("key")->ObjectString());
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_none_when_optional_object_is_absent)
{
    services::JsonObject object(R"({ })");

    EXPECT_EQ(infra::none, object.GetOptionalObject("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_none_when_optional_array_is_absent)
{
    services::JsonObject object(R"({ })");

    EXPECT_EQ(infra::none, object.GetOptionalArray("key"));
    EXPECT_FALSE(object.Error());
}

TEST(JsonObjectTest, get_nonexistent_string_sets_error_on_object)
{
    services::JsonObject object(R"({ })");

    EXPECT_FALSE(object.Error());
    object.GetString("key");
    EXPECT_TRUE(object.Error());
}

TEST(JsonArrayIteratorTest, empty_array_iterator_compares_equal_to_end)
{
    services::JsonArray jsonArray(R"([ ])");
    services::JsonArrayIterator iterator(jsonArray.begin());
    services::JsonArrayIterator endIterator(jsonArray.end());

    EXPECT_EQ(endIterator, iterator);
}

TEST(JsonArrayIteratorTest, nonempty_array_iterator_does_not_compare_equal_to_end)
{
    services::JsonArray jsonArray(R"([ "key" : "value" ])");
    services::JsonArrayIterator iterator(jsonArray.begin());
    services::JsonArrayIterator endIterator(jsonArray.end());

    EXPECT_NE(endIterator, iterator);
}

TEST(JsonObjectIteratorTest, get_value_from_iterator)
{
    services::JsonArray jsonArray(R"([ "value" ])");
    services::JsonArrayIterator iterator(jsonArray.begin());

    EXPECT_EQ("value", iterator->Get<infra::BoundedConstString>());
}

TEST(JsonObjectIteratorTest, get_multiple_values_from_iterator)
{
    services::JsonArray jsonArray(R"([ "value", true, { "subobject" } ])");
    services::JsonArrayIterator iterator(jsonArray.begin());

    EXPECT_EQ("value", (iterator++)->Get<infra::BoundedConstString>());
    EXPECT_EQ(true, (iterator++)->Get<bool>());
    EXPECT_EQ(R"({ "subobject" })", (*iterator++).Get<services::JsonObject>().ObjectString());
}

TEST(JsonArrayTest, empty_array_construction)
{
    services::JsonArray jsonArray(R"([ ])");

    EXPECT_EQ(jsonArray.end(), jsonArray.begin());
}

TEST(JsonArrayTest, nonempty_array_construction)
{
    services::JsonArray jsonArray(R"([ "value" ])");

    EXPECT_NE(jsonArray.end(), jsonArray.begin());
}

TEST(JsonArrayTest, iterate_over_array)
{
    services::JsonArray jsonArray(R"([ "value" ])");

    for (auto value : jsonArray)
        EXPECT_EQ("value", value.Get<infra::BoundedConstString>());

    EXPECT_EQ(1, std::distance(jsonArray.begin(), jsonArray.end()));
}

TEST(JsonArrayTest, iterate_over_strings_in_array)
{
    services::JsonArray jsonArray(R"([ "value" ])");

    for (auto string : JsonStringArray(jsonArray))
        EXPECT_EQ("value", string);

    EXPECT_EQ(1, std::distance(JsonStringArray(jsonArray).first, JsonStringArray(jsonArray).second));
}

TEST(JsonArrayTest, iterate_over_strings_in_array_with_other_values_sets_error)
{
    services::JsonArray jsonArray(R"([ true, "value" ])");

    for (auto string : JsonStringArray(jsonArray))
    {}

    EXPECT_TRUE(jsonArray.Error());
}

TEST(JsonArrayTest, iterate_over_booleans_in_array)
{
    services::JsonArray jsonArray(R"([ true ])");

    for (auto boolean : JsonBooleanArray(jsonArray))
        EXPECT_EQ(true, boolean);

    EXPECT_EQ(1, std::distance(JsonBooleanArray(jsonArray).first, JsonBooleanArray(jsonArray).second));
}

TEST(JsonArrayTest, iterate_over_integers_in_array)
{
    services::JsonArray jsonArray(R"([ 5 ])");

    for (auto integer : JsonIntegerArray(jsonArray))
        EXPECT_EQ(5, integer);

    EXPECT_EQ(1, std::distance(JsonIntegerArray(jsonArray).first, JsonIntegerArray(jsonArray).second));
}

TEST(JsonArrayTest, iterate_over_objects_in_array)
{
    services::JsonArray jsonArray(R"([ { "name": "Richard" } ])");

    for (auto object : JsonObjectArray(jsonArray))
        EXPECT_EQ(R"({ "name": "Richard" })", object.ObjectString());

    EXPECT_EQ(1, std::distance(JsonObjectArray(jsonArray).first, JsonObjectArray(jsonArray).second));
}

TEST(JsonArrayTest, iterate_over_arrays_in_array)
{
    services::JsonArray jsonArray(R"([ [ true ] ])");

    for (auto array : JsonArrayArray(jsonArray))
        EXPECT_EQ("[ true ]", array.ObjectString());

    EXPECT_EQ(1, std::distance(JsonArrayArray(jsonArray).first, JsonArrayArray(jsonArray).second));
}

TEST(JsonArrayTest, incorrect_array_sets_error)
{
    std::vector<infra::BoundedConstString> errorArrays = { R"(~)", R"([~])", R"([ "value" ~])", R"([ "value" , ~])", R"([ { ])" };

    for (auto errorArray : errorArrays)
    {
        services::JsonArray jsonArray(errorArray);

        for (auto value : jsonArray)
        {}

        EXPECT_TRUE(jsonArray.Error());
    }
}
