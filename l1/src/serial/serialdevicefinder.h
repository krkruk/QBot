#ifndef SERIALDEVICEFINDER_H
#define SERIALDEVICEFINDER_H
#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace serial
{
    class PortInfo;
    class SerialPortInfo;

/**
 * The class recognizes all available serial devices and detects their IDs.
 *
 * @brief The SerialDeviceFinder class is recognizing a serial device.
 */
class SerialDeviceFinder
{
    using result_type = std::pair<std::string, serial::PortInfo>;
    static constexpr short MAX_ATTEMPTS {5};

public:
    /**
     * The method identifies all available serial ports and tries to retrieve
     * their IDs stored in a JSON string.
     *
     * For instance, a serial device sends the following line:
     * {"0":{"content":"data"}}
     *
     * In such case, the ID is the text "0".
     *
     * As a result, the method returns a list of serial IDs and PortInfo. This
     * information allows establishing a valid connection to the devices. All
     * ID detection tasks are done asynchronously in a separate threads.
     * @brief list Lists a pairs of (SerialID, PortInfo).
     * @return a list of (SerialID, PortInfo) pairs
     */
    std::vector<result_type> list();

    /**
     * The method provides results of {list} method in a convienient form
     * of unordered_map.
     * @brief map Generates a map of data collected from {list}
     * @return A map of IDs de
     */
    std::unordered_map<std::string, serial::PortInfo> map();


private:
    result_type retrieve_id(const serial::PortInfo &port);
    bool is_valid_json(const std::string &data, boost::property_tree::ptree &json);
};

}
#endif // SERIALDEVICEFINDER_H
