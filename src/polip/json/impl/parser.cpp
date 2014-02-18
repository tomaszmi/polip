#include <sstream>
#include <stdexcept>
#include "polip/json/parser.hpp"
#include "polip/json/error.hpp"
#include "grammar.hpp"

namespace pjson = polip::json;

pjson::Value pjson::load(const std::string& jsonDoc, Conformance level)
{
    /*
        TODO:
            handle conformance level
            process errors
     */
    auto it = jsonDoc.begin();
    ExtendedGrammar<std::string::const_iterator> parser;
    pjson::Value value;
    bool success =
        qi::phrase_parse(it, jsonDoc.end(), parser, ascii::space, value);
    if (success && it == jsonDoc.end()) {
        return value;
    }
    std::cout << "parsing error, still to parse: " +
                     std::string(it, jsonDoc.end()) << std::endl;
    throw parse_error{};
}

void pjson::parse(const std::string& jsonDoc, DispatchTarget& builder)
{
    // TODO
}
