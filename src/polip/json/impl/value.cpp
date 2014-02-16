#include "polip/json/value.hpp"

namespace pjson = polip::json;

namespace {

enum class What2Get
{
    Begin,
    End
};

template<typename T>
struct ResultType;

template<>
struct ResultType<pjson::Array>
{
    static pjson::Value::const_array_iterator nop(const pjson::Value::variant_type& v);
    static pjson::Value::array_iterator nop(pjson::Value::variant_type& v);
};

template<>
struct ResultType<pjson::Object>
{
    static pjson::Value::const_object_iterator nop(const pjson::Value::variant_type& v);
    static pjson::Value::object_iterator nop(pjson::Value::variant_type& v);
};

template<typename T>
auto constIterableElem(const pjson::Value::variant_type& v, What2Get get) -> decltype(ResultType<T>::nop(v))
{
    const T* const got = boost::get<T>(&v);
    if(got == nullptr)
    {
        throw typename pjson::details::UnexpectedType<T>::error{};
    }
    return get == What2Get::Begin ? got->begin() : got->end();
}

template<typename T>
auto iterableElem(pjson::Value::variant_type& v, What2Get get) -> decltype(ResultType<T>::nop(v))
{
    T* got = boost::get<T>(&v);
    if(got == nullptr)
    {
        throw typename pjson::details::UnexpectedType<T>::error{};
    }
    return get == What2Get::Begin ? got->begin() : got->end();
}

} // anonymous namespace

pjson::Value::const_array_iterator pjson::Value::arrayBegin() const
{
    return constIterableElem<Array>(base_type::get(), What2Get::Begin);
}

pjson::Value::const_array_iterator pjson::Value::arrayEnd() const
{
    return constIterableElem<Array>(base_type::get(), What2Get::End);
}

pjson::Value::array_iterator pjson::Value::arrayBegin()
{
    return iterableElem<Array>(base_type::get(), What2Get::Begin);
}

pjson::Value::array_iterator pjson::Value::arrayEnd()
{
    return iterableElem<Array>(base_type::get(), What2Get::End);
}

pjson::Value::const_object_iterator pjson::Value::objectBegin() const
{
    return constIterableElem<Object>(base_type::get(), What2Get::Begin);
}

pjson::Value::const_object_iterator pjson::Value::objectEnd() const
{
    return constIterableElem<Object>(base_type::get(), What2Get::End);
}

pjson::Value::object_iterator pjson::Value::objectBegin()
{
    return iterableElem<Object>(base_type::get(), What2Get::Begin);
}

pjson::Value::object_iterator pjson::Value::objectEnd()
{
    return iterableElem<Object>(base_type::get(), What2Get::End);
}
