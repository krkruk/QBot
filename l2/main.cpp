#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>

#include "grpcclient.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    GrpcClient c(grpc::CreateChannel("localhost:5000", grpc::InsecureChannelCredentials()));
    QTimer::singleShot(5000, &app, [&c]()
    {
        c.sendPwm(255, 255, [](const grpc::Status &s)
        {
            std::cout << "Status: " << s.ok() << std::endl;
        });
    });
    return app.exec();
}
