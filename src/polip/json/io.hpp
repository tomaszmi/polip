#ifndef INCLUDE_POLIP_JSON_IO_HPP
#define INCLUDE_POLIP_JSON_IO_HPP

#include <iosfwd>
#include "polip/json/value_fwd.hpp"

namespace polip
{
namespace json
{

std::ostream& operator<<(std::ostream& os, const Object& object);
std::ostream& operator<<(std::ostream& os, const Value& value);

struct VerboseValue
{
    VerboseValue(const Value& v) : value(v)
    {
    }
    const Value& value;
};

std::ostream& operator<<(std::ostream& os, const VerboseValue& value);

}
}  // namespace polip::json

#endif  // INCLUDE_POLIP_JSON_IO_HPP
