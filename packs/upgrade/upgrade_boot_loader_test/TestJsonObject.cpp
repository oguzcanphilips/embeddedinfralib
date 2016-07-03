#include "gmock/gmock.h"
#include "upgrade_boot_loader/JsonObject.hpp"

TEST(JsonObjectTest, GetStringFindsString)
{
    infra::BoundedConstString::WithStorage<256> objectString(R"({"string":"value"})");
    application::JsonObject object(objectString);

    EXPECT_EQ("value", object.GetString("string"));
    EXPECT_TRUE(object.Valid());
}

TEST(JsonObjectTest, GetStringSignalsErrorWhenNotFound)
{
    infra::BoundedConstString::WithStorage<256> objectString(R"({"string":"value"})");
    application::JsonObject object(objectString);

    object.GetString("value");
    EXPECT_FALSE(object.Valid());
}