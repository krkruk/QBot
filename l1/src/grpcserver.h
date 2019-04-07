#ifndef GRPCSERVER_H
#define GRPCSERVER_H
#include <memory>
#include <thread>
#include <mutex>
#include "chassisserviceimpl.h"


class SequentialCommandExecutor;
namespace rpc
{
class GrpcChassisVisitor;
}

class GrpcServer
{
    std::weak_ptr<SequentialCommandExecutor> executor;
    std::weak_ptr<rpc::GrpcChassisVisitor> visitor;
    std::unique_ptr<ChassisServiceImpl> service;
    std::unique_ptr<grpc::Server> server;
    std::thread server_thread;
public:
    explicit GrpcServer(std::weak_ptr<SequentialCommandExecutor> executor,
                std::weak_ptr<rpc::GrpcChassisVisitor> visitor);
    ~GrpcServer();

    void kill();

private:
    void run();
};

#endif // GRPCSERVER_H
