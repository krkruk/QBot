#ifndef GRPCCHASSISVISITOR_H
#define GRPCCHASSISVISITOR_H

namespace rpc {

namespace svc
{
    class PwmDriveCommand;
    class DifferentialDriveCommand;
    class PeripheralDeviceCommand;
}

/**
 * The class is responsible for handling a query and starting an action
 * defined in a received command knowing the structure of the underlying
 * hardware.
 * In other words, the interface provides a way to translate the command
 * sent by a client application into a set of commands acceptable by the
 * robot.
 * @brief The GrpcChassisVisitor class provides an interface for handling
 * remote queries.
 */
class GrpcChassisVisitor
{
public:
    /**
     * Destructor
     */
    virtual ~GrpcChassisVisitor() = default;

    /**
     * A method accept a PwmDriveCommand and translates it into
     * an internally recognizable command that can be sent to the
     * microcontrollers
     * @brief accept Accepts PWM-related commands.
     * @param cmd PWM command
     */
    virtual void accept(const rpc::svc::PwmDriveCommand &cmd) = 0;


    /**
     * A method accept a DifferentialDriveCommand and translates it into
     * an internally recognizable command that can be sent to the
     * microcontrollers
     * @brief accept Accepts a command that must provide an implementation
     * of differential steering algorithm
     * @param cmd differential steering command
     */
    virtual void accept(const rpc::svc::DifferentialDriveCommand &cmd) = 0;


    /**
     * A method accept a PeripheralDeviceCommand and translates it into
     * an internally recognizable command that can be sent to the
     * microcontrollers
     * @brief accept Accepts commands that can turn on/off peripheral hardware
     * @param cmd command
     */
    virtual void accept(const rpc::svc::PeripheralDeviceCommand &cmd) = 0;
};

}
#endif // GRPCCHASSISVISITOR_H
