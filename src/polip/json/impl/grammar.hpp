#ifndef INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
#define INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP

#include <functional>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include "polip/json/value.hpp"
#include "polip/json/parser.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace pjson = polip::json;

namespace polip
{
namespace json
{

template <typename Iterator>
struct DecInt64Grammar : qi::grammar<Iterator, int64_t(), ascii::space_type>
{
    DecInt64Grammar() : DecInt64Grammar::base_type(value, "json_int")
    {
        using qi::lexeme;
        using qi::lit;
        using ascii::digit;
        using ascii::char_;

        qi::int_parser<int64_t> int64_;

        value %= lexeme[
            // exclude octet integer
            !('+' | (-lit('-') >> '0' >> digit)) >> int64_ >>
            // exclude floating point
            !char_(".eE")];
    }
    qi::rule<Iterator, int64_t(), ascii::space_type> value;
};

template <typename Iterator>
struct DoubleGrammar : qi::grammar<Iterator, double(), ascii::space_type>
{
    DoubleGrammar() : DoubleGrammar::base_type(value, "json_double")
    {
        using qi::lexeme;
        using qi::lit;
        using ascii::digit;
        using qi::double_;

        value %= lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> double_];
    }
    qi::rule<Iterator, double(), ascii::space_type> value;
};

template <typename Iterator>
struct QuotedUnicodeStringGrammar : qi::grammar<Iterator, std::string()>
{
    QuotedUnicodeStringGrammar()
        : QuotedUnicodeStringGrammar::base_type(value, "json_string")
    {
        using qi::lexeme;
        using qi::char_;

        /*
            NOTE: TODO:
                1) support for embedded quot
                2) support for unicode chars
        */

        value %= lexeme['"' >> *(char_ - '"') >> '"'];
    }

    qi::rule<Iterator, std::string()> value;
};

template <typename Iterator>
class DispatchingParser : public qi::grammar<Iterator, ascii::space_type>
{
public:
    DispatchingParser(DispatchTarget& target)
        : DispatchingParser::base_type(value, "json"), m_target(target)
    {
        using qi::lexeme;
        using ascii::char_;
        using qi::lit;
        using namespace std::placeholders;

        nullValue %= lit("null");

        array %=
            char_('[')[std::bind(&DispatchTarget::arrayBegin, &m_target)] >>
            -(value % ',') >>
            char_(']')[std::bind(&DispatchTarget::arrayEnd, &m_target)];

        object %=
            char_('{') >>
            -((stringValue
                   [std::bind(&DispatchTarget::objectBegin, &m_target, _1)] >>
               ':' >> value) %
              ',') >>
            char_('}')[std::bind(&DispatchTarget::objectEnd, &m_target)];

        value %=
            (nullValue[std::bind(&DispatchTarget::nullValue, &m_target)] |
             qi::bool_[std::bind(&DispatchTarget::boolValue, &m_target, _1)] |
             int64Value
                 [std::bind(&DispatchTarget::integerValue, &m_target, _1)] |
             doubleValue
                 [std::bind(&DispatchTarget::doubleValue, &m_target, _1)] |
             stringValue
                 [std::bind(&DispatchTarget::stringValue, &m_target, _1)] |
             array | object);
    }

private:
    DispatchTarget& m_target;
    qi::rule<Iterator, ascii::space_type> nullValue;
    DecInt64Grammar<Iterator> int64Value;
    DoubleGrammar<Iterator> doubleValue;
    QuotedUnicodeStringGrammar<Iterator> stringValue;
    qi::rule<Iterator, ascii::space_type> value;
    qi::rule<Iterator, ascii::space_type> array;
    qi::rule<Iterator, ascii::space_type> object;
};

template <typename Iterator>
struct ExtendedGrammar
    : public qi::grammar<Iterator, pjson::Value(), ascii::space_type>
{
    ExtendedGrammar() : ExtendedGrammar::base_type(value, "json")
    {
        using qi::lexeme;
        using ascii::char_;
        using qi::lit;

        nullValue %= lit("null") >> qi::attr_type()(pjson::Null());
        array %= '[' >> -(value % ',') >> ']';
        member %= stringValue >> ':' >> value;
        object %= '{' >> -(member % ',') >> '}';
        value %= (nullValue | qi::bool_ | int64Value | doubleValue |
                  stringValue | array | object);
    }

    template <typename Attr>
    using Rule = qi::rule<Iterator, Attr, ascii::space_type>;

    Rule<pjson::Null()> nullValue;
    DecInt64Grammar<Iterator> int64Value;
    DoubleGrammar<Iterator> doubleValue;
    QuotedUnicodeStringGrammar<Iterator> stringValue;
    Rule<pjson::NameValue()> member;
    Rule<pjson::Array()> array;
    Rule<pjson::Object()> object;
    Rule<pjson::Value()> value;
};

}
}  // namespace polip::json

#endif  // INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
