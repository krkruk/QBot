#ifndef CHASSISMODEL_H
#define CHASSISMODEL_H
#include <unordered_map>
#include <vector>
#include <memory>

#include "sequentialcommandexecutor.h"
#include "jsondispatcherprocessor.h"
#include "grpcchassiscontroller.h"
#include "wheelsendmessage.h"
#include "serialportinfo.h"
#include "drivecommand.h"
#include "dispatcher.h"
#include "serialport.h"
#include "chassis.h"
#include "wheel.h"

/**
 * The class is responsible for combining all components to create a chassis model.
 *
 * The class creates a series of serial ports handles, Wheel models and connects
 * it with an executor.
 * @brief The ChassisModel class combines all components into a single chassis model.
 */
class ChassisModel
{
    using Serial = serial::SerialPort<
        serial::ReadLineAlgorithm<
            std::function<void(std::string)>
        >
    >;
    using Wheel = model::Wheel<Serial>;
    using ChassisDispatcher = MappedDispatcher<
        std::string, JsonDispatcherProcessor<int, boost::property_tree::ptree>
    >;
    static constexpr unsigned WHEEL_COUNT {2};
public:

    /**
     * @brief ChassisModel Constructor of the Chassis model
     * @param io_ctx boost io_context
     * @param port_mapping a map of (ID, PortInfo) pairs.
     */
    explicit ChassisModel(
            boost::asio::io_context &io_ctx,
            const std::unordered_map<std::string, serial::PortInfo> &port_mapping);
    ~ChassisModel() = default;

    /**
     * @brief getExecutor Returns executor
     * @return Executor
     */
    std::weak_ptr<SequentialCommandExecutor> getExecutor()
    {
        return executor;
    }

    /**
     * @brief getVisitor Returns visitor
     * @return Visitor
     */
    std::weak_ptr<rpc::GrpcChassisVisitor> getVisitor()
    {
        return visitor;
    }

    /**
     * @brief checkSerialHealth Checks if all serial are OK.
     * @return False if one of the serials is not OK.
     */
    bool checkSerialHealth() const;

    /**
     * @brief wheelCount Return a number of wheels defined by the model
     * @return number of wheels
     */
    static constexpr unsigned wheelCount()
    {
        return WHEEL_COUNT;
    }

private:
    std::unique_ptr<model::Chassis<Wheel, WHEEL_COUNT>> chassis;
    std::shared_ptr<SequentialCommandExecutor> executor;
    std::shared_ptr<rpc::GrpcChassisVisitor> visitor;
    std::vector<std::shared_ptr<Serial>> serials;
    std::vector<std::shared_ptr<Wheel>> wheels;
    ChassisDispatcher dispatcher;

    void on_message_received(std::vector<WheelSendMessage> &&message);
    void on_feedback_received(std::string feedback);
    void set_wheel_feedback(int id, boost::property_tree::ptree tree);
};

#endif // CHASSISMODEL_H
