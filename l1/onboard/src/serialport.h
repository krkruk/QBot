#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <boost/asio/serial_port.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <memory>
#include <string>
#include <array>


class SerialPort
{
    std::unique_ptr<boost::asio::serial_port> serial;
    boost::asio::deadline_timer keepalive;
    std::string port_name;
    std::array<char, 4096> read_bytes;

public:
    explicit SerialPort(boost::asio::io_context &ctx, const std::string &portName);
    ~SerialPort();

private:
    void on_message_read(const boost::system::error_code &e, std::size_t bytes_received);
    void on_timeout(const boost::system::error_code &e);
    void on_error();
};

#endif // SERIALPORT_H
