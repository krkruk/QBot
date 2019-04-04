#include "grpcchassiscontroller.h"
#include "wheelsendmessage.h"
#include "pwmdrivecommand.h"
#include "service.pb.h"

#include <exception>
#include <vector>


rpc::GrpcChassisController::GrpcChassisController(unsigned int wheelCount, std::weak_ptr<SequentialCommandExecutor> executor)
    : wheelCount{wheelCount}, executor{executor}
{

}

void rpc::GrpcChassisController::accept(const rpc::svc::PwmDriveCommand &cmd)
{
    if (auto sh_executor = executor.lock())
    {
        auto command = std::make_unique<PwmDriveCommand>(wheelCount);
        command->setLeftValue(cmd.lpwm());
        command->setRightValue(cmd.rpwm());
        sh_executor->addCommand(std::move(command));
    }
    else
    {
        throw std::runtime_error("Cannot access the SequentialCommandExecutor");
    }

}

void rpc::GrpcChassisController::accept(const svc::DifferentialDriveCommand &cmd)
{
    (void) cmd;
    throw std::runtime_error("Not implemented");
}

void rpc::GrpcChassisController::accept(const svc::PeripheralDeviceCommand &cmd)
{
    (void) cmd;
    throw std::runtime_error("Not implemented");
}
