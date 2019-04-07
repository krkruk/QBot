#ifndef CHASSISMODEL_H
#define CHASSISMODEL_H
#include <unordered_map>
#include <vector>
#include <memory>

#include "sequentialcommandexecutor.h"
#include "grpcchassiscontroller.h"
#include "wheelsendmessage.h"
#include "serialportinfo.h"
#include "drivecommand.h"
#include "serialport.h"
#include "chassis.h"
#include "wheel.h"

class ChassisModel
{
    using Serial = serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>;
    using Wheel = model::Wheel<Serial>;

    static constexpr unsigned WHEEL_COUNT {2};
public:

    explicit ChassisModel(
            boost::asio::io_context &io_ctx,
            const std::unordered_map<std::string, serial::PortInfo> &port_mapping);
    ~ChassisModel() = default;

    std::weak_ptr<SequentialCommandExecutor> getExecutor()
    {
        return executor;
    }

    std::weak_ptr<rpc::GrpcChassisVisitor> getVisitor()
    {
        return visitor;
    }

private:
    std::unique_ptr<model::Chassis<Wheel, WHEEL_COUNT>> chassis;
    std::shared_ptr<SequentialCommandExecutor> executor;
    std::shared_ptr<rpc::GrpcChassisVisitor> visitor;
    std::vector<std::shared_ptr<Serial>> serials;
    std::vector<std::shared_ptr<Wheel>> wheels;

    void on_message_receive(std::vector<WheelSendMessage> &&message);
};

#endif // CHASSISMODEL_H
