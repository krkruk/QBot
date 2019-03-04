#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "serialport.h"

int main()
{
    int exit_code {0};
    boost::asio::io_context ctx;
    SerialPort port{ctx, "/dev/ttyUSB0"};
    boost::asio::signal_set sig{ctx, SIGINT, SIGTERM};
    sig.async_wait([&ctx, &exit_code](const auto &err, int sig_num)
    {
        if (!err)
        {
            std::cerr << "Error: " << sig_num << std::endl;
            ctx.stop();
            exit_code = sig_num;
        }
    });
    ctx.run();
    return exit_code;
}
