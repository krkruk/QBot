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
        static unsigned wheel_counter;
        if (wheel_counter < WHEELS)
        {
            wheels[wheel_counter++] = wheel;
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
};

}

#endif // CHASSIS_H
