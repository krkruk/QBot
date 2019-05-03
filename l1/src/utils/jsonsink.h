#ifndef JSONSINK_H
#define JSONSINK_H
#include <boost/property_tree/ptree.hpp>


/**
 * The JsonSink class collects JSON objects and combines
 * them into a single one.
 *
 * Such feature may be useful if many objects have
 * create a single message to a microcontroller.
 *
 * For instance, a robot has 4 wheel and one microcontroller
 * that controls them. It is reasonable to combine JSON messages
 * into a single one and send a combined message to the
 * microcontroller.
 * {wheel[0], wheel[1], wheel[2], wheel[3]}
 *      -> JsonSink (combine messages)
 *          -> SerialPort (send a single msg to uC)
 * @brief The JsonSink class collects JSON objects
 */
class JsonSink
{
    boost::property_tree::ptree json;
public:

    /**
     * @brief Collects JSON messages and
     * combines them into a single message
     * @param data an object that has toJson() member.
     */
    template<typename Message>
    void write(const Message &data)
    {
        for (const auto &parent : data.toJson())
        {
            json.add_child(parent.first, parent.second);
        }
    }

    /**
     * @brief toString Returns a serialized JSON message
     * ready to be sent anywhere
     * @param pretty true to enable indents
     * @return a serialized message
     */
    std::string toString(bool pretty = false);

    /**
     * @brief operator bool Data can be always sent to sink.
     */
    operator bool() const
    {
        return true;
    }
};
#endif // JSONSINK_H
