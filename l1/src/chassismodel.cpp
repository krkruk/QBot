#include "chassismodel.h"
#include <boost/asio/io_context.hpp>
#include "grpcchassiscontroller.h"

constexpr unsigned ChassisModel::WHEEL_COUNT;


ChassisModel::ChassisModel(
        boost::asio::io_context &io_ctx,
        const std::unordered_map<std::string, serial::PortInfo> &port_mapping)
    : chassis{std::make_unique<model::Chassis<Wheel, WHEEL_COUNT>>()},
      executor{std::make_shared<SequentialCommandExecutor>()},
      serials(WHEEL_COUNT),
      wheels(WHEEL_COUNT)
{
    for (unsigned port_number{0}; port_number < WHEEL_COUNT; ++port_number)
    {
        serials[port_number] = std::make_shared<Serial>(
                    io_ctx,
                    port_mapping.at(std::to_string(port_number)),
                    serial::PrintContent{});

        wheels[port_number] = std::make_shared<Wheel>(
                    port_number,
                    serials[port_number]);
        chassis->addWheel(wheels[port_number]);
    }

    executor->setNotifier([&](std::vector<WheelSendMessage> &&message)
    {
        on_message_receive(std::move(message));
    });
    visitor = std::make_shared<rpc::GrpcChassisController>(WHEEL_COUNT,
                                                           executor);
}

void ChassisModel::on_message_receive(std::vector<WheelSendMessage> &&message)
{
    chassis->notify(std::move(message));
}
