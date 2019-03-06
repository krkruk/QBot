#ifndef WHEELRECVMESSAGE_H
#define WHEELRECVMESSAGE_H
#include <boost/property_tree/ptree.hpp>
#include <string>


class WheelRecvMessage
{
    WheelRecvMessage(int id, boost::property_tree::ptree &&content);
public:
    static constexpr const char *KEY_ANGULAR_VEL = "ROT";
    static constexpr const char *KEY_CURRENT = "CUR";
    static constexpr const char *KEY_TEMPERATURE = "TMP";
    static constexpr const char *KEY_PWM = "PWM";
    static constexpr const char *KEY_ERROR_CODE = "ERR";


    int getId() const;
    int getAngularVelocity() const;
    int getMilliamps() const;
    double getCelsius() const;
    int getPwm() const;
    int getErrorCode() const;

    static WheelRecvMessage fromRaw(int id, const std::string &json);

private:
    int id {-1};
    boost::property_tree::ptree content;

    template<typename T>
    T get_value(const char *key, T default_value) const;
};

#endif // WHEELRECVMESSAGE_H
