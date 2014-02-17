#ifndef INCLUDE_POLIP_JSON_ERROR_HPP
#define INCLUDE_POLIP_JSON_ERROR_HPP

#include <exception>

namespace polip
{
namespace json
{

struct error : std::exception {};

struct parse_error : error {};
struct not_null : error {};
struct not_bool : error {};
struct not_int : error {};
struct not_double : error {};
struct not_string : error {};
struct not_array : error {};
struct not_object : error {};

}} // namespace polip::json

#endif // INCLUDE_POLIP_JSON_ERROR_HPP
