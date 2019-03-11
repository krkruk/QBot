#ifndef WHEEL_H
#define WHEEL_H
#include <memory>


namespace model
{

/**
 * The Wheel class
 */
template<typename DataSink>
class Wheel
{
    std::weak_ptr<DataSink> sink;
public:
    /**
     * @brief Wheel Constructor, requires DataSink
     * with a method DataSing::write(Message)
     * @param sink A data sink
     */
    explicit Wheel(std::weak_ptr<DataSink> sink);


    /**
     * The method sends message to the DataSink. Please
     * refer to DataSink documentation to know what type
     * of messages are acceptable.
     * @brief Sends a message to the registered DataSink
     */
    template<typename Message>
    void sendMessage(const Message &msg);
};

}
#include "wheel.cpp"

#endif // WHEEL_H
