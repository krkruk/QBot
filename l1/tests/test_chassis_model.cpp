#define BOOST_TEST_MODULE test_chassis_model
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <utility>
#include <sstream>
#include <string>

#include "wheelsendmessage.h"
#include "jsonsink.h"
#include "wheel.h"


template<typename T>
bool contains(std::string root, T value)
{
    return root.find(value) != std::string::npos;
}

class MockSerialPort
{
    std::string data;
public:
    template<typename Message>
    void write(const Message &data)
    {
        this->data = data.toString();
    }

    void write(const std::string &data)
    {
        this->data = data;
    }

    std::string getData() const
    {
        return data;
    }
};


BOOST_AUTO_TEST_CASE(test_send_message_to_sink)
{
    using namespace model;
    auto sink = std::make_shared<JsonSink>();
    auto leftWheel = std::make_shared<Wheel<JsonSink>>(sink);
    auto rightWheel = std::make_shared<Wheel<JsonSink>>(sink);
    auto lmsg = WheelSendMessage::Builder(1).setPwm(200).build();
    auto rmsg = WheelSendMessage::Builder(2).setPwm(-200).build();
    leftWheel->sendMessage(lmsg);
    rightWheel->sendMessage(rmsg);

    const auto msg = sink->toString();
    BOOST_TEST(contains(msg, R"("1":{"PWM":"200")"));
    BOOST_TEST(contains(msg, R"("2":{"PWM":"-200")"));
}

BOOST_AUTO_TEST_CASE(test_send_message_directly_to_serial)
{
    using namespace model;
    auto serial = std::make_shared<MockSerialPort>();
    auto leftWheel = std::make_shared<Wheel<MockSerialPort>>(serial);
    auto lmsg = WheelSendMessage::Builder(1).setPwm(666).build();
    leftWheel->sendMessage(lmsg);

    BOOST_TEST(contains(serial->getData(), "666"));
}
