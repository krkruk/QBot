#ifndef CHASSIS_H
#define CHASSIS_H
#include <exception>
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
};

}

#endif // CHASSIS_H
