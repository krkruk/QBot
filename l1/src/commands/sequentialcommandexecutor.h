#ifndef SEQUENTIALCOMMANDEXECUTOR_H
#define SEQUENTIALCOMMANDEXECUTOR_H
#include <functional>
#include <memory>
#include <queue>

class DriveCommand;
class WheelSendMessage;

/**
 * @brief The SequentialCommandExecutor class is responsible for
 * executing Commands in order
 */
class SequentialCommandExecutor
{
    using Callback = std::function<void(std::vector<WheelSendMessage>)>;

    std::queue<std::unique_ptr<DriveCommand>> commands;
    Callback callback;
public:
    /**
     * @brief SequentialCommandExecutor Constructor
     */
    SequentialCommandExecutor();
    SequentialCommandExecutor(const SequentialCommandExecutor&) = delete;
    SequentialCommandExecutor(SequentialCommandExecutor&&) = delete;
    SequentialCommandExecutor &operator=(const SequentialCommandExecutor&) = delete;
    SequentialCommandExecutor &operator=(SequentialCommandExecutor&&) = delete;

    /**
     * The method registers a consumer of data generated by the Command.
     * The callback therefore must accept a product generated
     * by Command::execute() method. See the implementation
     * of the specific Command for more details.
     *
     * Once exec() method is called, all registered commands shall
     * be launched one by one with no wait period.
     * @brief setNotifier Sets a consumer of the Command
     * @param callback a Consumer
     */
    void setNotifier(Callback callback);

    /**
     * The method adds a command into a queue.
     * @brief addCommand Adds a Command
     * @param cmd a command
     */
    void addCommand(std::unique_ptr<DriveCommand> cmd);

    /**
     * The method starts all registered commands and sends
     * generated data into a registered callback.
     * @brief exec Launches all stored Commands.
     */
    void exec();
};


#endif // SEQUENTIALCOMMANDEXECUTOR_H
