#include "jsondispatcherprocessor.h"

template<typename IdType, typename OutputType>
bool JsonDispatcherProcessor<IdType, OutputType>::isValid(IdType id) const
{
    return content.find(get_id(id)) != content.not_found();
}

template<typename IdType, typename OutputType>
void JsonDispatcherProcessor<IdType, OutputType>::operator()(const std::string &data)
{
    std::stringstream ss(data);
    boost::property_tree::json_parser::read_json(ss, content);
}

template<typename IdType, typename OutputType>
std::string JsonDispatcherProcessor<IdType, OutputType>::get_id(IdType id) const
{
    return boost::lexical_cast<std::string>(id);
}

/**
 * Template specializations
 */
template
class JsonDispatcherProcessor<int, std::string>;

template
class JsonDispatcherProcessor<int, boost::property_tree::ptree>;

template
class JsonDispatcherProcessor<std::string, std::string>;

template
class JsonDispatcherProcessor<std::string, boost::property_tree::ptree>;
