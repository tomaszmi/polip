
#include <algorithm>
#include <gtest/gtest.h>
#include "polip/json/parser.hpp"

using namespace polip::json;

TEST(json_parser, test_relaxed_load_empty_array)
{
    {
        Value v = load("[]");
        EXPECT_EQ(v.arrayBegin(), v.arrayEnd());
    }
    {
        const Value v = load("[]");
        EXPECT_EQ(v.arrayBegin(), v.arrayEnd());
    }
}

TEST(json_parser, test_relaxed_load_empty_object)
{
    {
        Value v = load("{}");
        EXPECT_EQ(v.objectBegin(), v.objectEnd());
    }
    {
        const Value v = load("{}");
        EXPECT_EQ(v.objectBegin(), v.objectEnd());
    }
}

TEST(json_parser, test_relaxed_load_null)
{
    EXPECT_NO_THROW(load("null").as<Null>());
}

TEST(json_parser, test_relaxed_load_bool)
{
    EXPECT_EQ(true, load("true").as<bool>());
    EXPECT_EQ(false, load("false").as<bool>());
}

TEST(json_parser, test_relaxed_load_int)
{
    EXPECT_EQ(1, load("1").as<int64_t>());
    EXPECT_EQ(0, load("0").as<int64_t>());
    EXPECT_EQ(0, load("-0").as<int64_t>());
    EXPECT_EQ(-1, load("-1").as<int64_t>());
}

TEST(json_parser, test_relaxed_load_invalid_int)
{
    EXPECT_THROW(load("+0"), parse_error);
    EXPECT_THROW(load("01"), parse_error);
    EXPECT_THROW(load("+1"), parse_error);
    EXPECT_THROW(load("+01"), parse_error);
    EXPECT_THROW(load("-01"), parse_error);
}

TEST(json_parser, test_relaxed_load_double)
{
    EXPECT_DOUBLE_EQ(1., load("1.0").as<double>());
    EXPECT_DOUBLE_EQ(0.1, load("0.1").as<double>());
    EXPECT_DOUBLE_EQ(3.1415, load("3.1415").as<double>());
    EXPECT_DOUBLE_EQ(-2.13, load("-2.13").as<double>());
    EXPECT_DOUBLE_EQ(0.1, load("1e-1").as<double>());
    EXPECT_DOUBLE_EQ(0, load("0e1").as<double>());
    EXPECT_DOUBLE_EQ(0.01, load("1E-2").as<double>());
    EXPECT_DOUBLE_EQ(0.333, load("333E-3").as<double>());
    EXPECT_DOUBLE_EQ(12, load("1.2E+1").as<double>());
    EXPECT_DOUBLE_EQ(130, load("1.3E2").as<double>());
}

TEST(json_parser, test_relaxed_load_invalid_double)
{
    EXPECT_THROW(load("01.2"), parse_error);
    EXPECT_THROW(load("01e2"), parse_error);
    EXPECT_THROW(load("+1e2"), parse_error);
    EXPECT_THROW(load("1a2"), parse_error);
    EXPECT_THROW(load("+1.2"), parse_error);
    EXPECT_THROW(load("e1"), parse_error);
}

TEST(json_parser, test_relaxed_load_unknown_kwd)
{
    EXPECT_THROW(load("Null"), parse_error);
    EXPECT_THROW(load("False"), parse_error);
    EXPECT_THROW(load("True"), parse_error);
    EXPECT_THROW(load("whatever"), parse_error);
}

TEST(json_parser, test_relaxed_load_string)
{
    EXPECT_EQ("ala", load(R"("ala")").as<std::string>());
    EXPECT_EQ("to be or not to be", load(R"("to be or not to be")").as<std::string>());
    EXPECT_EQ("space1 spaces2  tab\ttabs2\t\tend", load("\"space1 spaces2  tab\ttabs2\t\tend\"").as<std::string>());
    EXPECT_EQ("", load("\"\"").as<std::string>());
    // TODO unicode testing cases
}

TEST(json_parser, DISABLED_test_relaxed_load_string_embedded_quot)  // TODO ENABLE IT
{
    EXPECT_EQ("a\"la", load("\"a\"la\"").as<std::string>());
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
    //std::cout << load(input) << std::endl;
}
