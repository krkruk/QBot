#ifndef SEQUENTIALCOMMANDEXECUTOR_H
#define SEQUENTIALCOMMANDEXECUTOR_H
#include <functional>
#include <memory>
#include <queue>

class DriveCommand;
class WheelSendMessage;

class SequentialCommandExecutor
{
    using Callback = std::function<void(std::vector<WheelSendMessage>)>;

    std::queue<std::unique_ptr<DriveCommand>> commands;
    Callback callback;
public:
    SequentialCommandExecutor();
    SequentialCommandExecutor(const SequentialCommandExecutor&) = delete;
    SequentialCommandExecutor(SequentialCommandExecutor&&) = delete;
    SequentialCommandExecutor &operator=(const SequentialCommandExecutor&) = delete;
    SequentialCommandExecutor &operator=(SequentialCommandExecutor&&) = delete;

    void setCallback(Callback callback);

    void addCommand(std::unique_ptr<DriveCommand> cmd);

    void exec();
};


#endif // SEQUENTIALCOMMANDEXECUTOR_H
