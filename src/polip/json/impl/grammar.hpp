#ifndef INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
#define INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP

#include <functional>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include "polip/json/error.hpp"
#include "polip/json/parser.hpp"
#include "polip/json/value.hpp"
#include "diagnostics.hpp"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace pjson = polip::json;

namespace polip
{
namespace json
{

template<typename Iterator>
struct ErrorHandler
{
    struct Handle
    {
        struct result { typedef void type; };

        void operator()(const Diagnostics& diagnostics, Iterator begin, Iterator end, Iterator where, const boost::spirit::info& info) const
        {
            std::cout << info << std::endl;
            throw parse_error<Iterator>{ diagnostics.get(info.tag), begin, end, where, "" };
        }
    };

    phx::function<Handle> handle;
};

template <typename Iterator>
struct DecInt64Grammar : qi::grammar<Iterator, int64_t(), ascii::space_type>
{
    explicit DecInt64Grammar(const std::string& name = "int") : DecInt64Grammar::base_type(value, name)
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
    explicit DoubleGrammar(const std::string& name = "double") : DoubleGrammar::base_type(value, name)
    {
        using qi::lexeme;
        using qi::lit;
        using ascii::digit;
        using qi::double_;

        value %= lexeme[!('+' | (-lit('-') >> '0' >> digit)) >> double_];
    }
    qi::rule<Iterator, double(), ascii::space_type> value;
};

struct AddSpecChar
{
    template <typename Sig>
    struct result
    {
        typedef void type;
    };

