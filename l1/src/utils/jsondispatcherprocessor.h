#ifndef JSONDISPATCHERPROCESSOR_H
#define JSONDISPATCHERPROCESSOR_H
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
#include <type_traits>
#include <functional>
#include <sstream>
#include <string>


/**
 * JsonDispatcherProcessor is an algorithm that performs preliminary
 * parsing of a received line of JSON data. Once parsed, it provides
 * accessors to retrieved information for each of the present IDs of
 * IdType.
 *
 * The class works with cooperation with MappedDispatcher. The template
 * requires the following arguments:
 *  * IdType - a type of ID
 *  * OutputType - a type of data to be returned in get() method.
 *
 * JsonDispatcher is a class that specializes the template. It is possible
 * to use only the following specializations:
 *   class JsonDispatcherProcessor<int, std::string>;
 *   class JsonDispatcherProcessor<int, boost::property_tree::ptree>;
 *   class JsonDispatcherProcessor<std::string, std::string>;
 *   class JsonDispatcherProcessor<std::string, boost::property_tree::ptree>;
 */
template<typename IdType, typename OutputType>
class JsonDispatcherProcessor
{
public:
    using id_type = IdType;
    using output_type = OutputType;
    using callback_type = std::function<void(id_type, output_type)>;

    /**
     * @brief isValid Checks if an ID of type IdType is present in
     * a received set data
     * @param id an identification label
     * @return true if the Processor has the data
     */
    bool isValid(IdType id) const;

    /**
     * @brief process Parses JSON
     * @param data one-liner JSON string
     */
    void process(const std::string &data);


    /**
     * @brief Retrieves JSON parameters for a given id
     * @param id an identification label
     * @return std::string returns a readable text JSON for the given ID
     */
    template <typename _output_type = OutputType>
    auto get(IdType id) const -> typename std::enable_if_t<
        std::is_same<std::string, _output_type>::value, _output_type>
    {
        const auto child = content.get_child(get_id(id));
        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, child, false);
        return ss.str();
    }

    /**
     * @brief Retrieves JSON parameters for a given id
     * @param id an identification label
     * @return boost::property_tree::ptree returns a branch of JSON in
     * boost::property_tree::ptree representation
     */
    template <typename _output_type = OutputType>
    auto get(IdType id) const -> typename std::enable_if_t<
        !std::is_same<std::string, _output_type>::value, _output_type>
    {
        return content.get_child(get_id(id));
    }



private:
    std::string get_id(IdType id) const;
    boost::property_tree::ptree content;
};

#endif // JSONDISPATCHERPROCESSOR_H
