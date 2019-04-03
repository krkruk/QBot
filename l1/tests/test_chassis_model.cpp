#define BOOST_TEST_MODULE test_chassis_model
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <string>

#include "sequentialcommandexecutor.h"
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

int extract(const std::vector<WheelSendMessage> &values, unsigned int id)
{
    return values[id].toJson().get_child(std::to_string(id))
            .get<int>(WheelSendMessage::KEY_PWM);
};

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

BOOST_AUTO_TEST_CASE(test_pwm_drive_command)
{
    constexpr int wheelCount = 4;
    auto cmd = std::make_unique<PwmDriveCommand>(wheelCount);
    cmd->setLeftValue(100);
    cmd->setRightValue(-100);
    std::vector<WheelSendMessage> values = cmd->execute();

    BOOST_CHECK_EQUAL(100, extract(values, 0));
    BOOST_CHECK_EQUAL(100, extract(values, 1));
    BOOST_CHECK_EQUAL(-100, extract(values, 2));
    BOOST_CHECK_EQUAL(-100, extract(values, 3));
}

BOOST_AUTO_TEST_CASE(test_command_executor)
{
    constexpr int wheelCount = 2;
    std::vector<WheelSendMessage> rcvd_values;
    auto callback = [&rcvd_values](std::vector<WheelSendMessage> &&message)
    {
        rcvd_values = std::move(message);
    };
    SequentialCommandExecutor exec;
    exec.setNotifier(callback);
    auto command = std::make_unique<PwmDriveCommand>(wheelCount);
    command->setLeftValue(100);
    command->setRightValue(250);
    exec.addCommand(std::move(command));
    command = std::make_unique<PwmDriveCommand>(wheelCount);
    command->setLeftValue(0);
    command->setRightValue(-250);
    exec.addCommand(std::move(command));
    exec.exec();

    BOOST_CHECK_EQUAL(0, extract(rcvd_values, 0));
    BOOST_CHECK_EQUAL(-250, extract(rcvd_values, 1));
}

BOOST_AUTO_TEST_CASE(test_register_in_chassis)
{
    using Wheel = model::Wheel<MockSerialPort>;
    constexpr int wheelCount {2};
    auto lserial = std::make_shared<MockSerialPort>();
    auto rserial = std::make_shared<MockSerialPort>();
    auto leftWheel = std::make_shared<Wheel>(0, lserial);
    auto rightWheel = std::make_shared<Wheel>(1, rserial);
    auto chassis = std::make_unique<model::Chassis<Wheel, wheelCount>>();
    chassis->addWheel(leftWheel);
    chassis->addWheel(rightWheel);
    auto command = std::make_unique<PwmDriveCommand>(wheelCount);
    command->setLeftValue(200);
    command->setRightValue(-200);
    auto executor = std::make_unique<SequentialCommandExecutor>();
    executor->setNotifier([&chassis](std::vector<WheelSendMessage> &&message)
    {
        chassis->notify(std::move(message));
    });
    executor->addCommand(std::move(command));
    executor->exec();

    BOOST_TEST(contains(lserial->getData(), R"("0":{"PWM":"200")"));
    BOOST_TEST(contains(rserial->getData(), R"("1":{"PWM":"-200")"));
}
