#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H
#include <boost/asio/serial_port_base.hpp>
#include <vector>

namespace boost
{
    namespace filesystem
    {
        class path;
    }
}

namespace serial
{
class SerialPortInfo;

/**
 * @brief The PortInfo class has basic data with regard to serial port.
 */
class PortInfo
{
    friend class SerialPortInfo;
    std::string port_name;

public:
    /**
     * @brief getPortName Returns serial port name
     * @return path to serial port
     */
    std::string getPortName() const;
};



/**
 * @brief The SerialPortInfo class provides basic info about serial ports.
 */
class SerialPortInfo
{
public:
    /**
     * @brief listSerials Returns a list of available serial ports.
     * @return a list of available serial ports ready to be
     * connected to
     */
    static std::vector<PortInfo> listSerials();


private:
    template<typename Container>
    static void filter_irrelevant_tty(Container &&serial_paths);

    template<typename Container>
    static void filter_unsupported_drivers(Container &&serial_paths);
};

}
#endif // SERIALPORTINFO_H
