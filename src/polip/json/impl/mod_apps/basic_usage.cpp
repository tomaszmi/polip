#include <iostream>
#include "polip/json/parser.hpp"
#include "polip/json/io.hpp"

namespace pjson = polip::json;

class ObjectBuilder : public pjson::DispatchTarget
{
private:
    void objectBeginImpl(const std::string& name)
    {
        std::cout << "on object begin: " << name << std::endl;
    }
    void objectEndImpl()
    {
        std::cout << "on object end" << std::endl;
    }
    void arrayBeginImpl()
    {
        std::cout << "on array begin" << std::endl;
    }
    void arrayEndImpl()
    {
        std::cout << "on array end" << std::endl;
    }
    void nullValueImpl()
    {
        std::cout << "null" << std::endl;
    }
    void boolValueImpl(bool v)
    {
        std::cout << "bool: " << v << std::endl;
    }
    void numberValueImpl(double v)
    {
        std::cout << "number: " << v << std::endl;
    }
    void stringValueImpl(const std::string& v)
    {
        std::cout << "string: " << v << std::endl;
    }
};

int main(int, char**)
{
    const std::string input = R"(
{
    "glossary": {
        "title": "example glossary",
        "GlossDiv": {
            "title": "S",
            "GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
                    "SortAs": null,
                    "GlossTerm": "Standard Generalized Markup Language",
                    "Acronym": true,
                    "Abbrev": "ISO 8879:1986",
                    "GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
                        "GlossSeeAlso": ["GML", "XML"]
                    },
                    "pi": 31415
                }
            }
        }
    }
}
)";

    try
    {
        std::cout << pjson::load(input) << std::endl;
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unknown error" << std::endl;
    }
    return 1;
}
