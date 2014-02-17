#ifndef INCLUDE_POLIP_JSON_VALUE_FWD_HPP
#define INCLUDE_POLIP_JSON_VALUE_FWD_HPP

#include <string>
#include <utility>
#include <vector>

namespace polip
{
namespace json
{

struct Null;
class Value;
using NameValue = std::pair<std::string, Value>;
using Array = std::vector<Value>;
using Object = std::vector<NameValue>;

}} // namespace polip::json

#endif // INCLUDE_POLIP_JSON_VALUE_FWD_HPP
