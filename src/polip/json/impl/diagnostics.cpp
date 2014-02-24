#include <stdexcept>
#include "diagnostics.hpp"

namespace pjson = polip::json;

void pjson::Diagnostics::add(const std::string& ruleName, DiagError issue)
{
    if(!m_diags.insert(std::make_pair(ruleName, issue)).second)
    {
        throw std::runtime_error("diagnostic already added");
    }
}

pjson::DiagError pjson::Diagnostics::get(const std::string& ruleName) const
{
    auto it = m_diags.find(ruleName);
    return it == m_diags.end() ? DiagError::Other : it->second;
}
