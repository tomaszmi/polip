#include <gtest/gtest.h>
#include "polip/json/value.hpp"
#include "polip/json/error.hpp"

using namespace polip::json;

TEST(json_value, test_value_as_null)
{
    EXPECT_EQ(Null{}, Value {}.as<Null>());
    EXPECT_EQ(Null{}, Value{Null{}}.as<Null>());
}

TEST(json_value, test_value_as_bool)
{
    EXPECT_EQ(true, Value{true}.as<bool>());
    EXPECT_EQ(false, Value{false}.as<bool>());
}

TEST(json_value, test_value_as_int)
{
    EXPECT_EQ(0, Value{int64_t{}}.as<int64_t>());
    EXPECT_EQ(10, Value{int64_t{10}}.as<int64_t>());
    EXPECT_EQ(-13, Value{int64_t{-13}}.as<int64_t>());
    EXPECT_THROW(Value{int64_t{}}.as<double>(), not_double);
}

TEST(json_value, test_value_as_double)
{
    EXPECT_DOUBLE_EQ(1.3, Value{1.3}.as<double>());
}

TEST(json_value, test_value_as_string)
{
    EXPECT_EQ("qwerty", Value{"qwerty"}.as<std::string>());
    EXPECT_EQ("qwe\"rty", Value{"qwe\"rty"}.as<std::string>());
    EXPECT_TRUE(Value{""}.as<std::string>().empty());
    EXPECT_TRUE(Value{std::string{}}.as<std::string>().empty());
}

TEST(json_value, test_value_as_array)
{
    EXPECT_TRUE(Value{Array{}}.as<Array>().empty());
    EXPECT_TRUE(Value{Array{true}}.as<Array>()[0].as<bool>());

    {
        Value v{
            Array{
                Null{},
                true,
                false,
                int64_t{123},
                Value{
                    Array{
                        true,
                        int64_t{4567}
                    }
                },
                4.0
            }
        };
        const Array& a = v.as<Array>();
        ASSERT_EQ(6, a.size());
        EXPECT_NO_THROW(a[0].as<Null>());
        EXPECT_EQ(true, a[1].as<bool>());
        EXPECT_EQ(false, a[2].as<bool>());
        EXPECT_EQ(123, a[3].as<int64_t>());
        ASSERT_EQ(2, a[4].as<Array>().size());
        EXPECT_EQ(true, a[4].as<Array>()[0].as<bool>());
        EXPECT_EQ(4567, a[4].as<Array>()[1].as<int64_t>());
        EXPECT_DOUBLE_EQ(4.0, a[5].as<double>());
    }
}

TEST(json_value, test_value_as_array_of_arrays)
{
    Value v {
        Array {
            Array {
                Array {
                }
            }
        }
    };
    ASSERT_EQ(1, v.as<Array>().size());
    ASSERT_EQ(1, v.as<Array>()[0].as<Array>().size());
    ASSERT_EQ(0, v.as<Array>()[0].as<Array>()[0].as<Array>().size());
}

TEST(json_value, test_value_as_object)
{
    EXPECT_TRUE(Value{Object{}}.as<Object>().empty());

    {
        Value v = Object{
            {"a", true},
            {"b", "qwerty"}
        };
        const Object& obj = v.as<Object>();
        ASSERT_EQ(2, obj.size());
        EXPECT_EQ("a", obj[0].first);
        EXPECT_EQ(true, obj[0].second.as<bool>());
        EXPECT_EQ("b", obj[1].first);
        EXPECT_EQ("qwerty", obj[1].second.as<std::string>());
    }
}

TEST(json_value, test_value_as_object_of_objects)
{
    Object o {{
        "o1", Object{{
            "o2", Object{
                { "o3", Null{} },
                { "o4", false  }
            }
        }}
    }};

    ASSERT_EQ(1, o.size());
    EXPECT_EQ("o1", o[0].first);
    ASSERT_EQ(1, o[0].second.as<Object>().size());
    EXPECT_EQ("o2", o[0].second.as<Object>()[0].first);

    ASSERT_EQ(2, o[0].second.as<Object>()[0].second.as<Object>().size());
    EXPECT_EQ("o3", o[0].second.as<Object>()[0].second.as<Object>()[0].first);
    EXPECT_EQ(Null{}, o[0].second.as<Object>()[0].second.as<Object>()[0].second.as<Null>());

    EXPECT_EQ("o4", o[0].second.as<Object>()[0].second.as<Object>()[1].first);
    EXPECT_FALSE(o[0].second.as<Object>()[0].second.as<Object>()[1].second.as<bool>());
}

TEST(json_value, test_getting_invalid_value)
{
    EXPECT_THROW(Value{ Null{} }.as<bool>(), not_bool);
    EXPECT_THROW(Value{ Null{} }.as<int64_t>(), not_int);
    EXPECT_THROW(Value{ Null{} }.as<double>(), not_double);
    EXPECT_THROW(Value{ Null{} }.as<std::string>(), not_string);
    EXPECT_THROW(Value{ Null{} }.as<Array>(), not_array);
    EXPECT_THROW(Value{ Null{} }.as<Object>(), not_object);
    EXPECT_THROW(Value{ true }.as<Null>(), not_null);
}

TEST(json_value, test_value_equal)
{
    Value v1{ Array{ int64_t{1}, Null{}, false } };
    Value v2{ Array{ int64_t{2}, Null{}, false } };
    Value v3{ Object{{ "x", Null{}}} };
    Value v4{ Object{{ "y", Null{}}} };
    Value v5{
        Object{
            { "y", Null{} },
            { "y", Null{} }
        }
    };

    EXPECT_TRUE(v1 == v1);
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v3 == v3);
    EXPECT_TRUE(v3 != v4);
    EXPECT_TRUE(v4 != v5);

    Value v6 { int64_t{1} };
    Value v7 { double{1} };

    EXPECT_TRUE(v6 != v7);

    Value v8 { Object{} };
    Value v9 { Object{} };
    Value v10 {
        Object {
            { "", Object{} }
        }
    };

    EXPECT_TRUE(v8 == v9);
    EXPECT_TRUE(v9 != v10);
}