    void operator()(std::string& s, char ch) const
    {
        switch (ch)
        {
            case 'b':
                s += '\b'; break;
            case 'f':
                s += '\f'; break;
            case 'n':
                s += '\n'; break;
            case 'r':
                s += '\r'; break;
            case 't':
                s += '\t'; break;
            case '"':
            case '\\':
            case '/':
                s += ch;
                break;
        }
    }
};

template <typename Iterator>
struct QuotedUnicodeStringGrammar : qi::grammar<Iterator, std::string()>
{
    explicit QuotedUnicodeStringGrammar(const std::string& name = "string")
        : QuotedUnicodeStringGrammar::base_type(value, name),
          escape(qi::lit('\\'), "\\"),
          quot(qi::lit('"'), "\"")
    {
        value.name("value");
        unescaped.name("unescaped");
        escaped.name("escaped");

        diags.add(value.name(), DiagError::ExpectedUnicodeChar);
        diags.add(unescaped.name(), DiagError::ExpectedString);
        diags.add(escaped.name(), DiagError::ExpectedSpecialChar);
        diags.add(escape.name(), DiagError::ExpectedEscape); 
        diags.add(quot.name(), DiagError::ExpectedQuot);

        using qi::char_;
        using qi::_val;

        phx::function<AddSpecChar> addSpecChar;

        /*
            NOTE: TODO: support for unicode chars
            Add support for \v special char (extended json)
        */
        escaped %= escape > char_("\"\\/bfnrt")[addSpecChar(qi::_r1, qi::_1)];
        unescaped %= char_("\x20-\x21\x23-\x5b\x5d-\x7e");
        value %= quot > *(escaped(_val) | unescaped) > quot;

        using namespace boost::spirit::qi::labels;
        qi::on_error<qi::fail>(value, failure.handle(phx::ref(diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(unescaped, failure.handle(phx::ref(diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(escaped, failure.handle(phx::ref(diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(escape, failure.handle(phx::ref(diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(quot, failure.handle(phx::ref(diags), _1, _2, _3, _4));
    }

    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, std::string()> unescaped;
    qi::rule<Iterator, void(std::string&)> escaped;
    qi::rule<Iterator> escape, quot;

    ErrorHandler<Iterator> failure;
    Diagnostics diags;
};

template<typename Iterator>
struct Tokens
{
    Tokens()
        : nullText(qi::lit("null"), "null"),
          arrayBegin(qi::lit("["), "["),
          arrayEnd(qi::lit("]"), "]"),
          objectBegin(qi::lit("{"), "{"),
          objectEnd(qi::lit("}"), "}"),
          comma(qi::lit(","), ","),
          colon(qi::lit(":"), ":"),
          null(std::string("null")),
          int64(std::string("int")),
          _double(std::string("double")),
          string(std::string("string")),
          member(std::string("member")),
          array(std::string("array")),
          object(std::string("object")),
          value(std::string("value")),
          diags(
              std::initializer_list<std::pair<const std::string, DiagError>>{
                  {"null", DiagError::Null},
                  {"[", DiagError::ExpectedArrayBegin},
                  {"]", DiagError::ExpectedArrayEnd},
                  {"{", DiagError::ExpectedObjectBegin},
                  {"}", DiagError::ExpectedObjectEnd},
                  {",", DiagError::Coma},
                  {":", DiagError::Colon},
                  {"int", DiagError::Int},
                  {"double", DiagError::Double},
                  {"string", DiagError::String},
                  {"member", DiagError::Member},
                  {"array", DiagError::Array},
                  {"object", DiagError::Object},
                  {"value", DiagError::Value}})
    {
    }

    template<typename Attr>
    using Rule = qi::rule<Iterator, Attr, ascii::space_type>;

    qi::rule<Iterator> nullText, arrayBegin, arrayEnd, objectBegin, objectEnd,
        comma, colon;

    Rule<pjson::Null()> null;
    DecInt64Grammar<Iterator> int64;
    DoubleGrammar<Iterator> _double;
    QuotedUnicodeStringGrammar<Iterator> string;
    Rule<pjson::NameValue()> member;
    Rule<pjson::Array()> array;
    Rule<pjson::Object()> object;
    Rule<pjson::Value()> value;

    ErrorHandler<Iterator> failure;
    Diagnostics diags;
};

template <typename Iterator>
class DispatchingExtendedGrammar : public qi::grammar<Iterator, ascii::space_type>
{
public:
    DispatchingExtendedGrammar(DispatchTarget& target)
        : DispatchingExtendedGrammar::base_type(json.value, "json"), m_target(target)
    {
        using ascii::char_;
        using qi::lit;
        namespace plc = std::placeholders;

        json.null %= json.nullText;

        json.array %=
            json.arrayBegin[std::bind(&DispatchTarget::arrayBegin, &m_target)] >>
            -(json.value % json.coma) >>
            json.arrayEnd[std::bind(&DispatchTarget::arrayEnd, &m_target)];

        json.object %=
            json.objectBegin >>
            -((json.string[std::bind(&DispatchTarget::objectBegin, &m_target, plc::_1)] >> json.colon >> json.value) % json.coma) >>
            json.objectEnd[std::bind(&DispatchTarget::objectEnd, &m_target)];

        json.value %=
            (json.null[std::bind(&DispatchTarget::nullValue, &m_target)] |
             qi::bool_[std::bind(&DispatchTarget::boolValue, &m_target, plc::_1)] |
             json.int64[std::bind(&DispatchTarget::integerValue, &m_target, plc::_1)] |
             json._double[std::bind(&DispatchTarget::doubleValue, &m_target, plc::_1)] |
             json.string[std::bind(&DispatchTarget::stringValue, &m_target, plc::_1)] |
             json.array | json.object);
    }

private:
    DispatchTarget& m_target;
    Tokens<Iterator> json;
};

template <typename Iterator>
struct ExtendedGrammar
    : public qi::grammar<Iterator, pjson::Value(), ascii::space_type>
{
    using Error = parse_error<Iterator>;

    ExtendedGrammar() : ExtendedGrammar::base_type(json.value, "json")
    {
        json.null %= json.nullText > qi::attr_type()(pjson::Null());
        json.array %= json.arrayBegin >> -(json.value % json.comma) >> json.arrayEnd;
        json.member %= json.string > json.colon > json.value;
        json.object %= json.objectBegin > -(json.member % json.comma) > json.objectEnd;
        json.value %= (json.null | qi::bool_ | json.int64 | json._double | json.string | json.array | json.object);

        using namespace boost::spirit::qi::labels;
        qi::on_error<qi::fail>(json.null, json.failure.handle(phx::ref(json.diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(json.array, json.failure.handle(phx::ref(json.diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(json.member, json.failure.handle(phx::ref(json.diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(json.object, json.failure.handle(phx::ref(json.diags), _1, _2, _3, _4));
        qi::on_error<qi::fail>(json.value, json.failure.handle(phx::ref(json.diags), _1, _2, _3, _4));
    }

    Tokens<Iterator> json;
};

}
}  // namespace polip::json

#endif  // INCLUDE_POLIP_JSON_IMPL_GRAMMAR_HPP
