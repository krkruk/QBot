#include "appengine.h"
#include <QGuiApplication>
#include <QGamepadManager>
#include <QDebug>
#include <QList>
#include <cmath>


AppEngine::AppEngine(std::shared_ptr<grpc::Channel> channel, QObject *parent)
    : QObject(parent),
      grpc{std::make_unique<GrpcClient>(std::move(channel))},
      gamepadTimer(std::make_unique<QTimer>())
{
    gamepadTimer->setInterval(GAMEPAD_TIMER_INTERVAL_MS);
    connect(gamepadTimer.get(), &QTimer::timeout,
            this, &AppEngine::onGamepadTimeout);
    gamepadTimer->start();

    const auto connectedGamepads = QGamepadManager::instance()->connectedGamepads();

    if (!connectedGamepads.size())
    {
        throw std::runtime_error("Gamepads not detected. Close the app.");
    }

    gamepad = new QGamepad(connectedGamepads.first(), this);
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

void AppEngine::onServerReplyStatusReceived(const grpc::Status &status)
{
}

double AppEngine::process_input(double input) const
{
    return REVERT_AXIS * inputMultiplier * apply_dead_zone(input);
}

double AppEngine::apply_dead_zone(double input) const
{
    return std::abs(input) > DEAD_ZONE_PERCENT ? input : 0.0;
}
