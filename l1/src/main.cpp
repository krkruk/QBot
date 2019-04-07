#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>

#include "serialport.h"
#include "serialportinfo.h"
#include "serialdevicefinder.h"

#include "wheel.h"
#include "sequentialcommandexecutor.h"
#include "wheelsendmessage.h"
#include "chassisserviceimpl.h"

int main()
{
    constexpr int wheelCount {2};
    int exit_code {0};
    boost::asio::io_context io_ctx;
    boost::asio::signal_set sig{io_ctx, SIGINT, SIGTERM};
    sig.async_wait([&io_ctx, &exit_code](const auto &err, int sig_num)
    {
        if (!err)
        {
            BOOST_LOG_TRIVIAL(error) << "Error: " << sig_num << std::endl;
            io_ctx.stop();
            exit_code = sig_num;
        }
    });

    serial::SerialDeviceFinder finder;
    auto mapped_serials = finder.map();
    for (auto &serial : mapped_serials)
    {
        BOOST_LOG_TRIVIAL(info) << "Detected ID=" << serial.first
                                << " @ port=" << serial.second.getPortName();
    }

    if (mapped_serials.size() < wheelCount)
    {
        BOOST_LOG_TRIVIAL(error) << "Some serials have not been detected. Quit.";
        exit(-1);
    }

    // Create data sinks. A data sink is any client that accepts data
    // incoming from a chassis model.
    using Serial = serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>;
//    using Wheel = model::Wheel<Serial>;
    auto lserial = std::make_shared<Serial>(io_ctx, mapped_serials["0"], serial::PrintContent{});
    auto rserial = std::make_shared<Serial>(io_ctx, mapped_serials["1"], serial::PrintContent{});

//    ChassisServiceImpl service;
//    grpc::ServerBuilder builder;
//    builder.AddListeningPort("0.0.0.0:5000", grpc::InsecureServerCredentials())
//            .RegisterService(&service);
//    auto server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());
//    server->Wait();

    io_ctx.run();
    return exit_code;
}
