#include "chassismodel.h"

#include <boost/asio/io_context.hpp>
#include <algorithm>
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
                    std::bind(
                        &ChassisModel::on_feedback_received,
                        this, std::placeholders::_1));

        wheels[port_number] = std::make_shared<Wheel>(
                    port_number,
                    serials[port_number]);
        chassis->addWheel(wheels[port_number]);
        dispatcher.addObserver(
                    static_cast<int>(port_number),
                    std::bind(&ChassisModel::set_wheel_feedback, this,
                              std::placeholders::_1, std::placeholders::_2));
    }

    executor->setNotifier([&](std::vector<WheelSendMessage> &&message)
    {
        on_message_received(std::move(message));
    });
    visitor = std::make_shared<rpc::GrpcChassisController>(WHEEL_COUNT,
                                                           executor);
    }

    bool ChassisModel::checkSerialHealth() const
    {
        return std::all_of(std::cbegin(serials), std::cend(serials),
                           [](std::shared_ptr<Serial> serial) -> bool
        {
            return *serial;
        });
    }

void ChassisModel::on_message_received(std::vector<WheelSendMessage> &&message)
{
    chassis->notify(std::move(message));
}

void ChassisModel::on_feedback_received(std::string feedback)
{
    BOOST_LOG_TRIVIAL(trace) << feedback;
    try
    {
        dispatcher.dispatch(feedback);
        dispatcher.notify();
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << "Could not dispatch telemetry due to: "
                                 << e.what();
    }
}

void ChassisModel::set_wheel_feedback(int id, boost::property_tree::ptree tree)
{
    chassis->processFeedback(Wheel::feedback_type::fromTree(id, tree));
}
