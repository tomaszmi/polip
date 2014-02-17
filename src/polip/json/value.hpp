#ifndef INCLUDE_POLIP_JSON_VALUE_HPP
#define INCLUDE_POLIP_JSON_VALUE_HPP

#include <boost/spirit/include/support_extended_variant.hpp>
#include "polip/json/value_fwd.hpp"
#include "polip/json/error.hpp"

namespace polip
{
namespace json
{

struct Null {};
class Value : public boost::spirit::extended_variant<
                        Null,
                        bool,
                        int64_t,
                        double,
                        std::string,
                        Array,
                        Object
                    >
{
public:
    Value() = default;

    template<typename T>
    Value(const T& v) : base_type(v) {}

    Value(const char* v) : base_type(std::string(v)) {}

    template<typename T>
    T as() const;

    using array_iterator = Array::iterator;
    using const_array_iterator = Array::const_iterator;

    const_array_iterator arrayBegin() const;
    const_array_iterator arrayEnd() const;

    array_iterator arrayBegin();
    array_iterator arrayEnd();

    using object_iterator = Object::iterator;
    using const_object_iterator = Object::const_iterator;

    const_object_iterator objectBegin() const;
    const_object_iterator objectEnd() const;

    object_iterator objectBegin();
    object_iterator objectEnd();
};

namespace details
{

template<typename T>
struct UnexpectedType;

template<> struct UnexpectedType<Null> {
    using error = not_null;
};

template<> struct UnexpectedType<bool> {
    using error = not_bool;
};

template<> struct UnexpectedType<int64_t> {
    using error = not_int;
};

template<> struct UnexpectedType<double> {
    using error = not_double;
};

template<> struct UnexpectedType<std::string> {
    using error = not_string;
};

template<> struct UnexpectedType<Array> {
    using error = not_array;
};

template<> struct UnexpectedType<Object> {
    using error = not_object;
};

} // namespace details

template<typename T>
T Value::as() const
{
    const T* const v = boost::get<T>(&base_type::get());
    if(v == nullptr)
    {
        throw typename details::UnexpectedType<T>::error{};
    }
    return *v;
}

}} // namespace polip::json

#endif // INCLUDE_POLIP_JSON_VALUE_HPP
