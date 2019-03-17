#include "pwmdrivecommand.h"


PwmDriveCommand::PwmDriveCommand(unsigned int wheelCount)
    : wheelCount{wheelCount}
{}

std::vector<WheelSendMessage> PwmDriveCommand::execute()
{
    std::vector<WheelSendMessage> values(wheelCount);
    unsigned int i {0};
    auto cast = [](auto value) { return static_cast<int>(value); };

    for (; i < wheelCount / 2; ++i)
    {
        values[i] = WheelSendMessage::Builder(cast(i))
                .setPwm(cast(leftValue)).build();
    }

    for (; i < wheelCount; ++i)
    {
        values[i] = WheelSendMessage::Builder(cast(i))
                .setPwm(cast(rightValue)).build();
    }
    return values;
}

void PwmDriveCommand::setLeftValue(double lvalue)
{
    leftValue = lvalue;
}

void PwmDriveCommand::setRightValue(double rvalue)
{
    rightValue = rvalue;
}
