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

decltype(stringParse("")) failure(const char* remaining = "")
{
    return std::make_pair(false, remaining);
}

void jsonExtGrammarParse(const std::string& input)
{
    auto it = input.begin();
    pjson::Value value;
    qi::phrase_parse(it, input.end(), ExtendedGrammar<std::string::const_iterator>(), ascii::space, value);
}

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

TEST(json_string_grammar, test_special_chars)
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
}

TEST(json_string_grammar, test_negative_cases)
{
    EXPECT_EQ(failure(), stringParse(""));
    EXPECT_EQ(failure("\"unclosed quot"), stringParse("\"unclosed quot"));
    EXPECT_EQ(failure("unopened quot\""), stringParse("unopened quot\""));
    //EXPECT_EQ(failure("\""), stringParse("\" \""));
}

TEST(json_string_grammar, test_negative_cases2)
{
}

TEST(json_string_grammar, test_extended_grammar_parser)
{
    std::string input = "[ null, false, ]";
    try
    {
        jsonExtGrammarParse(input);
    }
    catch(ExtendedGrammar<std::string::const_iterator>::Error& e)
    {
        std::cout << std::string(e.begin, e.where) << std::endl;
        std::cout << std::string(e.begin, e.end) << std::endl;
    }
}
