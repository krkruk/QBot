#ifndef GRPCCHASSISCONTROLLER_H
#define GRPCCHASSISCONTROLLER_H
#include <memory>

#include "sequentialcommandexecutor.h"
#include "grpcchassisvisitor.h"

namespace rpc
{

/**
 * @brief The GrpcChassisController class is an implementation of
 * {@class GrpcChassisVisitor}. Please, see the parent class for
 * more details.
 */
class GrpcChassisController : public GrpcChassisVisitor
{
    const unsigned int wheelCount;
//    std::weak_ptr<>
    std::weak_ptr<SequentialCommandExecutor> executor;
public:
    /**
     * @brief GrpcChassisController Creates a controller that handles parsing
     * incoming messages
     * @param wheelCount It answers a question: how many wheels does the robot have?
     * @param executor An executor that is responsible for running generated commands.
     */
    GrpcChassisController(unsigned int wheelCount, std::weak_ptr<SequentialCommandExecutor> executor);

//    template<typename Chassis>
//    GrpcChassisController(Chassis &chassis, std::weak_ptr<SequentialCommandExecutor> executor)
//    {

//    }

    ~GrpcChassisController() override = default;

    void accept(const svc::PwmDriveCommand &cmd) override;
    void accept(const svc::DifferentialDriveCommand &cmd) override;
    void accept(const svc::PeripheralDeviceCommand &cmd) override;
};

}
#endif // GRPCCHASSISCONTROLLER_H
