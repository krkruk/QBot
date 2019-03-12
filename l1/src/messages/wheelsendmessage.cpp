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

boost::property_tree::ptree WheelSendMessage::toJson() const
{
   return content;
}

WheelSendMessage::Builder::Builder(int id)
    : id {id}
{
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setPwm(int pwm) noexcept
{
    this->pwm = std::make_pair(true, pwm);
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setAngularVelocity(double angularVelocity) noexcept
{
    this->angularVelocity = std::make_pair(true, angularVelocity);
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setProportional(double kp) noexcept
{
    this->kp = std::make_pair(true, kp);
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setIntegral(double ki) noexcept
{
    this->ki = std::make_pair(true, ki);
    return *this;
}

WheelSendMessage::Builder &WheelSendMessage::Builder::setDifferential(double kd) noexcept
{
    this->kd = std::make_pair(true, kd);
    return *this;
}

WheelSendMessage WheelSendMessage::Builder::build()
{
    auto dec_format = boost::format("%.02f");

    WheelSendMessage msg;
    boost::property_tree::ptree values;
    auto set_value = [&values, &dec_format](const char *key, const std::pair<bool, double> &value)
    {
        if (value.first)
        {
            values.put(key, dec_format % value.second);
        }
    };

    if (pwm.first)
    {
        values.put(KEY_PWM, pwm.second);
    }
    set_value(KEY_ANGULAR_VEL, angularVelocity);
    set_value(KEY_PROPORTIONAL, kp);
    set_value(KEY_INTEGRAL, ki);
    set_value(KEY_DIFFERENTIAL, kd);
    msg.content.add_child(std::to_string(id), values);

    return msg;
}
