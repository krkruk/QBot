#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <vector>
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

#include <google/protobuf/util/json_util.h>
#include "service.pb.h"


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

    using Serial = serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>;
    using Wheel = model::Wheel<Serial>;
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

    auto visitor = std::make_unique<rpc::GrpcChassisController>(wheelCount, executor);

    // This is just to show it works
    boost::asio::steady_timer timer{io_ctx, boost::asio::chrono::seconds{5}};
    timer.async_wait([&visitor, &executor](const auto &)
    {
        rpc::svc::PwmDriveCommand pwm;
        pwm.set_lpwm(255);
        pwm.set_rpwm(255);

        std::string buff;
        google::protobuf::util::MessageToJsonString(pwm, &buff);
        BOOST_LOG_TRIVIAL(info) << "Sending a message: " << buff;
        visitor->accept(pwm);
        executor->exec();
    });
    boost::asio::steady_timer timer_restore{io_ctx, boost::asio::chrono::seconds{10}};
    timer_restore.async_wait([&visitor, &executor](const auto &)
    {
        rpc::svc::PwmDriveCommand pwm;
        pwm.set_lpwm(0);
        pwm.set_rpwm(0);

        std::string buff;
        google::protobuf::util::MessageToJsonString(pwm, &buff);
        BOOST_LOG_TRIVIAL(info) << "Sending a message: " << buff;
        visitor->accept(pwm);
        executor->exec();
    });

//    ChassisServiceImpl service;
//    grpc::ServerBuilder builder;
//    builder.AddListeningPort("0.0.0.0:5000", grpc::InsecureServerCredentials())
//            .RegisterService(&service);
//    auto server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());
//    server->Wait();

    io_ctx.run();
    return exit_code;
}
