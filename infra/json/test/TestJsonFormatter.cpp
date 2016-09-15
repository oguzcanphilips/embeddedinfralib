#include "gtest/gtest.h"
#include "infra/json/public/JsonFormatter.hpp"

TEST(BasicUsageTest, format_json_object)
{
    infra::BoundedString::WithStorage<100> response;
    {
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, response);
        formatter.Add("name", "Upgrade 19.2");
        formatter.Add("version", "19.2");
        formatter.Add("canupgrade", true);
    }

    EXPECT_EQ(R"({ "name":"Upgrade 19.2", "version":"19.2", "canupgrade":true })", response);
}

TEST(JsonObjectFormatter, construction_results_in_empty_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
    }

    EXPECT_EQ("{  }", string);
}

TEST(JsonObjectFormatter, add_bool)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("trueTag", true);
        formatter.Add("falseTag", false);
    }

    EXPECT_EQ(R"({ "trueTag":true, "falseTag":false })", string);
}

TEST(JsonObjectFormatter, add_int)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("intTag", 0);
        formatter.Add("uint32Tag", static_cast<uint32_t>(5));
    }

    EXPECT_EQ(R"({ "intTag":0, "uint32Tag":5 })", string);
}

TEST(JsonObjectFormatter, add_const_char_ptr)
{
    infra::BoundedString::WithStorage<64> string;

    {
        const char* s = "test";
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({ "tag":"test" })", string);
}

TEST(JsonObjectFormatter, add_BoundedConstString)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::BoundedConstString s("test");
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({ "tag":"test" })", string);
}

TEST(JsonObjectFormatter, add_sub_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        {
            infra::JsonObjectFormatter subObject(formatter.SubObject("tag"));
            subObject.Add("subTagName", "value");
        }
    }

    EXPECT_EQ(R"({ "tag":{ "subTagName":"value" } })", string);
}

TEST(JsonObjectFormatter, output_is_truncated_on_small_output_string)
{
    infra::BoundedString::WithStorage<1> string;

    {
        infra::BoundedConstString s("test");
        infra::JsonObjectFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({)", string);
}

TEST(JsonArrayFormatter, construction_results_in_empty_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
    }

    EXPECT_EQ("[  ]", string);
}

TEST(JsonArrayFormatter, add_bool)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add(true);
        formatter.Add(false);
    }

    EXPECT_EQ(R"([ true, false ])", string);
}

TEST(JsonArrayFormatter, add_int)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add(0);
        formatter.Add(static_cast<uint32_t>(5));
    }

    EXPECT_EQ(R"([ 0, 5 ])", string);
}

TEST(JsonArrayFormatter, add_const_char_ptr)
{
    infra::BoundedString::WithStorage<64> string;

    {
        const char* s = "test";
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add(s);
    }

    EXPECT_EQ(R"([ "test" ])", string);
}

TEST(JsonArrayFormatter, add_BoundedConstString)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::BoundedConstString s("test");
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add(s);
    }

    EXPECT_EQ(R"([ "test" ])", string);
}

TEST(JsonArrayFormatter, add_sub_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        {
            infra::JsonObjectFormatter subObject(formatter.SubObject());
            subObject.Add("subTagName", "value");
        }
    }

    EXPECT_EQ(R"([ { "subTagName":"value" } ])", string);
}

TEST(JsonArrayFormatter, output_is_truncated_on_small_output_string)
{
    infra::BoundedString::WithStorage<1> string;

    {
        infra::BoundedConstString s("test");
        infra::JsonArrayFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add(s);
    }

    EXPECT_EQ(R"([)", string);
}

