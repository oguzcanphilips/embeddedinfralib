#include "gtest/gtest.h"
#include "services/util/public/JsonFormatter.hpp"

TEST(BasicUsageTest, format_json_object)
{
    infra::BoundedString::WithStorage<100> response;
    {
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, response);
        formatter.Add("name", "Upgrade 19.2");
        formatter.Add("version", "19.2");
        formatter.Add("canupgrade", true);
    }

    EXPECT_EQ(R"({ "name": "Upgrade 19.2", "version": "19.2", "canupgrade": true })", response);
}

TEST(JsonFormatter, construction_results_in_empty_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
    }

    EXPECT_EQ("{  }", string);
}

TEST(JsonFormatter, add_bool)
{
    infra::BoundedString::WithStorage<64> string;

    {
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("trueTag", true);
        formatter.Add("falseTag", false);
    }

    EXPECT_EQ(R"({ "trueTag": true, "falseTag": false })", string);
}

TEST(JsonFormatter, add_int)
{
    infra::BoundedString::WithStorage<64> string;

    {
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("intTag", 0);
        formatter.Add("uint32Tag", static_cast<uint32_t>(5));
    }

    EXPECT_EQ(R"({ "intTag": 0, "uint32Tag": 5 })", string);
}

TEST(JsonFormatter, add_const_char_ptr)
{
    infra::BoundedString::WithStorage<64> string;

    {
        const char* s = "test";
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({ "tag": "test" })", string);
}

TEST(JsonFormatter, add_BoundedConstString)
{
    infra::BoundedString::WithStorage<64> string;

    {
        infra::BoundedConstString s("test");
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({ "tag": "test" })", string);
}

TEST(JsonFormatter, add_sub_object)
{
    infra::BoundedString::WithStorage<64> string;

    {
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        {
            services::JsonFormatter subObject(formatter.SubObject("tag"));
            subObject.Add("subTagName", "value");
        }
    }

    EXPECT_EQ(R"({ "tag": { "subTagName": "value" } })", string);
}

TEST(JsonFormatter, output_is_truncated_on_small_output_string)
{
    infra::BoundedString::WithStorage<1> string;

    {
        infra::BoundedConstString s("test");
        services::JsonFormatter::WithStringStream formatter(infra::inPlace, string);
        formatter.Add("tag", s);
    }

    EXPECT_EQ(R"({)", string);
}

