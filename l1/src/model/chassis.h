#ifndef CHASSIS_H
#define CHASSIS_H
#include <exception>
#include <vector>
#include <memory>
#include <array>

namespace model
{

template<typename WheelClass, int WHEELS>
class Chassis
{
    std::array<std::weak_ptr<WheelClass>, WHEELS> wheels;
public:
    static constexpr const int wheel_count = WHEELS;

    /**
     * The {@class Chassis} combines all wheels into a unified chassis model.
     * The class cooperates with Command that produces a valid data
     * that can be consumed by WheelClass instance.
     *
     * @brief addWheel Adds a wheel to be recognized in the chassis model
     * @param wheel a reference to a wheel object
     * @throws std::invalid_argument Throws an exception if one tries
     * to pass more WheelClass than defined by WHEELS
     */
    void addWheel(std::weak_ptr<WheelClass> wheel)
    {
        auto sh_wheel = wheel.lock();
        if (sh_wheel && sh_wheel->getId() < WHEELS)
        {
            wheels[sh_wheel->getId()] = wheel;
        }
        else
        {
            throw std::invalid_argument("Too many wheels added.");
        }
    }

    /**
     * The method is responsible for sending data to a data sink of each wheel.
     * The sink usually is a serial instance but it is not a principle
     * @brief notify Writes WheelClass::message_type data into a data sink.
     * @param message A message accepted by the data sink.
     */
    void notify(std::vector<typename WheelClass::message_type> &&message)
    {
        if (message.size() != wheel_count)
        {
            throw  std::invalid_argument("Invalid wheel number.");
        }


        for (std::size_t i{0}; i < wheel_count; ++i)
        {
            if (auto wheel = wheels[i].lock())
            {
                wheel->sendMessage(std::move(message[i]));
            }
        }
    }

    /**
     * @brief processFeedback The method receives a parsed feedback_type
     * message to be stored by a WheelClass instance. This is to provide
     * some feedback data to any clients that may query the application.
     *
     * The data usually comprises telemetry such as temperature, milliamps etc.
     * @param message WheelMessage type
     */
    void processFeedback(typename WheelClass::feedback_type &&message)
    {
        const auto id = message.getId();
        if (id < wheel_count)
        {
            if (auto wheel = wheels[id].lock())
            {
                wheel->setFeedback(std::move(message));
            }
        }
    }

    const typename WheelClass::feedback_type *state(int id) const
    {
        if (id >= wheel_count)
        {
            throw  std::invalid_argument("Invalid wheel number.");
        }

        if (const auto wheel = wheels[id].lock())
        {
            return wheel->state();
        }
        else
        {
            throw  std::runtime_error("Cannot get instance to WheelClass.");
        }
    }
};

}

#endif // CHASSIS_H
