#define BOOST_TEST_MODULE test_chassis_model
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <string>

#include "sequentialcommandexecutor.h"
#include "jsondispatcherprocessor.h"
#include "wheelsendmessage.h"
#include "wheelrecvmessage.h"
#include "pwmdrivecommand.h"
#include "dispatcher.h"
#include "jsonsink.h"
#include "chassis.h"
#include "wheel.h"

#include "grpcchassiscontroller.h"
#include "grpcchassisvisitor.h"
#include "service.pb.h"


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

    /**
     * @brief operator bool This is a mock serial so it always works :D
     */
    operator bool() const
    {
        return true;
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

BOOST_AUTO_TEST_CASE(test_cleaning_the_jsonsink)
{
    using namespace model;
    auto sink = std::make_shared<JsonSink>();
    auto leftWheel = std::make_shared<Wheel<JsonSink>>(1, sink);
    auto rightWheel = std::make_shared<Wheel<JsonSink>>(2, sink);
    auto lmsg = leftWheel->generateMessage().setPwm(200).build();
    auto rmsg = rightWheel->generateMessage().setPwm(-200).build();
    leftWheel->sendMessage(lmsg);
    rightWheel->sendMessage(rmsg);

    auto msg = sink->toString();
    lmsg = leftWheel->generateMessage().setPwm(16).build();
    rmsg = rightWheel->generateMessage().setPwm(32).build();
    leftWheel->sendMessage(lmsg);
    rightWheel->sendMessage(rmsg);

    msg = sink->toString();
    BOOST_TEST(!contains(msg, R"("1":{"PWM":"200")"));
    BOOST_TEST(!contains(msg, R"("2":{"PWM":"-200")"));
    BOOST_TEST(contains(msg, R"("1":{"PWM":"16")"));
    BOOST_TEST(contains(msg, R"("2":{"PWM":"32")"));
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

BOOST_AUTO_TEST_CASE(test_register_feedback_message)
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

    auto lmsg = WheelRecvMessage::fromRaw(0, R"({"PWM":200,"CUR":2800})");
    auto rmsg = WheelRecvMessage::fromRaw(1, R"({"PWM":-200,"CUR":500})");

    chassis->processFeedback(std::move(lmsg));
    chassis->processFeedback(std::move(rmsg));

    BOOST_CHECK_EQUAL(200, leftWheel->state()->getPwm());
    BOOST_CHECK_EQUAL(2800, leftWheel->state()->getMilliamps());
    BOOST_CHECK_EQUAL(-200, rightWheel->state()->getPwm());
    BOOST_CHECK_EQUAL(500, rightWheel->state()->getMilliamps());
}

BOOST_AUTO_TEST_CASE(test_dispatch_feedback_in_full_model)
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

    const std::string rjson{R"({"1":{"ROT":"20.0","CUR":"1260","TMP":"26.0","PWM":"-200","ERR":"0"}})"};
    MappedDispatcher<std::string, JsonDispatcherProcessor<int, boost::property_tree::ptree>>
            dispatcher;
    dispatcher.addObserver(0, [leftWheel](int id, boost::property_tree::ptree tree)
    {
        leftWheel->setFeedback(WheelRecvMessage::fromTree(id, tree));
    });
    dispatcher.addObserver(1, [rightWheel](int id, boost::property_tree::ptree tree)
    {
        rightWheel->setFeedback(WheelRecvMessage::fromTree(id, tree));
    });

    dispatcher.dispatch(rjson);
    dispatcher.notify();

    BOOST_CHECK_EQUAL(0, leftWheel->state()->getPwm());
    BOOST_CHECK_EQUAL(0, leftWheel->state()->getMilliamps());
    BOOST_CHECK_EQUAL(-200, rightWheel->state()->getPwm());
    BOOST_CHECK_EQUAL(1260, rightWheel->state()->getMilliamps());

    const std::string ljson{R"({"0":{"ROT":"66.6","CUR":"1260","TMP":"26.0","PWM":"-66","ERR":"0"}})"};
    dispatcher.dispatch(ljson);
    dispatcher.notify();
    BOOST_CHECK_EQUAL(-66, leftWheel->state()->getPwm());
    BOOST_TEST(66.6 == leftWheel->state()->getAngularVelocity(),
               boost::test_tools::tolerance(0.01));
    BOOST_CHECK_EQUAL(-66, chassis->state(0)->getPwm());
    BOOST_TEST(66.6 == chassis->state(0)->getAngularVelocity(),
               boost::test_tools::tolerance(0.01));
    BOOST_CHECK_EQUAL(-200, rightWheel->state()->getPwm());
    BOOST_CHECK_EQUAL(1260, rightWheel->state()->getMilliamps());
}

