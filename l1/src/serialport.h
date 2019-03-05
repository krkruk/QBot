#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <boost/asio/serial_port.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <array>

namespace serial
{

class PortInfo;
struct PrintContent
{
    void operator()(const char *data, std::streamsize bytes_received)
    {
        std::cout.write(data, bytes_received);
        std::cout << std::flush;
    }
};

/**
 * The SerialPort class provides a basic interface to access
 * serial device.
 * @brief The SerialPort class access a serial device.
 */
class SerialPort
{
    using Callback = std::function<void(const char*, std::streamsize)>;

    std::unique_ptr<boost::asio::serial_port> serial;
    std::array<char, 4096> read_bytes;
    Callback callback {PrintContent()};

public:
    explicit SerialPort(boost::asio::io_context &ctx, const std::string &portName);
    explicit SerialPort(boost::asio::io_context &ctx, const PortInfo &portName);
    ~SerialPort();

    /**
     * Sets a callback. The callback must be aligned with the following type:
     * void(const char *data, std::streamsize bytes_received)
     * @brief setCallback Sets a callback.
     * @param callback a callback that is to be envoked every time
     * a new data package arrives.
     */
    void setCallback(Callback callback);

    /**
     * @brief write Writes to the serial device
     * @param data some data in a required format
     */
    void write(const std::string &data);

    /**
     * @brief operator bool Checks if SerialPort is opened
     */
    operator bool() const;

private:
    void connect_message_read();
    void on_message_received(const boost::system::error_code &e, std::streamsize bytes_received);
    void on_error();
};

}

#endif // SERIALPORT_H
