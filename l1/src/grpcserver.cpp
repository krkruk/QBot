#include "grpcserver.h"

#include "sequentialcommandexecutor.h"
#include "chassisserviceimpl.h"
#include "chassismodel.h"

#include <boost/log/trivial.hpp>
#include <grpc++/grpc++.h>


GrpcServer::GrpcServer(std::weak_ptr<ChassisModel> model)
    : model{model}
{
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
    auto sh_model = model.lock();

    if (!sh_model)
    {
        throw std::runtime_error("Cannot access chassis serial-side model.");
    }

    ChassisServiceImpl service(
    sh_model->getVisitor(),
    [x=sh_model->getExecutor()]()
    {
        if (auto sh_executor = x.lock())
        {
            sh_executor->exec();
        }
    },
    [wheels=sh_model->getWheels()](rpc::svc::AllWheelFeedback *response)
    {
        for (const auto &wheel : wheels)
        {
            if (wheel)
            {
                auto *wheelResponse = response->add_wheels();
                wheelResponse->set_id(wheel->getId());
                wheelResponse->set_milliamps(wheel->state()->getMilliamps());
                wheelResponse->set_pwm(wheel->state()->getPwm());
                wheelResponse->set_error_code(wheel->state()->getErrorCode());
                wheelResponse->set_angular_velocity(wheel->state()->getAngularVelocity());
                wheelResponse->set_celsius(wheel->state()->getCelsius());
            }
        }
    });

    grpc::ServerBuilder builder;
    builder.AddListeningPort(
                "0.0.0.0:5000",
                grpc::InsecureServerCredentials())
            .RegisterService(&service);
    server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());

    BOOST_LOG_TRIVIAL(info) << "The grpc server has been started.";
    server->Wait();
}
