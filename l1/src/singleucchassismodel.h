#ifndef SINGLEUCCHASSISMODEL_H
#define SINGLEUCCHASSISMODEL_H
#include <unordered_map>
#include <vector>
#include <memory>

#include "sequentialcommandexecutor.h"
#include "jsondispatcherprocessor.h"
#include "grpcchassiscontroller.h"
#include "wheelsendmessage.h"
#include "serialportinfo.h"
#include "chassismodel.h"
#include "drivecommand.h"
#include "dispatcher.h"
#include "serialport.h"
#include "jsonsink.h"
#include "chassis.h"
#include "wheel.h"

/**
 * The class is responsible for combining all components to create a chassis model.
 *
 * The class creates a series of serial ports handles, Wheel models and connects
 * it with an executor.
 * @brief The ChassisModel class combines all components into a single chassis model.
 */
class SingleUcChassisModel : public ChassisModel
{
    using Serial = serial::SerialPort<
        serial::ReadLineAlgorithm<
            std::function<void(std::string)>
        >
    >;
    using Wheel = model::Wheel<JsonSink>;
    using ChassisDispatcher = MappedDispatcher<
        std::string, JsonDispatcherProcessor<int, boost::property_tree::ptree>
    >;
    static constexpr unsigned WHEEL_COUNT {4};
public:

    /**
     * @brief ChassisModel Constructor of the Chassis model
     * @param io_ctx boost io_service
     * @param port_mapping a map of (ID, PortInfo) pairs.
     */
    explicit SingleUcChassisModel(
            boost::asio::io_service &io_ctx,
            const std::unordered_map<std::string, serial::PortInfo> &port_mapping);
    ~SingleUcChassisModel() override {}

    /**
     * @brief getExecutor Returns executor
     * @return Executor
     */
    std::weak_ptr<SequentialCommandExecutor> getExecutor() override
    {
        return executor;
    }

    /**
     * @brief getVisitor Returns visitor
     * @return Visitor
     */
    std::weak_ptr<rpc::GrpcChassisVisitor> getVisitor() override
    {
        return visitor;
    }

    /**
     * @brief getWheels
     * @return
     */
    const std::vector<std::shared_ptr<Wheel>> &getWheels() const
    {
        return wheels;
    }

    /**
     * @brief checkSerialHealth Checks if all serial are OK.
     * @return False if one of the serials is not OK.
     */
    bool checkSerialHealth() const override;

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
    std::shared_ptr<JsonSink> sink;
    std::shared_ptr<Serial> serial;
    std::vector<std::shared_ptr<Wheel>> wheels;
    ChassisDispatcher dispatcher;

    void on_message_received(std::vector<WheelSendMessage> &&message);
    void on_feedback_received(std::string feedback);
    void set_wheel_feedback(int id, boost::property_tree::ptree tree);
};

#endif // SINGLEUCCHASSISMODEL_H
