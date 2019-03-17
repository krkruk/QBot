#ifndef COMMAND_H
#define COMMAND_H


/**
 * The Command template is a generic way to create a Command Pattern (see GOF).
 */
template<typename RetValue = void>
class Command
{
public:
    /**
     * @brief ~Command Virtual destructor to ensure polimorphic destruction
     */
    virtual ~Command() {}

    /**
     * @brief execute Executes a command (any code with common interface)
     * @return RetValue, by standard implementation void
     */
    virtual RetValue execute() = 0;
};

#endif // COMMAND_H
