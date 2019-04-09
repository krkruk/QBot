#include "chassisserviceimpl.h"
#include "grpcchassisvisitor.h"


namespace
{
    template<typename T>
    inline void unused(const T &var)
    {
        (void) var;
    }
}

ChassisServiceImpl::ChassisServiceImpl(
        std::weak_ptr<rpc::GrpcChassisVisitor> visitor,
        std::function<void()> onResolvedAction,
        std::function<void(rpc::svc::AllWheelFeedback *)> fillTelemetry)
    : rpc::svc::ChassisService::Service{},
      visitor{visitor},
      notify{onResolvedAction},
      fillTelemetry{fillTelemetry}
{
}

grpc::Status ChassisServiceImpl::drivePwm(
        grpc::ServerContext *context,
        const rpc::svc::PwmDriveCommand *request,
        rpc::svc::CommandResult *response)
{
    unused(context);

    return resolve(response, *request);
}

grpc::Status ChassisServiceImpl::driveDifferential(
        grpc::ServerContext *context,
        const rpc::svc::DifferentialDriveCommand *request,
        rpc::svc::CommandResult *response)
{
    unused(context);

    return resolve(response, *request);
}

grpc::Status ChassisServiceImpl::startPeripheralDevice(
        grpc::ServerContext *context,
        const rpc::svc::PeripheralDeviceCommand *request,
        rpc::svc::PeripheralDeviceCommand *response)
{
    unused(request);
    unused(context);
    unused(response);

    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                        "Mismatched status code");
}

grpc::Status ChassisServiceImpl::getWheelFeedback(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        rpc::svc::AllWheelFeedback *response)
{
    unused(request);
    unused(context);

    if (fillTelemetry)
    {
        fillTelemetry(response);
        return grpc::Status::OK;
    }
    else
    {
        return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                            "Mismatched status code");
    }
}

template<typename Message>
grpc::Status ChassisServiceImpl::resolve(
        rpc::svc::CommandResult *response, const Message &message)
{
    if (auto sh_visitor = visitor.lock())
    {
        sh_visitor->accept(message);
        response->set_result(rpc::svc::CommandResult::OK);
        notify();
        return grpc::Status::OK;
    }
    else
    {
        response->set_result(rpc::svc::CommandResult::FAILURE);
        return grpc::Status(grpc::StatusCode::INTERNAL,
                            "Cannot access serial devices.");
    }
}

