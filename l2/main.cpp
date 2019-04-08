#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QDebug>

#include "appengine.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    AppEngine apk(grpc::CreateChannel(
                      "localhost:5000",
                      grpc::InsecureChannelCredentials()));

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    return app.exec();
}
