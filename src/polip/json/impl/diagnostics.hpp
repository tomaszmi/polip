#ifndef INCLUDE_POLIP_JSON_IMPL_DIAGNOSTICS_HPP
#define INCLUDE_POLIP_JSON_IMPL_DIAGNOSTICS_HPP

#include <map>
#include <string>
#include "polip/json/error.hpp"

namespace polip
{
namespace json
{

class Diagnostics
{
public:
    template<typename... T>
    Diagnostics(T&&... init) : m_diags{ std::forward<T>(init)... } {}

    void add(const std::string& ruleName, DiagError issue);
    DiagError get(const std::string& ruleName) const;

private:
    std::map<std::string, DiagError> m_diags;
};

}
}  // namespace polip::json

#endif // INCLUDE_POLIP_JSON_IMPL_DIAGNOSTICS_HPP
