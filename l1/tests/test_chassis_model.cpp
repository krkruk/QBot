#define BOOST_TEST_MODULE test_chassis_model
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <string>

#include "wheelsendmessage.h"
#include "pwmdrivecommand.h"
#include "jsonsink.h"
#include "chassis.h"
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
    auto leftWheel = std::make_shared<Wheel<JsonSink>>(1, sink);
    auto rightWheel = std::make_shared<Wheel<JsonSink>>(2, sink);
    auto lmsg = leftWheel->generateMessage().setPwm(200).build();
    auto rmsg = rightWheel->generateMessage().setPwm(-200).build();
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
    auto leftWheel = std::make_shared<Wheel<MockSerialPort>>(1, serial);
    auto lmsg = leftWheel->generateMessage().setPwm(666).build();
    leftWheel->sendMessage(lmsg);

    BOOST_TEST(contains(serial->getData(), "666"));
}

BOOST_AUTO_TEST_CASE(test_register_in_chassis)
{
    using Wheel = model::Wheel<MockSerialPort>;
    auto serial = std::make_shared<MockSerialPort>();
    auto leftWheel = std::make_shared<Wheel>(0, serial);
    auto rightWheel = std::make_shared<Wheel>(1, serial);
    auto chassis = std::make_unique<model::Chassis<Wheel, 2>>();
    chassis->addWheel(leftWheel);
    chassis->addWheel(rightWheel);
    //TODO: what's next?
}

BOOST_AUTO_TEST_CASE(test_pwm_drive_command)
{
    constexpr int wheelCount = 4;
    auto cmd = std::make_unique<PwmDriveCommand>(wheelCount);
    cmd->setLeftValue(100);
    cmd->setRightValue(-100);
    std::vector<WheelSendMessage> values = cmd->execute();

    auto extract = [&values](unsigned int id)
    {
        return values[id].toJson().get_child(std::to_string(id))
                .get<int>(WheelSendMessage::KEY_PWM);
    };

    BOOST_CHECK_EQUAL(100, extract(0));
    BOOST_CHECK_EQUAL(100, extract(1));
    BOOST_CHECK_EQUAL(-100, extract(2));
    BOOST_CHECK_EQUAL(-100, extract(3));
}


//BOOST_AUTO_TEST_CASE(test_command_executor)
//{
//    constexpr int wheelCount = 2;
//    CommandExecutor exec;
//    std::unordered_map<int, double> recvd_values;
//    auto callback = [&recvd_values](std::unordered_map<int, double> values)
//    {
//        recvd_values = values;
//    };
//    exec.setCallback(callback);
//    auto command = std::make_unique<PwmDriveCommand>(wheelCount);
//    command->setLeftValue(100);
//    command->setRightValue(-100);
//    exec.addCommand(command);
//    exec.exec();

//    BOOST_CHECK_EQUAL(100, recvd_values[0]);
//    BOOST_CHECK_EQUAL(-100, recvd_values[1]);
//}
