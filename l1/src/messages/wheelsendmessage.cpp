#include "wheelsendmessage.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>

std::string WheelSendMessage::toString(bool pretty) const
{
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, content, pretty);
    return ss.str();
}

WheelSendMessage::Builder::Builder(int id)
    : id {id}
{
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setPwm(int pwm) noexcept
{
    this->pwm = pwm;
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setAngularVelocity(double angularVelocity) noexcept
{
    this->angularVelocity = angularVelocity;
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setProportional(double kp) noexcept
{
    this->kp = kp;
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setIntegral(double ki) noexcept
{
    this->ki = ki;
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setDifferential(double kd) noexcept
{
    this->kd = kd;
    return *this;
}

WheelSendMessage WheelSendMessage::Builder::build()
{
    auto dec_format = boost::format("%.02f");

    WheelSendMessage msg;
    boost::property_tree::ptree values;
    values.put(KEY_PWM, pwm);
    values.put(KEY_ANGULAR_VEL, dec_format % angularVelocity);
    values.put(KEY_PROPORTIONAL, dec_format % kp);
    values.put(KEY_INTEGRAL, dec_format % ki);
    values.put(KEY_DIFFERENTIAL, dec_format % kd);
    msg.content.add_child(std::to_string(id), values);

    return msg;
}
