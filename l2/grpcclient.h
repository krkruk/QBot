#ifndef GRPCCLIENT_H
#define GRPCCLIENT_H

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <grpcpp/grpcpp.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>

#include "service.grpc.pb.h"
#include "service.pb.h"

using grpc::Channel;
using grpc::ClientContext;

class GrpcClient {
    static constexpr int THREADS {2};
    std::shared_ptr<rpc::svc::ChassisService::Stub> stub;
    boost::asio::thread_pool th_pool;
    std::mutex mtx;

public:
    GrpcClient(std::shared_ptr<Channel> channel)
        : stub(rpc::svc::ChassisService::NewStub(channel)),
          th_pool{THREADS}
    {}

    ~GrpcClient()
    {
        th_pool.join();
    }

    void sendPwm(int lpwm, int rpwm, std::function<void(const grpc::Status &)> callback)
    {
        rpc::svc::PwmDriveCommand pwm;
        pwm.set_lpwm(lpwm);
        pwm.set_rpwm(rpwm);

        boost::asio::post(th_pool, [this, cmd=std::move(pwm), callback]()
        {
           this->send_pwm(cmd, callback);
        });
    }

private:
    void send_pwm(const rpc::svc::PwmDriveCommand &pwm, std::function<void(const grpc::Status &)> callback)
    {
        rpc::svc::CommandResult response;
        ClientContext ctx;
        grpc::Status status;
        {
            std::lock_guard<std::mutex> _(mtx);
            stub->drivePwm(&ctx, pwm, &response);
        }
        callback(status);
    }

};
#endif // GRPCCLIENT_H
