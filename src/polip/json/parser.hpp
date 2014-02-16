#ifndef INCLUDE_POLIP_JSON_PARSER_HPP
#define INCLUDE_POLIP_JSON_PARSER_HPP

#include <string>
#include "polip/json/value.hpp"

namespace polip
{
namespace json
{

enum class Conformance
{
    Relaxed,    // Python-like
    Strict      // RFC 4627 compliant
};

Value load(const std::string& jsonDoc, Conformance level = Conformance::Relaxed);

class DispatchTarget
{
public:
    virtual ~DispatchTarget() {}

    void objectBegin(const std::string& name) { objectBeginImpl(name); }
    void objectEnd() { objectEndImpl(); }
    void arrayBegin() { arrayBeginImpl(); }
    void arrayEnd() { arrayEndImpl(); }
    void nullValue() { nullValueImpl(); }
    void boolValue(bool v) { boolValueImpl(v); }
    void integerValue(int64_t v) { integerValueImpl(v); }
    void doubleValue(double v) { doubleValueImpl(v); }
    void stringValue(const std::string& v) { stringValueImpl(v); }

private:
    virtual void objectBeginImpl(const std::string& name) = 0;
    virtual void objectEndImpl() = 0;
    virtual void arrayBeginImpl() = 0;
    virtual void arrayEndImpl() = 0;
    virtual void nullValueImpl() = 0;
    virtual void boolValueImpl(bool v) = 0;
    virtual void integerValueImpl(int64_t v) = 0;
    virtual void doubleValueImpl(double v) = 0;
    virtual void stringValueImpl(const std::string& v) = 0;
};

void parse(const std::string& jsonDoc, DispatchTarget& builder);


}} // namespace polip::json

#endif // INCLUDE_POLIP_JSON_PARSER_HPP