BOOST_AUTO_TEST_CASE(test_pwm_visitor)
{
    constexpr int wheelCount {2};
    std::vector<WheelSendMessage> messages;
    auto executor = std::make_shared<SequentialCommandExecutor>();
    executor->setNotifier([&messages](auto &&msgs)
    {
        messages = std::move(msgs);
    });

    std::unique_ptr<rpc::GrpcChassisVisitor> visitor {
        std::make_unique<rpc::GrpcChassisController>(wheelCount, executor)
    };
    rpc::svc::PwmDriveCommand pwm;
    pwm.set_lpwm(128);
    pwm.set_rpwm(-200);
    visitor->accept(pwm);
    executor->exec();

    BOOST_ASSERT(wheelCount == messages.size());
    BOOST_CHECK_EQUAL(128, extract(messages, 0));
    BOOST_CHECK_EQUAL(-200, extract(messages, 1));
}

BOOST_AUTO_TEST_CASE(test_combine_all_together_separate_microcontrollers)
{
    constexpr int wheelCount {2};
    // Create data sinks. A data sink is any client that accepts data
    // incoming from a chassis model.
    using Wheel = model::Wheel<MockSerialPort>;
    auto lserial = std::make_shared<MockSerialPort>();
    auto rserial = std::make_shared<MockSerialPort>();

    // Create wheel instances. These wheels must be also present in real life.
    // Moreover, Wheel ID must be aligned with the DataSink.
    auto leftWheel = std::make_shared<Wheel>(0, lserial);
    auto rightWheel = std::make_shared<Wheel>(1, rserial);

    // Create a model of the chassis.
    auto chassis = std::make_unique<model::Chassis<Wheel, wheelCount>>();
    chassis->addWheel(leftWheel);
    chassis->addWheel(rightWheel);

    // Create a command executor. It dispatches commands received from the client
    // app
    auto executor = std::make_shared<SequentialCommandExecutor>();
    executor->setNotifier([&chassis](std::vector<WheelSendMessage> &&message)
    {
        chassis->notify(std::move(message));
    });

    // Retrieves a visitor responsible for receivng external commands.
    auto visitor = std::make_unique<rpc::GrpcChassisController>(wheelCount, executor);

    // This command shall be sent by a client application
    rpc::svc::PwmDriveCommand pwm;
    pwm.set_lpwm(666);
    pwm.set_rpwm(-44);

    // This command accepts a command from the remote application.
    visitor->accept(pwm);

    // Usually it is a good habit to inform the hardware that there are
    // new commands to execute.
    executor->exec();

    BOOST_TEST(contains(lserial->getData(), R"("0":{"PWM":"666")"));
    BOOST_TEST(contains(rserial->getData(), R"("1":{"PWM":"-44")"));
}

BOOST_AUTO_TEST_CASE(test_combine_all_together_single_uc)
{
    constexpr int wheelCount {4};
    // Create data sinks. A data sink is any client that accepts data
    // incoming from a chassis model.
    using Wheel = model::Wheel<JsonSink>;
    auto sink = std::make_shared<JsonSink>();
    auto serial = std::make_unique<MockSerialPort>();

    // Create wheel instances. These wheels must be also present in real life.
    // Moreover, Wheel ID must be aligned with the DataSink.
    auto leftFrontWheel = std::make_shared<Wheel>(0, sink);
    auto leftRearWheel = std::make_shared<Wheel>(1, sink);
    auto rightFrontWheel = std::make_shared<Wheel>(2, sink);
    auto rightRearWheel = std::make_shared<Wheel>(3, sink);

    // Create a model of the chassis.
    auto chassis = std::make_unique<model::Chassis<Wheel, wheelCount>>();
    chassis->addWheel(leftRearWheel);
    chassis->addWheel(leftFrontWheel);
    chassis->addWheel(rightFrontWheel);
    chassis->addWheel(rightRearWheel);

    // Create a command executor. It dispatches commands received from the client
    // app
    auto executor = std::make_shared<SequentialCommandExecutor>();
    executor->setNotifier([&chassis, &sink, &serial](std::vector<WheelSendMessage> &&message)
    {
        chassis->notify(std::move(message));
        // This is a must because each wheel receives a command and sends a correct value
        // into the JsonSink. There, JsonSink has all data that must be sent into a single
        // microcontroller. Of course, this action may be deferred in time but it must
        // be done otherwise no command will be issued to the microcontroller.
        serial->write(sink->toString());
    });

    // Retrieves a visitor responsible for receivng external commands.
    auto visitor = std::make_unique<rpc::GrpcChassisController>(wheelCount, executor);

    // This command shall be sent by a client application
    rpc::svc::PwmDriveCommand pwm;
    pwm.set_lpwm(666);
    pwm.set_rpwm(-44);

    // This command accepts a command from the remote application.
    visitor->accept(pwm);

    // Usually it is a good habit to inform the hardware that there are
    // new commands to execute.
    executor->exec();

    BOOST_TEST(contains(serial->getData(), R"("0":{"PWM":"666")"));
    BOOST_TEST(contains(serial->getData(), R"("1":{"PWM":"666")"));
    BOOST_TEST(contains(serial->getData(), R"("2":{"PWM":"-44")"));
    BOOST_TEST(contains(serial->getData(), R"("3":{"PWM":"-44")"));
}
