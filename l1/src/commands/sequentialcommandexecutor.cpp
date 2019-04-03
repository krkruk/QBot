#include "sequentialcommandexecutor.h"

#include <exception>
#include "wheelsendmessage.h"
#include "drivecommand.h"


SequentialCommandExecutor::SequentialCommandExecutor()
{
}

void SequentialCommandExecutor::setNotifier(SequentialCommandExecutor::Callback callback)
{
    this->callback = callback;
}

void SequentialCommandExecutor::addCommand(std::unique_ptr<DriveCommand> cmd)
{
    commands.push(std::move(cmd));
}

void SequentialCommandExecutor::exec()
{
    if (!callback)
    {
        throw std::logic_error("Missing callback");
    }

    while (commands.size())
    {
        // Consider running these commands in boost::asio::io_context by posting a task
        auto &cmd = commands.front();
        auto result = cmd->execute();
        callback(std::move(result));
        commands.pop();
    }
}
