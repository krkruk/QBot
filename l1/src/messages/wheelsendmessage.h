#ifndef WHEELSENDMESSAGE_H
#define WHEELSENDMESSAGE_H
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <utility>


/**
 * The WheelSendMessage encapsulates all commands that can be sent to a client.
 *
 * As a result, any WheelSendMessage will generate the following JSON string:
 * {"id":{"PWM":int,"ROT":int,"KP":double,"KI":double,"KD":double}}
 *
 * @brief The WheelSendMessage class encapsulates all commands that can be sent
 * to the microcontroller
 */
class WheelSendMessage
{
    friend class Builder;
public:
    static constexpr const char *KEY_PWM = "PWM";
    static constexpr const char *KEY_ANGULAR_VEL = "ROT";
    static constexpr const char *KEY_PROPORTIONAL = "KP";
    static constexpr const char *KEY_INTEGRAL = "KI";
    static constexpr const char *KEY_DIFFERENTIAL = "KD";


    /**
     * The WheelSendMessage::Builder generates a message
     * in a convienient way.
     * @brief The Builder class build WheelSendMessage
     */
    class Builder
    {
        friend class WheelSendMessage;
        int id;
        std::pair<bool, int> pwm;
        std::pair<bool, double> angularVelocity;
        std::pair<bool, double> kp;
        std::pair<bool, double> ki;
        std::pair<bool, double> kd;

    public:
        /**
         * @brief Builder Creates a Builder to generate
         * a WheelSendMessage instance
         * @param id an ID of a wheel. The ID is a mean
         * to choose a device where a command can be issue.
         */
        explicit Builder(int id);

        /**
         * @brief setPwm Sets PWM in a wheel of the given ID
         * @param pwm PWM value. The values must be consulted with
         * the documentation of a wheel driver provider.
         * @return  Builder reference
         */
        Builder &setPwm(int pwm) noexcept;

        /**
         * @brief setAngularVelocity Sets angular velocity in a wheel
         * of the given ID
         * @param angularVelocity Angular velocity
         * @return  Builder reference
         */
        Builder &setAngularVelocity(double angularVelocity) noexcept;

        /**
         * @brief setProportional Sets proportional constant
         * and allows sending it to the client device.
         * @param kp proportional coefficient
         * @return  Builder reference
         */
        Builder &setProportional(double kp) noexcept;

        /**
         * @brief setIntegral Sets integral constant
         * and allows sending it to the client device.
         * @param ki integral coefficient
         * @return  Builder reference
         */
        Builder &setIntegral(double ki) noexcept;

        /**
         * @brief setDifferential Sets differential constant
         * and allows sending it to the client device.
         * @param kd differential coefficient
         * @return  Builder reference
         */
        Builder &setDifferential(double kd) noexcept;

        /**
         * A builder executor. It build a final object.
         * @brief build Builds WheelSendMessage
         * @return WheelSendMessage
         */
        WheelSendMessage build();
    };


    /**
     * @brief toString Generates string representation of JSON
     * values
     * @param pretty formats the data so it looks pretty
     * @return string representation of JSON
     */
    std::string toString(bool pretty = false) const;

    /**
     * @brief toJson Returns JSON representation
     * @return JSON
     */
    boost::property_tree::ptree toJson() const;

private:
    boost::property_tree::ptree content;
};

#endif // WHEELSENDMESSAGE_H
