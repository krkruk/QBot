#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <unordered_map>
#include <iostream>
#include <vector>

#include "serialdevicefinder.h"
#include "chassismodel.h"
#include "grpcserver.h"

auto detect_serials()
{
    serial::SerialDeviceFinder finder;
    auto mapped_serials = finder.map();
    for (auto &serial : mapped_serials)
    {
        BOOST_LOG_TRIVIAL(info) << "Detected ID=" << serial.first
                                << " @ port=" << serial.second.getPortName();
    }

    if (mapped_serials.size() < ChassisModel::wheelCount())
    {
        BOOST_LOG_TRIVIAL(error) << "Some serials have not been detected. Quit.";
        exit(-1);
    }

    return mapped_serials;
}

int main()
{
    int exit_code {0};
    boost::asio::io_context io_ctx;
    boost::asio::signal_set sig{io_ctx, SIGINT, SIGTERM};
    sig.async_wait([&io_ctx, &exit_code](const auto &err, int sig_num)
    {
        if (!err)
        {
            BOOST_LOG_TRIVIAL(error) << "Signal received: " << sig_num
                                     << ", " << err;
            io_ctx.stop();
            exit_code = sig_num;
            BOOST_LOG_TRIVIAL(info) << "IO Context has been stopped.";
        }
    });

    auto mapped_serials = detect_serials();
    ChassisModel model{io_ctx, mapped_serials};
    GrpcServer grpc_server{model.getExecutor(), model.getVisitor()};
    boost::asio::steady_timer timer{io_ctx, boost::asio::chrono::seconds{10}};
    timer.async_wait([&model](const auto &)
    {
        BOOST_LOG_TRIVIAL(info) << "Serials OK?" << model.checkSerialHealth();
    });
    io_ctx.run();
    return exit_code;
}
