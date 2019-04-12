#include "appengine.h"
#include <google/protobuf/util/json_util.h>
#include <QGuiApplication>
#include <QGamepadManager>
#include <QStringList>
#include <QProcess>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <thread>
#include <cmath>


AppEngine::AppEngine(const QString &address, int port, QObject *parent)
    : QObject(parent),
      grpc{std::make_unique<GrpcClient>(
               grpc::CreateChannel(QString("%1:%2")
                                   .arg(address)
                                   .arg(port).toStdString(),
                      grpc::InsecureChannelCredentials()))},
      gamepadTimer(std::make_unique<QTimer>()),
      telemetryTimer(std::make_unique<QTimer>()),
      address{address},
      port{port}
{
    gamepadTimer->setInterval(GAMEPAD_TIMER_INTERVAL_MS);
    connect(gamepadTimer.get(), &QTimer::timeout,
            this, &AppEngine::onGamepadTimeout);
    gamepadTimer->start();

    telemetryTimer->setInterval(TELEMETRY_TIMER_INTERVAL_MS);
    connect(telemetryTimer.get(), &QTimer::timeout,
            this, &AppEngine::onTelemetryTimeout);
    telemetryTimer->start();

    const auto connectedGamepads = QGamepadManager::instance()->connectedGamepads();

    if (!connectedGamepads.size())
    {
        throw std::runtime_error("Gamepads not detected. Close the app.");
    }

    gamepad = new QGamepad(connectedGamepads.first(), this);
}

void AppEngine::setCameraEnabled(bool cameraEnabled)
{
    if (m_cameraEnabled == cameraEnabled)
    {
        return;
    }

    m_cameraEnabled = cameraEnabled;
    emit cameraEnabledChanged(m_cameraEnabled);
}

void AppEngine::enableCamera()
{
    grpc->enableCamera([this](const rpc::svc::PeripheralDeviceCommand &camera)
    {
        this->setCameraEnabled(
                    camera.status() == rpc::svc::PeripheralDeviceCommand::ENABLED);

        if (cameraEnabled())
        {
                std::this_thread::sleep_for(std::chrono::seconds{5});
                QProcess::startDetached(
                            "/bin/sh", QStringList() << "-c" <<
                            QString("nc %1 %2 | mplayer -fps 300 -demuxer h264es -")
                            .arg(address).arg(CAMERA_PORT));
        }
    });
}

void AppEngine::onGamepadTimeout()
{
    if (gamepad.isNull())
    {
        return;
    }

    leftAxis = gamepad->axisLeftY();
    rightAxis = gamepad->axisRightY();

    leftAxis = process_input(leftAxis);
    rightAxis = process_input(rightAxis);
    grpc->sendPwm(static_cast<int>(leftAxis),
                  static_cast<int>(rightAxis),
                  std::bind(&AppEngine::onServerReplyStatusReceived,
                            this, std::placeholders::_1));
}

void AppEngine::onTelemetryTimeout()
{
    grpc->getTelemetry([](const rpc::svc::AllWheelFeedback &msg)
    {
        std::string json;
        google::protobuf::util::MessageToJsonString(msg, &json);
        qDebug() << "Feedback received: " << json.c_str();
    });
}

void AppEngine::onServerReplyStatusReceived(const grpc::Status &status)
{
    Q_UNUSED(status)
}

double AppEngine::process_input(double input) const
{
    return REVERT_AXIS * inputMultiplier * apply_dead_zone(input);
}

double AppEngine::apply_dead_zone(double input) const
{
    return std::abs(input) > DEAD_ZONE_PERCENT ? input : 0.0;
}
