#include "serialportinfo.h"
#include "serialport.h"

template<typename Algorithm>
serial::SerialPort<Algorithm>::SerialPort(boost::asio::io_service &ctx,
                                                    const std::string &portName,
                                                    typename Algorithm::callback_type readCallback)
    : serial{std::make_unique<boost::asio::serial_port>(ctx, portName)},
      alg{readCallback}
{
    using namespace boost::asio;
    // TODO: move to some kind of a settings file e.g. YAML, INI
    serial->set_option(serial_port_base::baud_rate(115200));
    serial->set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    serial->set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial->set_option(serial_port_base::character_size(8));
    connect_message_read();
}

template<typename Algorithm>
serial::SerialPort<Algorithm>::SerialPort(boost::asio::io_service &ctx,
                                                    const PortInfo &portName,
                                                    typename Algorithm::callback_type readCallback)
    : SerialPort{ctx, portName.getPortName(), readCallback}
{
}

template<typename Algorithm>
serial::SerialPort<Algorithm>::~SerialPort()
{
   on_error();
}

template<typename Algorithm>
template<typename Message>
void serial::SerialPort<Algorithm>::write(const Message &data)
{
    const auto msg = data.toString();
    BOOST_LOG_TRIVIAL(debug) << "Send to uC: " << msg;
    assert(msg.size() < 2049);
    /*
     * Synchronous write should suffice assuming data payload
     * is small. To make sure it is small perform assert().
     */
    if (*this)
    {
        serial->write_some(boost::asio::buffer(msg));
    }
}

template<typename Algorithm>
void serial::SerialPort<Algorithm>::write(const std::string &data)
{
    BOOST_LOG_TRIVIAL(debug) << "Send to uC: " << data;
    assert(data.size() < 2049);
    /*
     * Synchronous write should suffice assuming data payload
     * is small. To make sure it is small perform assert().
     */
    if (*this)
    {
        serial->write_some(boost::asio::buffer(data));
    }
}

template<typename Algorithm>
void serial::SerialPort<Algorithm>::connect_message_read()
{
    if (*this)
    {
        // here is a bug when deleting an object...
        // consider using shared_from_this to pass a reference
        serial->async_read_some(boost::asio::buffer(read_bytes),
                               [&](const auto &e, auto bytes_received)
        {
            on_message_received(e, bytes_received);
        });
    }
}

template<typename Algorithm>
serial::SerialPort<Algorithm>::operator bool() const
{
    return serial->is_open();
}

template<typename Algorithm>
void serial::SerialPort<Algorithm>::on_message_received(const boost::system::error_code &e, std::streamsize bytes_received)
{
    if (!e && bytes_received)
    {
        /**
         * Left for debugging in future. Might be useful. If not, just delete it.
         */
//        std::cout << "line: ";
//        for (std::streamsize i{0}; i < bytes_received; ++i)
//        {
//            std::cout << (int) read_bytes[i] << ' ';
//        }
//        std::cout << std::endl;
        alg(read_bytes.data(), bytes_received);
        connect_message_read();
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Serial error: " << e.message()
                                 << " code=" << e.value()
                                 << " category=" << e.category().name();
        on_error();
    }
}

template<typename Algorithm>
void serial::SerialPort<Algorithm>::on_error()
{
    if (*this)
    {
        try
        {
            serial->cancel();
            serial->close();
        }
        catch (std::exception &e)
        {
            BOOST_LOG_TRIVIAL(error) << "Error when canceling serial: "
                                     << e.what();

        }
    }
}
