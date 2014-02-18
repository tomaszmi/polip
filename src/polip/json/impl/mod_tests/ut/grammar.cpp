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

TEST(json_grammar, test_string_grammar)
{
    EXPECT_EQ(success(), parse("\"\""));
    EXPECT_EQ(failure(), parse(""));
    EXPECT_EQ(success("simple"), parse("\"simple\""));
    EXPECT_EQ(failure("\"unclosed quot"), parse("\"unclosed quot"));
    EXPECT_EQ(failure("unopened quot\""), parse("unopened quot\""));
}
