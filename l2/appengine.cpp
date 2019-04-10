#include "appengine.h"
#include <google/protobuf/util/json_util.h>
#include <QGuiApplication>
#include <QGamepadManager>
#include <QDebug>
#include <QList>
#include <cmath>


AppEngine::AppEngine(std::shared_ptr<grpc::Channel> channel, QObject *parent)
    : QObject(parent),
      grpc{std::make_unique<GrpcClient>(std::move(channel))},
      gamepadTimer(std::make_unique<QTimer>()),
      telemetryTimer(std::make_unique<QTimer>())
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
