
#include <algorithm>
#include <gtest/gtest.h>
#include "polip/json/parser.hpp"
#include "polip/json/io.hpp"

using namespace polip::json;

TEST(json_parser, test_relaxed_load_empty_array)
{
    Value v = load("[]");
    EXPECT_EQ(v.arrayBegin(), v.arrayEnd());
    const Value cv = load("[]");
    EXPECT_EQ(cv.arrayBegin(), cv.arrayEnd());
}

TEST(json_parser, test_relaxed_load_empty_object)
{
    Value v = load("{}");
    EXPECT_EQ(v.objectBegin(), v.objectEnd());
    const Value cv = load("{}");
    EXPECT_EQ(cv.objectBegin(), cv.objectEnd());
}

TEST(json_parser, test_relaxed_load_null)
{
    Value v = load("null");
    EXPECT_NO_THROW(v.as<Null>());
}

TEST(json_parser, test_relaxed_load_bool_true)
{
    Value v = load("true");
    EXPECT_EQ(true, v.as<bool>());
}

TEST(json_parser, test_relaxed_load_bool_false)
{
    Value v = load("false");
    EXPECT_EQ(false, v.as<bool>());
}

TEST(json_parser, test_relaxed_load_int)
{
    EXPECT_EQ(1, load("1").as<int64_t>());
    //EXPECT_EQ(1, load("+1").as<int64_t>());
    //EXPECT_EQ(-1, load("-1").as<int64_t>());
    //EXPECT_EQ(0, load("-0").as<int64_t>());
    //EXPECT_EQ(0, load("+0").as<int64_t>());
    //EXPECT_EQ(1, load("01").as<int64_t>());
}

TEST(json_parser, test_load)
{
    std::cout << load("01") << std::endl;
    std::cout << load("+01") << std::endl;
    std::cout << load("-01") << std::endl;
    std::cout << load("1.2") << std::endl;
}

TEST(json_parser, test_invalid_json)
{
    EXPECT_THROW(load("Null"), std::runtime_error);
    EXPECT_THROW(load("False"), std::runtime_error);
    EXPECT_THROW(load("True"), std::runtime_error);
    EXPECT_THROW(load("whatever"), std::runtime_error);
}

TEST(json_parser, test_more)
{
    const std::string input = R"(
{
    "glossary": {
        "title": "example glossary",
        "GlossDiv": {
            "title": "S",
            "GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
                    "SortAs": null,
                    "GlossTerm": "Standard Generalized Markup Language",
                    "Acronym": true,
                    "Abbrev": "ISO 8879:1986",
                    "GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
                        "GlossSeeAlso": ["GML", "XML"]
                    },
                    "pi": "3.1415"
                }
            }
        }
    }
}
)";
    std::cout << load(input) << std::endl;
}
