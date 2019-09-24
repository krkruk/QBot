#include "singleucchassismodel.h"

#include <boost/asio/io_service.hpp>
#include <algorithm>
#include "grpcchassiscontroller.h"

constexpr unsigned SingleUcChassisModel::WHEEL_COUNT;


SingleUcChassisModel::SingleUcChassisModel(
        boost::asio::io_service &io_ctx,
        const std::unordered_map<std::string, serial::PortInfo> &port_mapping)
    : chassis{std::make_unique<model::Chassis<Wheel, WHEEL_COUNT>>()},
      executor{std::make_shared<SequentialCommandExecutor>()},
      sink{std::make_shared<JsonSink>()},
      wheels(WHEEL_COUNT)
{
    BOOST_LOG_TRIVIAL(info) << "Use a single uC mode.";
    if (port_mapping.size() < 1)
    {
        throw std::invalid_argument("Not enough serial ports detected");
    }
    serial = std::make_shared<Serial>(
                    io_ctx,
                    port_mapping.cbegin()->second,
                    std::bind(
                        &SingleUcChassisModel::on_feedback_received,
                        this, std::placeholders::_1));
    for (unsigned port_number{0}; port_number < WHEEL_COUNT; ++port_number)
    {
        wheels[port_number] = std::make_shared<Wheel>(
                    port_number,
                    sink);
        chassis->addWheel(wheels[port_number]);
        dispatcher.addObserver(
                    static_cast<int>(port_number),
                    std::bind(&SingleUcChassisModel::set_wheel_feedback, this,
                              std::placeholders::_1, std::placeholders::_2));
    }

    executor->setNotifier([&](std::vector<WheelSendMessage> &&message)
    {
        on_message_received(std::move(message));
    });
    visitor = std::make_shared<rpc::GrpcChassisController>(WHEEL_COUNT,
                                                           executor);
}

bool SingleUcChassisModel::checkSerialHealth() const
{
    return *serial;
}

void SingleUcChassisModel::on_message_received(std::vector<WheelSendMessage> &&message)
{
    chassis->notify(std::move(message));
    // This is a must because each wheel receives a command and sends a correct value
    // into the JsonSink. There, JsonSink has all data that must be sent into a single
    // microcontroller. Of course, this action may be deferred in time but it must
    // be done otherwise no command will be issued to the microcontroller.
    serial->write(sink->toString());
}

void SingleUcChassisModel::on_feedback_received(std::string feedback)
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

void SingleUcChassisModel::set_wheel_feedback(int id, boost::property_tree::ptree tree)
{
    chassis->processFeedback(Wheel::feedback_type::fromTree(id, tree));
}
