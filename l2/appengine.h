#ifndef APPENGINE_H
#define APPENGINE_H

#include <QPointer>
#include <QGamepad>
#include <QObject>
#include <QTimer>

#include <grpcpp/grpcpp.h>
#include "grpcclient.h"


using grpc::Channel;
class AppEngine : public QObject
{
    Q_OBJECT
    static constexpr int GAMEPAD_TIMER_INTERVAL_MS = 50;
    static constexpr int MIN_MULTIPLIER { -1 };
    static constexpr int REVERT_AXIS { true ? -1 : 1 };
    static constexpr double DEAD_ZONE_PERCENT { 0.10 };
public:
    explicit AppEngine(std::shared_ptr<Channel> channel, QObject *parent = nullptr);
    ~AppEngine() override = default;

signals:
    void quit();


private slots:
    void onGamepadTimeout();

    void onServerReplyStatusReceived(const grpc::Status &status);


private:
    double process_input(double input) const;
    double apply_dead_zone(double input) const;

    std::unique_ptr<GrpcClient> grpc;
    std::unique_ptr<QTimer> gamepadTimer;
    QPointer<QGamepad> gamepad;

    int inputMultiplier { 255 };
    double leftAxis { 0.0 };
    double rightAxis { 0.0 };
};

#endif // APPENGINE_H
