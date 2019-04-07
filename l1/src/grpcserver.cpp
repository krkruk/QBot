#include "grpcserver.h"

#include <boost/log/trivial.hpp>
#include <grpc++/grpc++.h>
#include <thread>

#include "sequentialcommandexecutor.h"
#include "chassisserviceimpl.h"

GrpcServer::GrpcServer(
        std::weak_ptr<SequentialCommandExecutor> executor,
        std::weak_ptr<rpc::GrpcChassisVisitor> visitor)
    : executor{executor}, visitor{visitor}
{
    service = std::make_unique<ChassisServiceImpl>(visitor, [&executor]()
    {
        if (auto sh_executor = executor.lock())
        {
            sh_executor->exec();
        }
    });
    server_thread = std::thread{&GrpcServer::run, this};
}

GrpcServer::~GrpcServer()
{
    if (server_thread.joinable())
    {
        kill();
        server_thread.join();
        BOOST_LOG_TRIVIAL(info) << "The server thread has been joined.";
    }
}

void GrpcServer::kill()
{
    std::unique_lock<std::mutex> _;
    if (server)
    {
        BOOST_LOG_TRIVIAL(info) << "About to shutdown the grpc server.";
        server->Shutdown();
        BOOST_LOG_TRIVIAL(info) << "The grpc server has been shut down.";
        server.reset();
    }
}

void GrpcServer::run()
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:5000",
                             grpc::InsecureServerCredentials())
            .RegisterService(service.get());
    server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());

    BOOST_LOG_TRIVIAL(info) << "The grpc server has been started.";
    std::unique_lock<std::mutex> _;
    server->Wait();
}
