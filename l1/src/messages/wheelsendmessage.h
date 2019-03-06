#ifndef WHEELSENDMESSAGE_H
#define WHEELSENDMESSAGE_H
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>


class WheelSendMessage
{
    friend class Builder;
public:
    static constexpr const char *KEY_PWM = "PWM";
    static constexpr const char *KEY_ANGULAR_VEL = "ROT";
    static constexpr const char *KEY_PROPORTIONAL = "KP";
    static constexpr const char *KEY_INTEGRAL = "KI";
    static constexpr const char *KEY_DIFFERENTIAL = "KD";


    class Builder
    {
        friend class WheelSendMessage;
        int id {0};
        int pwm {0};
        int angularVelocity {0};
        double kp {0.0};
        double ki {0.0};
        double kd {0.0};

    public:
        explicit Builder(int id);

        Builder(const Builder &) = delete;
        Builder(Builder &&) = delete;
        Builder &operator=(const Builder &) = delete;
        Builder &operator=(Builder &&) = delete;

        Builder &setPwm(int pwm) noexcept;
        Builder &setAngularVelocity(int angularVelocity) noexcept;
        Builder &setProportional(double kp) noexcept;
        Builder &setIntegral(double ki) noexcept;
        Builder &setDifferential(double kd) noexcept;
        WheelSendMessage build();
    };


    std::string toString(bool pretty = false) const;

private:
    boost::property_tree::ptree content;
};

#endif // WHEELSENDMESSAGE_H
