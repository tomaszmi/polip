#include <ostream>
#include "polip/json/io.hpp"

namespace pjson = polip::json;

namespace {

struct Indent
{
    unsigned level;
    std::size_t indent_size;  // # of spaces
};

std::ostream& operator<<(std::ostream& os, const Indent& in)
{
    os << '\n' << std::string(in.level*in.indent_size, ' ');
    return os;
}

class OutStreamer : public boost::static_visitor<void>
{
public:
    OutStreamer(std::ostream& os, unsigned level) : m_os(os), m_level(level)
    {
    }

    void operator()(const pjson::Null& elem) const
    {
        m_os << "null";
    }

    void operator()(bool elem) const
    {
        m_os << (elem ? "true" : "false");
    }

    void operator()(int64_t elem) const
    {
        m_os << elem;
    }

    void operator()(double elem) const
    {
        m_os << elem;
    }

    void operator()(const std::string& elem) const
    {
        m_os << '"' << elem << '"';
    }

    void operator()(const pjson::Array& array) const
    {
        m_os << "[";
        for(auto const& item : array)
        {
            boost::apply_visitor(OutStreamer(m_os, m_level+1), item);
            m_os << ", ";
        }
        m_os << "]";
    }

    void operator()(const pjson::Object& object) const
    {
        m_os << Indent{m_level, 4} << '{';
        for(auto const& pair : object)
        {
            m_os << Indent{1, 4} << '"' << pair.first << "\": ";
            boost::apply_visitor(OutStreamer(m_os, m_level+1), pair.second);
        }
        m_os << Indent{m_level, 4} << '}';
    }

private:
    std::ostream& m_os;
    unsigned m_level;
};

} // anonymous namespace

std::ostream& pjson::operator<<(std::ostream& os, const Object& object)
{
    os << '{';
    for(auto const& pair : object)
    {
        os << Indent{1, 4} << '"' << pair.first << "\": ";
        boost::apply_visitor(OutStreamer(os, 1), pair.second);
    }
    os << Indent{0, 4} << '}';
    return os;
}

std::ostream& pjson::operator<<(std::ostream& os, const Value& value)
{
    boost::apply_visitor(OutStreamer(os, 1), value);
    return os;
}
