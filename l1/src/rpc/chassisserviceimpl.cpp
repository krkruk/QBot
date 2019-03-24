#include "chassisserviceimpl.h"

namespace
{
    template<typename T>
    inline void unused(const T &var)
    {
        (void) var;
    }
}

grpc::Status ChassisServiceImpl::drivePwm(
        grpc::ServerContext *context,
        const rpc::svc::PwmDriveCommand *request,
        rpc::svc::CommandResult *response)
{
    unused(request);
    unused(context);
    unused(response);

    response->set_result(rpc::svc::CommandResult::OK);
    response->set_description("It works!");
    return grpc::Status::OK;
}

grpc::Status ChassisServiceImpl::driveDifferential(
        grpc::ServerContext *context,
        const rpc::svc::DifferentialDriveCommand *request,
        rpc::svc::CommandResult *response)
{
    unused(request);
    unused(context);
    unused(response);

    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                        "Mismatched status code");
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
