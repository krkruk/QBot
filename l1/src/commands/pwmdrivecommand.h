#ifndef PWMDRIVECOMMAND_H
#define PWMDRIVECOMMAND_H
#include "drivecommand.h"


/**
 * The PwmDriveCommand implements the following behavior:
 *  * any LeftValue shall be set for all left column wheels
 *  * any RightValue shall be set for all right column wheels
 *
 * The ID enumeration is as follows (e.g. 6-wheel chassis):
 *  (IDs placed in parentheses) <br />
 *
 *   (0) ||  ------- ||  (3) <br />
 *              |            <br />
 *              |            <br />
 *   (1) ||  ------- ||  (4) <br />
 *              |            <br />
 *              |            <br />
 *   (2) ||  ------- ||  (5) <br />
 * @brief The PwmDriveCommand class implements PWM based control of the robot.
 */
class PwmDriveCommand : public DriveCommand
{
    const unsigned int wheelCount;
    double leftValue{0.0};
    double rightValue{0.0};

public:
    /**
     * @brief PwmDriveCommand Constructor
     * @param wheelCount How many wheels are present in the chassis.
     */
    PwmDriveCommand(unsigned int wheelCount);

    /**
     * @brief Destructor
     */
    ~PwmDriveCommand() override = default;

    /**
     * @brief execute Computes received data and returns
     * a valid command to be sent to the robot microcontrollers
     * @return a message ready to be forwarded to the microcontrollers
     */
    std::vector<WheelSendMessage> execute() override;

    /**
     * @brief setLeftValue Sets PWM for the left column of wheels
     * @param lvalue PWM. For the min/max values consult with the uC
     * documentation
     */
    void setLeftValue(double lvalue);

    /**
     * @brief setRightValue Sets PWM for the right column of wheels
     * @param rvalue PWM. For the min/max values consult with the uC
     * documentation
     */
    void setRightValue(double rvalue);
};
#endif // PWMDRIVECOMMAND_H
