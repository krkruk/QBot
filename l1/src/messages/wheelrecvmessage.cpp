#include "wheelrecvmessage.h"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>


WheelRecvMessage::WheelRecvMessage(int id, boost::property_tree::ptree &&content)
    : id{id}, content{std::move(content)}
{
}

int WheelRecvMessage::getId() const
{
    return id;
}

double WheelRecvMessage::getAngularVelocity() const
{
    return get_value(KEY_ANGULAR_VEL, 0.0);
}

int WheelRecvMessage::getMilliamps() const
{
    return get_value(KEY_CURRENT, 0);
}

double WheelRecvMessage::getCelsius() const
{
    return get_value(KEY_TEMPERATURE, 0.0);
}

int WheelRecvMessage::getPwm() const
{
    return get_value(KEY_PWM, 0);
}

int WheelRecvMessage::getErrorCode() const
{
    return get_value(KEY_ERROR_CODE, 0);
}

WheelRecvMessage WheelRecvMessage::fromRaw(int id, const std::string &json)
{
    std::stringstream ss(json);
    boost::property_tree::ptree content;
    boost::property_tree::json_parser::read_json(ss, content);

    return WheelRecvMessage(id, std::move(content));
}

template<typename T>
T WheelRecvMessage::get_value(const char *key, T default_value) const
{
    return content.get_child(std::to_string(id))
            .get<T>(key, default_value);
}
