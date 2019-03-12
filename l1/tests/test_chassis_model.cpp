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

namespace model
{
    template<typename WheelClass>
    class Chassis
    {
    public:
    };
}

BOOST_AUTO_TEST_CASE(test_send_message_to_sink)
{
    using namespace model;
    auto sink = std::make_shared<JsonSink>();
    auto leftWheel = std::make_shared<Wheel<JsonSink>>(1, sink);
    auto rightWheel = std::make_shared<Wheel<JsonSink>>(2, sink);
    auto lmsg = leftWheel->generateMessage().setPwm(200).build();
    auto rmsg = rightWheel->generateMessage().setPwm(-200).build();
    leftWheel->sendMessage(lmsg);
    rightWheel->sendMessage(rmsg);

    const auto msg = sink->toString();
    std::cout << "lmsg: " << lmsg.toString() << std::endl;
    std::cout << "MSG: " << msg << std::endl;
    BOOST_TEST(contains(msg, R"("1":{"PWM":"200")"));
    BOOST_TEST(contains(msg, R"("2":{"PWM":"-200")"));
}

BOOST_AUTO_TEST_CASE(test_send_message_directly_to_serial)
{
    using namespace model;
    auto serial = std::make_shared<MockSerialPort>();
    auto leftWheel = std::make_shared<Wheel<MockSerialPort>>(1, serial);
    auto lmsg = leftWheel->generateMessage().setPwm(666).build();
    leftWheel->sendMessage(lmsg);

    BOOST_TEST(contains(serial->getData(), "666"));
}

BOOST_AUTO_TEST_CASE(test_register_in_chassis)
{
    using Wheel = model::Wheel<MockSerialPort>;
    auto serial = std::make_shared<MockSerialPort>();
    auto leftWheel = std::make_shared<Wheel>(1, serial);
    auto chassis = std::make_unique<model::Chassis<Wheel>>();
    auto lmsg = leftWheel->generateMessage().setPwm(100).build();
//    chassis->addWheel(leftWheel);
}
