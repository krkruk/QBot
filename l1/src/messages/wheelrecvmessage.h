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


    /**
     * The method return a unique identifier called ID. The ID
     * is assigned to each individual wheel.
     * @brief getId Returns ID of the wheel.
     * @return  a unique ID
     */
    int getId() const;

    /**
     * Returns angular velocity. Please refer to the hardware
     * provider to find the units.
     * @brief getAngularVelocity Returns angular velocity
     * @return
     */
    double getAngularVelocity() const;

    /**
     * @brief getMilliamps Returns a value in milliamps of
     * temporary current consumption
     * @return milliamps
     */
    int getMilliamps() const;

    /**
     * @brief getCelsius Returns temperature of MOSFET power
     * transistors
     * @return temperature in Celsius
     */
    double getCelsius() const;

    /**
     * @brief getPwm Returns currently set PWM value
     * @return PWM value
     */
    int getPwm() const;

    /**
     * Returns error code. Error code '0' must mean 'no errors'.
     * @brief getErrorCode Returns error code
     * @return error code
     */
    int getErrorCode() const;


    /**
     * Parses JSON and loads telemetry data.
     * @brief fromRaw Parser raw json
     * @param id ID of wheel
     * @param json JSON
     * @return an instance of WheelRecvMessage
     */
    static WheelRecvMessage fromRaw(int id, const std::string &json);

private:
    int id {-1};
    boost::property_tree::ptree content;

    template<typename T>
    T get_value(const char *key, T default_value) const;
};

#endif // WHEELRECVMESSAGE_H
