#ifndef CHASSISSERVICEIMPL_H
#define CHASSISSERVICEIMPL_H
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"
#include "service.pb.h"


class ChassisServiceImpl : public rpc::svc::ChassisService::Service
{
public:
    ~ChassisServiceImpl() override = default;

    grpc::Status drivePwm(grpc::ServerContext *context,
                          const rpc::svc::PwmDriveCommand *request,
                          rpc::svc::CommandResult *response) override;

    grpc::Status driveDifferential(grpc::ServerContext *context,
                                   const rpc::svc::DifferentialDriveCommand *request,
                                   rpc::svc::CommandResult *response) override;

    grpc::Status startPeripheralDevice(grpc::ServerContext *context,
                                       const rpc::svc::PeripheralDeviceCommand *request,
                                       rpc::svc::PeripheralDeviceCommand *response) override;
};


#endif // CHASSISSERVICEIMPL_H
