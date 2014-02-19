#include <gtest/gtest.h>
#include "polip/json/impl/grammar.hpp"

using namespace polip::json;

namespace
{

std::pair<bool, std::string> parse(const std::string& input)
{
    auto it = input.begin();
    std::string result;
    bool status = qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space, result);
    return std::make_pair(
        status,
        status ? result : std::string(it, input.end())
    );
}

decltype(parse("")) success(const char* result = "")
{
    return std::make_pair(true, result);
}

decltype(parse("")) failure(const char* remaining = "")
{
    return std::make_pair(false, remaining);
}

} // anonymous namespace

TEST(json_string_grammar, test_basic_strings)
{
    EXPECT_EQ(success(), parse("\"\""));
    EXPECT_EQ(success(" "), parse("\" \""));
    EXPECT_EQ(success("aAzZ190^$'"), parse("\"aAzZ190^$'\""));
    EXPECT_EQ(success("simple"), parse("\"simple\""));
}

TEST(json_string_grammar, test_single_valid_chars)
{
    for(int i = 0x20; i <=0x7e; i++)
    {
        if(i != '\\' && i != '"')
        {
            std::stringstream ss;
            ss << '"' << static_cast<char>(i) << '"';
            EXPECT_EQ(success(std::string(1, static_cast<char>(i)).c_str()), parse(ss.str()));
        }
    }
}

TEST(json_string_grammar, test_special_chars)
{
    EXPECT_EQ(success("\""), parse( R"("\"")" ));
    EXPECT_EQ(success("\b"), parse( R"("\b")" ));
    EXPECT_EQ(success("\f"), parse( R"("\f")" ));
    EXPECT_EQ(success("\n"), parse( R"("\n")" ));
    EXPECT_EQ(success("\r"), parse( R"("\r")" ));
    EXPECT_EQ(success("\t"), parse( R"("\t")" ));
    EXPECT_EQ(success("\\"), parse( R"("\\")" ));
    EXPECT_EQ(success("\\\\"), parse( R"("\\\\")" ));
    EXPECT_EQ(success("/"),  parse( R"("\/")" ));

    EXPECT_EQ(success("embed\"quot"), parse("\"embed\\\"quot\""));
    EXPECT_EQ(success("embed\nquot"), parse("\"embed\\nquot\""));
}

TEST(json_string_grammar, test_negative_cases)
{
    EXPECT_EQ(failure(), parse(""));
    EXPECT_EQ(failure("\"unclosed quot"), parse("\"unclosed quot"));
    EXPECT_EQ(failure("unopened quot\""), parse("unopened quot\""));
    //EXPECT_EQ(failure("\""), parse("\" \""));
}
