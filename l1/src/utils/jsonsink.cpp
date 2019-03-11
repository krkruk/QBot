#include "jsonsink.h"
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <string>


std::string JsonSink::toString(bool pretty) const
{
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, json, pretty);
    return ss.str();
}
