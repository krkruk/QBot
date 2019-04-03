#ifndef WHEEL_H
#define WHEEL_H
#include <memory>
#include "wheelsendmessage.h"


namespace model
{

/**
 * The Wheel class
 */
template<typename DataSink>
class Wheel
{
    const int id;
    std::weak_ptr<DataSink> sink;
public:
    using message_type = WheelSendMessage;

    /**
     * @brief Wheel Constructor, requires DataSink
     * with a method DataSing::write(Message)
     * @param sink A data sink
     */
    explicit Wheel(int id, std::weak_ptr<DataSink> sink);

    int getId() const;

    /**
     * The method sends message to the DataSink. Please
     * refer to DataSink documentation to know what type
     * of messages are acceptable.
     * @brief Sends a message to the registered DataSink
     */
    template<typename Message>
    void sendMessage(const Message &msg);

    WheelSendMessage::Builder generateMessage() const;
};

}
#include "wheel.cpp"

#endif // WHEEL_H
