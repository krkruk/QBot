#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>

#include "serialport.h"
#include "serialportinfo.h"


int main()
{
    int exit_code {0};
    auto ports = serial::SerialPortInfo::listSerials();
    for (auto &path : ports)
    {
        std::cout << path.getPortName()
                  << std::endl;
    }

    boost::asio::io_context ctx;
    serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>
            port{ctx, ports[0], serial::PrintContent{}};
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
