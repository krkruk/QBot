#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

#include "serialdevicefinder.h"
#include "serialportinfo.h"
#include "serialport.h"

#include "sequentialcommandexecutor.h"
#include "grpcchassiscontroller.h"
#include "chassisserviceimpl.h"
#include "wheelsendmessage.h"
#include "drivecommand.h"
#include "chassis.h"
#include "wheel.h"

#include "grpcserver.h"

namespace
{
    using Serial = serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>;
    using Wheel = model::Wheel<Serial>;
    constexpr int wheelCount {2};
}

auto detect_serials()
{
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
            BOOST_LOG_TRIVIAL(error) << "Error: " << sig_num << std::endl;
            io_ctx.stop();
            exit_code = sig_num;
        }
    });

    auto mapped_serials = detect_serials();
    auto lserial = std::make_shared<Serial>(io_ctx, mapped_serials["0"], serial::PrintContent{});
    auto rserial = std::make_shared<Serial>(io_ctx, mapped_serials["1"], serial::PrintContent{});

    auto leftWheel = std::make_shared<Wheel>(0, lserial);
    auto rightWheel = std::make_shared<Wheel>(1, rserial);

    auto chassis = std::make_unique<model::Chassis<Wheel, wheelCount>>();
    chassis->addWheel(leftWheel);
    chassis->addWheel(rightWheel);

    auto executor = std::make_shared<SequentialCommandExecutor>();
    executor->setNotifier([&chassis](std::vector<WheelSendMessage> &&message)
    {
        chassis->notify(std::move(message));
    });

    auto visitor = std::make_shared<rpc::GrpcChassisController>(wheelCount, executor);
    GrpcServer grpc_server(executor, visitor);
    io_ctx.run();
    return exit_code;
}
