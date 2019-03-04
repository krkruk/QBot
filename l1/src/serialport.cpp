#include "serialport.h"
#include <iostream>


SerialPort::SerialPort(boost::asio::io_context &ctx, const std::string &portName)
    : serial{std::make_unique<boost::asio::serial_port>(ctx)}, keepalive{ctx}, port_name{portName}
{
    using namespace boost::asio;
    serial->open(portName);
    serial->set_option(serial_port_base::baud_rate(115200));
    serial->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    serial->set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial->set_option(serial_port_base::character_size(8));
    serial->send_break();
    serial->async_read_some(boost::asio::buffer(read_bytes),
                           [&](const auto &e, auto bytes_received)
    {
        on_message_read(e, bytes_received);
    });
}

SerialPort::~SerialPort()
{
   if (serial->is_open())
   {
       serial->cancel();
       serial->close();
   }
}

void SerialPort::on_message_read(const boost::system::error_code &e, std::size_t bytes_received)
{
    if (!e && bytes_received)
    {
        std::cout.write(read_bytes.data(), bytes_received);
        std::cout << std::flush;
        serial->async_read_some(boost::asio::buffer(read_bytes),
                               [&](const auto &e, auto bytes_received)
        {
            on_message_read(e, bytes_received);
        });
    }
    else
    {
        std::cerr << e.value() << ' ' << e.message() << std::endl;
        on_error();
        std::cerr << "Is open: " << serial->is_open() << std::endl;
    }
}

void SerialPort::on_timeout(const boost::system::error_code &e)
{
    using namespace boost::asio;
    std::cout << "\nTimeout: " << e.message() << std::endl;
    auto &ctx = serial->get_io_context();
    serial = std::make_unique<boost::asio::serial_port>(ctx, port_name);
    serial->set_option(serial_port_base::baud_rate(115200));
    serial->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    serial->set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial->set_option(serial_port_base::character_size(8));
    serial->send_break();
    serial->async_read_some(boost::asio::buffer(read_bytes),
                           [&](const auto &e, auto bytes_received)
    {
        on_message_read(e, bytes_received);
    });
}

void SerialPort::on_error()
{
    serial->cancel();
    serial->close();
    keepalive.expires_from_now(boost::posix_time::seconds(5));
    keepalive.async_wait([&](const auto &e){ this->on_timeout(e); });
}
