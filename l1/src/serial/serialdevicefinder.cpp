#include "serialdevicefinder.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <future>

#include "serialportinfo.h"
#include "serialport.h"


std::vector<serial::SerialDeviceFinder::result_type> serial::SerialDeviceFinder::list()
{
    auto ports = serial::SerialPortInfo::listSerials();
    using port_future = std::future<result_type>;
    std::vector<port_future> futures;
    for (const auto &port : ports)
    {
        futures.push_back(std::async(std::launch::async,
                                     &serial::SerialDeviceFinder::retrieve_id,
                                     this, std::cref(port)));
    }

    std::vector<result_type> results(futures.size());
    std::transform(std::begin(futures), std::end(futures), std::begin(results),
                   [](port_future &future) -> result_type
    {
        try
        {
            return future.get();
        }
        catch (std::runtime_error &e)
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot detect id of the serial. " << e.what();
            return {};
        }
    });
    return results;
}

std::unordered_map<std::string, serial::PortInfo> serial::SerialDeviceFinder::map()
{
    auto serial_list = list();
    std::unordered_map<std::string, serial::PortInfo> mapping;
    for (auto &&entry : serial_list)
    {
        mapping.insert(std::move(entry));
    }
    return mapping;
}

serial::SerialDeviceFinder::result_type
serial::SerialDeviceFinder::retrieve_id(const serial::PortInfo &port)
{
    using namespace boost::asio;
    io_service ctx;

    // To be moved into a properties file
    boost::asio::serial_port serial{ctx, port.getPortName()};
    serial.set_option(serial_port_base::baud_rate(115200));
    serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial.set_option(serial_port_base::character_size(8));

    boost::property_tree::ptree json;
    streambuf message;
    std::string data;
    short attempt {0};
    do
    {
        read_until(serial, message, '\n');
        std::istream in(&message);
        std::getline(in, data);
    }
    while (!is_valid_json(data, json) && ++attempt < MAX_ATTEMPTS);

    if (attempt >= MAX_ATTEMPTS || json.size() == 0)
    {
        throw std::runtime_error("Could not retrieve data from the "
                                 + port.getPortName() + " serial device.");
    }

    return {json.front().first, port};
}

bool serial::SerialDeviceFinder::is_valid_json(
        const std::string &data, boost::property_tree::ptree &json)
{
    using namespace boost::property_tree;
    std::stringstream ss{data};
    try
    {
        read_json(ss, json);
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(warning) << "Cannot read JSON. " << e.what();
        return false;
    }
    return true;
}
