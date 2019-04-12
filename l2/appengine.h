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
    Q_PROPERTY(bool cameraEnabled READ cameraEnabled WRITE setCameraEnabled NOTIFY cameraEnabledChanged)

    static constexpr int GAMEPAD_TIMER_INTERVAL_MS = 50;
    static constexpr int TELEMETRY_TIMER_INTERVAL_MS = 1000;
    static constexpr int MIN_MULTIPLIER { -1 };
    static constexpr int REVERT_AXIS { true ? -1 : 1 };
    static constexpr double DEAD_ZONE_PERCENT { 0.10 };

    static constexpr int CAMERA_PORT {6000};
public:
    explicit AppEngine(const QString &address, int port, QObject *parent = nullptr);
    ~AppEngine() override = default;

    bool cameraEnabled() const
    {
        return m_cameraEnabled;
    }


signals:
    void cameraEnabledChanged(bool cameraEnabled);


public slots:
    void setCameraEnabled(bool cameraEnabled);
    void enableCamera();


private slots:
    void onGamepadTimeout();
    void onTelemetryTimeout();
    void onServerReplyStatusReceived(const grpc::Status &status);


private:
    double process_input(double input) const;
    double apply_dead_zone(double input) const;

    std::unique_ptr<GrpcClient> grpc;
    std::unique_ptr<QTimer> gamepadTimer;
    std::unique_ptr<QTimer> telemetryTimer;
    QPointer<QGamepad> gamepad;
    QString address;
    int port {5000};

    int inputMultiplier { 255 };
    double leftAxis { 0.0 };
    double rightAxis { 0.0 };
    bool m_cameraEnabled;
};

#endif // APPENGINE_H
