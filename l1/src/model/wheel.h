#ifndef WHEEL_H
#define WHEEL_H
#include <memory>
#include <mutex>
#include "wheelrecvmessage.h"
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

    mutable std::mutex feedback_mtx;
    WheelRecvMessage feedback;

public:
    using message_type = WheelSendMessage;
    using feedback_type = WheelRecvMessage;

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

    /**
     * The method stores a message that contains telemetric data.
     *
     * The method is thread safe.
     * @brief setFeedback Stores a message received form the microcontroller
     * @param message a message
     */
    void setFeedback(feedback_type &&message);

    /**
     * @brief state Publishes telemetric data to one of the clients
     *
     * The method is thread safe.
     * @return telemetric data.
     */
    const feedback_type *state() const;

    /**
     * @brief generateMessage Generates a message builder
     * @return Message builder that will create a data structure ready to be
     * sent to the microcontroller
     */
    WheelSendMessage::Builder generateMessage() const;
};

}
#include "wheel.cpp"

#endif // WHEEL_H
