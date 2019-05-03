#ifndef CHASSISMODEL_H
#define CHASSISMODEL_H
#include <memory>
#include <vector>
class SequentialCommandExecutor;
class Wheel;
namespace rpc
{
class GrpcChassisVisitor;
}

class ChassisModel
{
public:
    virtual ~ChassisModel() = default;
    /**
     * @brief getExecutor Returns executor
     * @return Executor
     */
    virtual std::weak_ptr<SequentialCommandExecutor> getExecutor() = 0;

    /**
     * @brief getVisitor Returns visitor
     * @return Visitor
     */
    virtual std::weak_ptr<rpc::GrpcChassisVisitor> getVisitor() = 0;

    /**
     * @brief checkSerialHealth Checks if all serial are OK.
     * @return False if one of the serials is not OK.
     */
    virtual bool checkSerialHealth() const = 0;
};

#endif // CHASSISMODEL_H
