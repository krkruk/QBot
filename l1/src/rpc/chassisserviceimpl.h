#ifndef CHASSISSERVICEIMPL_H
#define CHASSISSERVICEIMPL_H
#include <grpcpp/grpcpp.h>
#include <functional>
#include <utility>

#include "service.grpc.pb.h"
#include "service.pb.h"

namespace rpc
{
class GrpcChassisVisitor;
}

class ChassisServiceImpl : public rpc::svc::ChassisService::Service
{
    std::weak_ptr<rpc::GrpcChassisVisitor> visitor;
    std::function<void()> notify;
public:
    explicit ChassisServiceImpl(std::weak_ptr<rpc::GrpcChassisVisitor> visitor,
                                std::function<void()> onResolvedAction);

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

    grpc::Status getWheelFeedback(grpc::ServerContext *context,
                                  const google::protobuf::Empty *request,
                                  rpc::svc::AllWheelFeedback *response) override;

private:
    template<typename Message>
    grpc::Status resolve(rpc::svc::CommandResult *response, const Message &message);
};


#endif // CHASSISSERVICEIMPL_H
