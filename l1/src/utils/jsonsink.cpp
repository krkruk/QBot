#include "jsonsink.h"
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <string>


std::string JsonSink::toString(bool pretty)
{
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, json, pretty);
    json.clear();
    return ss.str();
}
