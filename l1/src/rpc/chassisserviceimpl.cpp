#include "chassisserviceimpl.h"
#include "grpcchassisvisitor.h"
#include <boost/log/trivial.hpp>


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
      rpiCam{nullptr},
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
    using namespace rpc::svc;
    unused(request);
    unused(context);
    unused(response);
    switch (request->device())
    {
    case rpc::svc::PeripheralDeviceCommand_Device_CAMERA_STREAM:
        if (!rpiCam || !rpiCam->running())
        {
            rpiCam = std::make_unique<boost::process::child>(
                        "/usr/bin/env", "sh", "/opt/launch_rpicam_stream.sh");
            BOOST_LOG_TRIVIAL(info) << "Camera has been launched. Status: "
                                    << rpiCam->running();
            response->set_device(PeripheralDeviceCommand::CAMERA_STREAM);
            response->set_status(rpiCam->running()
                                 ? PeripheralDeviceCommand::ENABLED
                                 : PeripheralDeviceCommand::DISABLED);
        }
        else
        {
            rpiCam.reset();
            BOOST_LOG_TRIVIAL(info) << "Camera has been killed.";
            response->set_device(PeripheralDeviceCommand::CAMERA_STREAM);
            response->set_status(PeripheralDeviceCommand::DISABLED);
        }
        break;

    case rpc::svc::PeripheralDeviceCommand_Device_UNKNOWN:
    case rpc::svc::PeripheralDeviceCommand_Device_FRONT_LED:
    case rpc::svc::PeripheralDeviceCommand_Device_REVERSE_LED:
    default:
        return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                            "Mismatched status code");
    }
    return grpc::Status::OK;
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

