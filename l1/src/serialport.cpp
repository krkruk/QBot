#include "serialport.h"
#include "serialportinfo.h"
#include <boost/log/trivial.hpp>
#include <iostream>


using namespace serial;

SerialPort::SerialPort(boost::asio::io_context &ctx, const std::string &portName)
    : serial{std::make_unique<boost::asio::serial_port>(ctx)}
{
    using namespace boost::asio;
    serial->open(portName);
    // TODO: move to some kind of settings
    serial->set_option(serial_port_base::baud_rate(115200));
    serial->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    serial->set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial->set_option(serial_port_base::character_size(8));
    serial->send_break();
    connect_message_read();
}

SerialPort::SerialPort(boost::asio::io_context &ctx, const PortInfo &portName)
    : SerialPort{ctx, portName.getPortName()}
{
}

SerialPort::~SerialPort()
{
   if (serial->is_open())
   {
       on_error();
   }
}

void SerialPort::setCallback(SerialPort::Callback callback)
{
    this->callback = callback;
}

void SerialPort::write(const std::string &data)
{
    assert(data.size() < 2049);
    /*
     * Synchronous write should suffice assuming data payload
     * is small. To make sure it is small perform assert().
     */
    serial->write_some(boost::asio::buffer(data));
}

void SerialPort::connect_message_read()
{
    serial->async_read_some(boost::asio::buffer(read_bytes),
                           [&](const auto &e, auto bytes_received)
    {
        on_message_received(e, bytes_received);
    });
}

SerialPort::operator bool() const
{
    return serial->is_open();
}

void SerialPort::on_message_received(const boost::system::error_code &e, std::streamsize bytes_received)
{
    if (!e && bytes_received)
    {
        if (callback)
        {
            callback(read_bytes.data(), bytes_received);
        }
        connect_message_read();
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Serial error: " << e.message();
        on_error();
    }
}

void SerialPort::on_error()
{
    serial->cancel();
    serial->close();
}
