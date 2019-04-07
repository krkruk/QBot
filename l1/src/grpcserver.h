#ifndef GRPCSERVER_H
#define GRPCSERVER_H
#include <memory>
#include <thread>


class SequentialCommandExecutor;
namespace rpc
{
class GrpcChassisVisitor;
}

namespace grpc
{
class Server;
}

/**
 * @brief The GrpcServer class starts a gRPC server and listens all
 * incoming connection at port 5000.
 */
class GrpcServer
{
    std::weak_ptr<SequentialCommandExecutor> executor;
    std::weak_ptr<rpc::GrpcChassisVisitor> visitor;
    std::unique_ptr<grpc::Server> server;
    std::thread server_thread;
public:
    /**
     * @brief GrpcServer Starts an active object of gRPC server
     * @param executor a reference to Executor
     * @param visitor a reference to Visitor
     */
    explicit GrpcServer(std::weak_ptr<SequentialCommandExecutor> executor,
                std::weak_ptr<rpc::GrpcChassisVisitor> visitor);
    ~GrpcServer();

    /**
     * @brief kill Stop gRPC server
     */
    void kill();


private:
    void run();
};

#endif // GRPCSERVER_H
