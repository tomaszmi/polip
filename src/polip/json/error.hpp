#ifndef INCLUDE_POLIP_JSON_ERROR_HPP
#define INCLUDE_POLIP_JSON_ERROR_HPP

#include <exception>
#include <string>

namespace polip
{
namespace json
{

enum class DiagError
{
    Other,
    ExpectedArrayBegin,
    ExpectedArrayEnd,
    ExpectedObjectBegin,
    ExpectedObjectEnd,
    ExpectedSpecialChar,
    ExpectedUnicodeChar,
    ExpectedString,
    ExpectedQuot,
    ExpectedEscape,
    InvalidSpecialChar,
    Coma,
    Colon,
    Null,
    Int,
    Double,
    String,
    Member,
    Array,
    Object,
    Value
};

struct error : std::exception {};

template<typename Iterator>
struct parse_error : error
{
    parse_error(DiagError issue_, Iterator begin_, Iterator end_,
                Iterator where_, const std::string& info_)
        : issue(issue_), begin(begin_), end(end_), where(where_), info(info_)
    {
    }

    DiagError issue;
    Iterator begin;
    Iterator end;
    Iterator where;
    std::string info;
};

struct not_null : error {};
struct not_bool : error {};
struct not_int : error {};
struct not_double : error {};
struct not_string : error {};
struct not_array : error {};
struct not_object : error {};

}
}  // namespace polip::json

#endif  // INCLUDE_POLIP_JSON_ERROR_HPP
