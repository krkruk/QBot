#include "serialportinfo.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>
#include <array>
#include <regex>


using namespace serial;


std::string PortInfo::getPortName() const
{
    return port_name;
}

std::vector<PortInfo> SerialPortInfo::listSerials()
{
    /*
     * Implementation based on
     * https://github.com/pyserial/pyserial/blob/master/serial/tools/list_ports_linux.py
     */
    const boost::filesystem::path ttySysFS{"/sys/class/tty"};
    std::vector<boost::filesystem::path> serial_paths;

    for (auto &&file : boost::filesystem::directory_iterator(ttySysFS))
    {
        serial_paths.push_back(std::move(file));
    }

    filter_irrelevant_tty(serial_paths);
    filter_unsupported_drivers(serial_paths);

    std::vector<PortInfo> ports(serial_paths.size());
    for (std::size_t i{0}; i < serial_paths.size(); ++i)
    {
        boost::filesystem::path path {"/dev"};
        ports[i].port_name = path.append(serial_paths[i].leaf().string()).string();
    }

    return ports;
}

template<typename Container>
void SerialPortInfo::filter_irrelevant_tty(Container &&serial_paths)
{
    const std::array<std::regex, 3> regex {
        std::regex {R"(ttyUSB\d*)"},
        std::regex {R"(ttyACM\d*)"},
        std::regex {R"(ttyAMA\d*)"}
    };

    serial_paths.erase(std::remove_if(std::begin(serial_paths), std::end(serial_paths),
    [&regex](const boost::filesystem::path &path)
    {
        return std::none_of(std::cbegin(regex), std::cend(regex),
                           [leaf=path.leaf()](const std::regex &r)
        {
            return std::regex_match(leaf.string(), r);
        });
    }), std::end(serial_paths));
}

template<typename Container>
void SerialPortInfo::filter_unsupported_drivers(Container &&serial_paths)
{
    const std::array<std::string, 2> drivers {
        "usb-serial",
        "usb"
    };

    serial_paths.erase(std::remove_if(std::begin(serial_paths), std::end(serial_paths),
    [&drivers](boost::filesystem::path path)
    {
        return std::none_of(std::cbegin(drivers), std::cend(drivers),
                           [path](const auto &driver)
        {
            boost::filesystem::path pp{path};
            pp.append("device").append("subsystem");
            const auto subsytem = boost::filesystem::canonical(pp);
            return driver == subsytem.leaf();
        });
    }), std::end(serial_paths));

}
