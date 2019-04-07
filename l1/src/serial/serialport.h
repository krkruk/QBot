#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/log/trivial.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace serial
{

class PortInfo;


/**
 * @brief The PrintContent struct Simple functor to print incoming
 * serial data
 */
struct PrintContent
{
    /**
     * @brief operator () Prints received data
     * @param line a line of data
     */
    void operator()(const std::string &line)
    {
        BOOST_LOG_TRIVIAL(trace) << line;
    }
};


/**
 * Algorithm that reads lines of data and notifies
 * with a Callback.
 */
template<typename Callback>
class ReadLineAlgorithm
{
    Callback callback;
    std::stringstream ss;
public:
    using callback_type = Callback;

    explicit ReadLineAlgorithm(Callback callback)
        : callback{callback}
    {}

    /**
     * The function processes incoming data. It triggers an
     * instance of Callback once a full line of data is received
     * (delimiter '\n').
     *
     * @brief operator () Accepts SerialPort data once a package of
     * data arrives
     * @param data a package of data in a raw form
     * @param bytes_received how many bytes have been received
     */
    void operator()(const char *data, std::streamsize bytes_received)
    {
        ss.write(data, bytes_received);

        std::string line;
        /**
         * Here we have a bug.
         * To read correctly a line, it is necessary to check in '\n' exists
         * anywhere in the string not just in the last position.
         * getline() returns true if:
         *  * reaches EOF
         *  * find '\n'
         *  * line.max_size() is reached
         *
         * Problematic is reaching of EOF because it falsly generates a new
         * line if a data package is incomplete (does not contain '\n').
         *
         * This must be fixed it some time in the future but for now
         * it works...
         */
        if (data[bytes_received - 1] == '\n' && std::getline(ss, line))
        {
            callback(line);
        }
    }
};

/**
 * The SerialPort class provides a basic interface to access
 * serial device.
 * @brief The SerialPort class access a serial device.
 * @param Algorithm an algorithm that processes incoming data
 * and notifies about a received message when necessary (according to
 * the algorithm>
 */
template<typename Algorithm>
class SerialPort
{
    std::unique_ptr<boost::asio::serial_port> serial;
    std::array<char, 4096> read_bytes;
    Algorithm alg;


public:
    /**
     * @brief SerialPort Constructor, automatically connects to a given serial
     * @param ctx io_context
     * @param portName Name of the port to connect to
     * @param readCallback Callback that receives a package of data
     * do be processed with. Requires an Algorithm to be processed in a correct
     * way. The callback must correspond to the required callback by the Algorithm
     */
    explicit SerialPort(boost::asio::io_context &ctx,
                        const std::string &portName,
                        typename Algorithm::callback_type readCallback);

    /**
     * @brief SerialPort Constructor, automatically connects to a given serial
     * @param ctx io_context
     * @param portName Name of the port to connect to
     * @param readCallback Callback that receives a package of data
     * do be processed with. Requires an Algorithm to be processed in a correct
     * way. The callback must correspond to the required callback by the Algorithm
     */
    explicit SerialPort(boost::asio::io_context &ctx,
                        const PortInfo &portName,
                        typename Algorithm::callback_type readCallback);

    /**
     * Destructor
     */
    ~SerialPort();

    /**
     * @brief write Writes to the serial device. Requires toString()
     * method
     * @param data some data in a required format
     */
    template<typename Message>
    void write(const Message &data);

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

#include "serialport.cpp"

#endif // SERIALPORT_H
