
#include <boost/spirit/include/support_info.hpp>
#include <gtest/gtest.h>
#include "polip/json/impl/grammar.hpp"

using namespace polip::json;

namespace
{

std::pair<bool, std::string> stringParse(const std::string& input)
{
    auto it = input.begin();
    std::string result;
    bool status = qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space, result);
    return std::make_pair(
        status,
        status ? result : std::string(it, input.end())
    );
}

decltype(stringParse("")) success(const char* result = "")
{
    return std::make_pair(true, result);
}

/*decltype(stringParse("")) failure(const char* remaining = "")
{
    return std::make_pair(false, remaining);
}*/

struct UnexpectedSuccess {};
using ParseError = ExtendedGrammar<std::string::const_iterator>::Error;

ParseError expectStringParsingError(const std::string& input)
{
    try
    {
        auto it = input.begin();
        std::string result;
        qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space, result);
        std::cout << "ala ma kota" << std::endl;
    }
    catch(ExtendedGrammar<std::string::const_iterator>::Error& e)
    {
        return e;
    }
    throw UnexpectedSuccess {};
}

/*
void jsonExtGrammarParse(const std::string& input)
{
    auto it = input.begin();
    pjson::Value value;
    qi::phrase_parse(it, input.end(), ExtendedGrammar<std::string::const_iterator>(), ascii::space, value);
}

ParseError expectError(const std::string& input)
{
    try
    {
        jsonExtGrammarParse(input);
    }
    catch(ExtendedGrammar<std::string::const_iterator>::Error& e)
    {
        return e;
    }
    throw UnexpectedSuccess {};
}*/

} // anonymous namespace

TEST(json_string_grammar, test_basic_strings)
{
    EXPECT_EQ(success(), stringParse("\"\""));
    EXPECT_EQ(success(" "), stringParse("\" \""));
    EXPECT_EQ(success("aAzZ190^$'"), stringParse("\"aAzZ190^$'\""));
    EXPECT_EQ(success("simple"), stringParse("\"simple\""));
}

TEST(json_string_grammar, test_single_valid_chars)
{
    for(int i = 0x20; i <=0x7e; i++)
    {
        if(i != '\\' && i != '"')
        {
            std::stringstream ss;
            ss << '"' << static_cast<char>(i) << '"';
            EXPECT_EQ(success(std::string(1, static_cast<char>(i)).c_str()), stringParse(ss.str()));
        }
    }
}

TEST(json_string_grammar, test_valid_special_chars)
{
    EXPECT_EQ(success("\""), stringParse( R"("\"")" ));
    EXPECT_EQ(success("\b"), stringParse( R"("\b")" ));
    EXPECT_EQ(success("\f"), stringParse( R"("\f")" ));
    EXPECT_EQ(success("\n"), stringParse( R"("\n")" ));
    EXPECT_EQ(success("\r"), stringParse( R"("\r")" ));
    EXPECT_EQ(success("\t"), stringParse( R"("\t")" ));
    EXPECT_EQ(success("\\"), stringParse( R"("\\")" ));
    EXPECT_EQ(success("\\\\"), stringParse( R"("\\\\")" ));
    EXPECT_EQ(success("/"),  stringParse( R"("\/")" ));

    EXPECT_EQ(success("embed\"quot"), stringParse("\"embed\\\"quot\""));
    EXPECT_EQ(success("embed\nquot"), stringParse("\"embed\\nquot\""));
    EXPECT_EQ(success(" "), stringParse("\" \""));
}

TEST(json_string_grammar, test_invalid_special_chars)
{
    const std::string input = "\"x\\yz\"";
    ParseError e = expectStringParsingError(input);
    EXPECT_EQ(DiagError::ExpectedQuot, e.issue);
    EXPECT_EQ(input.begin() + 3, e.where);
}

TEST(json_string_grammar, test_empty_string)
{
    const std::string input = "";
    auto it = input.begin();
    EXPECT_FALSE(qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space));
    EXPECT_EQ(input.begin(), it);
}

TEST(json_string_grammar, test_not_a_quot_at_the_beginning)
{
    const std::string input = "  f";
    auto it = input.begin();
    EXPECT_FALSE(qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space));
    EXPECT_EQ(input.begin()+2, it);
}

TEST(json_string_grammar, test_unclosed_quot)
{
    const std::string input = "\"most poople finish at z";
    ParseError e = expectStringParsingError(input);
    EXPECT_EQ(DiagError::ExpectedQuot, e.issue);
    EXPECT_EQ(input.begin() + input.size(), e.where);
}

TEST(json_string_grammar, test_unopened_quot)
{
    const std::string input = "but not me\"";
    auto it = input.begin();
    EXPECT_FALSE(qi::phrase_parse(it, input.end(), QuotedUnicodeStringGrammar<std::string::const_iterator>(), ascii::space));
    EXPECT_EQ(input.begin(), it);
}

TEST(json_string_grammar, test_extended_grammar_parser)
{
    /*{
        const std::string input = "[ null, false, ]";
        ParseError e = expectError(input);
        EXPECT_EQ(DiagError::ExpectedQuot, e.issue);
        EXPECT_EQ(input.begin(), e.begin);
        EXPECT_EQ(input.end(), e.end);
        EXPECT_EQ(input.begin() + std::strlen("[ null, false"), e.where);
    }*/
}
