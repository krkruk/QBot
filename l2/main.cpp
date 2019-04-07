#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <memory>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "service.grpc.pb.h"
#include "service.pb.h"

using grpc::Channel;
using grpc::ClientContext;

class Client {
    std::unique_ptr<rpc::svc::ChassisService::Stub> stub;
public:
    Client(std::shared_ptr<Channel> channel)
        : stub(rpc::svc::ChassisService::NewStub(channel))
    {}

    void sendPwm(int lpwm, int rpwm)
    {
        rpc::svc::PwmDriveCommand pwm;
        pwm.set_lpwm(lpwm);
        pwm.set_rpwm(rpwm);

        rpc::svc::CommandResult response;
        ClientContext ctx;
        grpc::Status status = stub->drivePwm(&ctx, pwm, &response);

        if (status.ok())
        {
            std::cout << "Result: " << response.result() << ": " << response.description() << std::endl;
        }
        else
        {
          std::cerr << status.error_code() << ": " << status.error_message()
                    << std::endl;

        }
    }
};
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();

//    Client client(grpc::CreateChannel(
//      "localhost:5000", grpc::InsecureChannelCredentials()));
//    client.sendPwm(255, 255);
//    return 0;
}
