#ifndef MOTOR_H_
#define MOTOR_H_
#include <Arduino.h>
#include <math.h>

namespace drv
{

template<typename MotorDrv>
class Motor
{
public:
    void setSpeed(int16_t speed)
    {
        auto *child = static_cast<MotorDrv *>(this);
        const auto sign = child->computeDirection(speed);
        const uint8_t pwm = static_cast<uint8_t>(abs(speed)) ;

        child->setPwm(pwm);
        child->setDirection(sign);
    }

    int16_t getSpeed() const
    {
        return static_cast<MotorDrv *>(this)->getSpeed();
    }
};

} // namespace drv
#endif // MOTOR_H_
