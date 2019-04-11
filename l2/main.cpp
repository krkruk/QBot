#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QQmlContext>
#include <QDebug>

#include "appengine.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    AppEngine apk(grpc::CreateChannel(
                      "192.168.11.7:5000",
                      grpc::InsecureChannelCredentials()));

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appEngine", &apk);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    return app.exec();
}
