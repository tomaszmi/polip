#ifndef INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
#define INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/bind.hpp>
#include "polip/json/value.hpp"

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
    DecInt64Grammar()
    : DecInt64Grammar::base_type(value, "dec_int64")
    {
        using qi::lexeme;
        using qi::lit;
        using ascii::digit_type;
        using ascii::char_;

        qi::int_parser<int64_t> int64_;

        value %= lexeme[
            // exclude octet integer
            !('+' | (-lit('-') >> '0' >> digit_type())) >>
            int64_ >>
            // exclude floating point
            !char_(".eE")];
    }
    qi::rule<Iterator, int64_t(), ascii::space_type> value;
};

template<typename Iterator>
class DispatchingParser : public qi::grammar<Iterator, ascii::space_type>
{
public:
    DispatchingParser(DispatchTarget& target)
    : DispatchingParser::base_type(value, "json"), m_target(target)
    {
        using qi::lexeme;
        using ascii::char_;
        using qi::lit;

        quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
        nullValue %= lit("null");

        array %= char_('[')[boost::bind(&DispatchTarget::arrayBegin, &m_target)] >>
                    -(value % ',') >>
                 char_(']')[boost::bind(&DispatchTarget::arrayEnd, &m_target)];

        object %= char_('{') >>
                        -((quoted_string[boost::bind(&DispatchTarget::objectBegin, &m_target, _1)] >> ':' >> value) % ',') >>
                  char_('}')[boost::bind(&DispatchTarget::objectEnd, &m_target)];

        value %= (
            nullValue[boost::bind(&DispatchTarget::nullValue, &m_target)] |
            qi::bool_[boost::bind(&DispatchTarget::boolValue, &m_target, _1)] |
            int64Value[boost::bind(&DispatchTarget::integerValue, &m_target, _1)] |
            qi::double_[boost::bind(&DispatchTarget::doubleValue, &m_target, _1)] |
            quoted_string[boost::bind(&DispatchTarget::stringValue, &m_target, _1)] |
            array |
            object
        );
    }

private:
    DispatchTarget& m_target;
    qi::rule<Iterator, ascii::space_type> nullValue;
    DecInt64Grammar<Iterator> int64Value;
    qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
    qi::rule<Iterator, ascii::space_type> value;
    qi::rule<Iterator, ascii::space_type> array;
    qi::rule<Iterator, ascii::space_type> object;
};

template<typename Iterator>
struct ExtendedGrammar : public qi::grammar<Iterator, pjson::Value(), ascii::space_type>
{
    ExtendedGrammar()
    : ExtendedGrammar::base_type(value, "json")
    {
        using qi::lexeme;
        using ascii::char_;
        using qi::lit;
        using qi::double_;

        nullValue = lit("null") >> qi::attr_type()(pjson::Null());
        quotedString = lexeme['"' >> +(char_ - '"') >> '"'];
        array = '[' >> -(value % ',') >> ']';
        member = quotedString >> ':' >> value;
        object = '{' >> -(member % ',') >> '}';
        value = (
            nullValue |
            qi::bool_ |
            int64Value |
            double_ |
            quotedString |
            array |
            object
        );
    }

    template<typename Attr>
    using Rule = qi::rule<Iterator, Attr, ascii::space_type>;

    Rule<pjson::Null()> nullValue;
    DecInt64Grammar<Iterator> int64Value;
    qi::rule<Iterator, std::string()> quotedString;
    Rule<pjson::NameValue()> member;
    Rule<pjson::Array()> array;
    Rule<pjson::Object()> object;
    Rule<pjson::Value()> value;
};

}} // namespace polip::json

#endif // INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
