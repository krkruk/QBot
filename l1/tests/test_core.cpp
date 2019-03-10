#define BOOST_TEST_MODULE test_core
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <iostream>
#include <string>

#include "jsondispatcherprocessor.h"
#include "wheelsendmessage.h"
#include "wheelrecvmessage.h"
#include "serialport.h"
#include "dispatcher.h"


template<typename T>
bool contains(std::string root, T value)
{
    return root.find(value) != std::string::npos;
}

BOOST_AUTO_TEST_CASE(create_send_message)
{
    WheelSendMessage msg {
        WheelSendMessage::Builder(1)
                .setPwm(255)
                .setAngularVelocity(10)
                .setProportional(0.2)
                .setIntegral(1.0)
                .setDifferential(1.0)
                .build()
    };
    std::string raw {msg.toString()};

    BOOST_TEST(contains(raw, R"("1":)"));
    BOOST_TEST(contains(raw, R"("PWM":"255")"));
    BOOST_TEST(contains(raw, R"("ROT":"10.00")"));
    BOOST_TEST(contains(raw, R"("KP":"0.20")"));
    BOOST_TEST(contains(raw, R"("KI":"1.00")"));
    BOOST_TEST(contains(raw, R"("KD":"1.00")"));
}


BOOST_AUTO_TEST_CASE(parse_received_message)
{
    std::string json{
        R"({"1":{"ROT":20.0,"CUR":1260,"TMP":26.0,"PWM":-200,"ERR":0}})"
    };
    WheelRecvMessage msg {WheelRecvMessage::fromRaw(1, json)};

    BOOST_CHECK_EQUAL(1, msg.getId());
    BOOST_CHECK_EQUAL(20.0, msg.getAngularVelocity());
    BOOST_CHECK_EQUAL(1260, msg.getMilliamps());
    BOOST_CHECK_EQUAL(26.0, msg.getCelsius());
    BOOST_CHECK_EQUAL(-200, msg.getPwm());
    BOOST_CHECK_EQUAL(0, msg.getErrorCode());
}

BOOST_AUTO_TEST_CASE(test_int_dispatcher)
{
    constexpr int expected = 20;
    Dispatcher<int> dispatcher;
    int value{};
    dispatcher.addObserver([&value](int expected) { value = expected; });
    dispatcher.dispatch(expected);
    dispatcher.notify();
    BOOST_CHECK_EQUAL(expected, value);
}

BOOST_AUTO_TEST_CASE(test_pack_dispatcher)
{
    const std::string expected_string {"hello world"};
    constexpr int expected_int = 20;
    Dispatcher<std::string, std::size_t> dispatcher;
    std::string value_string{};
    std::size_t value_int{};
    dispatcher.addObserver([&value_string, &value_int](auto text, auto number)
        { value_string = text; value_int = number; });
    dispatcher.dispatch(expected_string, expected_int);
    dispatcher.notify();
    BOOST_CHECK_EQUAL(expected_string, value_string);
    BOOST_CHECK_EQUAL(expected_int, value_int);
}

BOOST_AUTO_TEST_CASE(test_dispatching_json)
{
    int identifier{};
    std::string json_received;

    const std::string json{R"({"1":{"ROT":"20.0","CUR":"1260","TMP":"26.0","PWM":"-200","ERR":"0"}})"};
    auto callback = [&identifier, &json_received](int id, std::string data){identifier = id;json_received = data;};
    MappedDispatcher<std::string,
        JsonDispatcherProcessor<int, std::string>> dispatcher;

    dispatcher.addObserver(1, callback);
    dispatcher.dispatch(json);
    dispatcher.notify();

    boost::property_tree::ptree ptree_data;
    std::stringstream ss(json_received);
    boost::property_tree::json_parser::read_json(ss, ptree_data);

    BOOST_CHECK_EQUAL(1, identifier);
    BOOST_TEST(20.0 == ptree_data.get<double>("ROT"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(26.0 == ptree_data.get<double>("TMP"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(1260 == ptree_data.get<int>("CUR"));
    BOOST_TEST(-200 == ptree_data.get<int>("PWM"));
}

BOOST_AUTO_TEST_CASE(test_dispatching_json_no_convert)
{
    using ptree = boost::property_tree::ptree;
    int identifier{};
    ptree ptree_data;

    const std::string json{R"({"1":{"ROT":"20.0","CUR":"1260","TMP":"26.0","PWM":"-200","ERR":"0"}})"};
    auto callback = [&identifier, &ptree_data](int id, ptree data){identifier = id; ptree_data = data;};

    MappedDispatcher<std::string,
        JsonDispatcherProcessor<int, boost::property_tree::ptree>> dispatcher;

    dispatcher.addObserver(1, callback);
    dispatcher.dispatch(json);
    dispatcher.notify();

    BOOST_CHECK_EQUAL(1, identifier);
    BOOST_TEST(20.0 == ptree_data.get<double>("ROT"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(26.0 == ptree_data.get<double>("TMP"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(1260 == ptree_data.get<int>("CUR"));
    BOOST_TEST(-200 == ptree_data.get<int>("PWM"));
}


BOOST_AUTO_TEST_CASE(test_dispatching_multiple_observers)
{
    using ptree = boost::property_tree::ptree;
    int id1, id2, id3 {};
    ptree pdata1, pdata2, pdata3;

    const std::string json{R"({"1":{"ROT":"20.0"},"2":{"ROT":"-30.0"},"3":{"ROT":"66.6"}})"};
    auto callback1 = [&id1, &pdata1](int id, ptree data){id1 = id; pdata1 = data;};
    auto callback2 = [&id2, &pdata2](int id, ptree data){id2 = id; pdata2 = data;};
    auto callback3 = [&id3, &pdata3](int id, ptree data){id3 = id; pdata3 = data;};

    MappedDispatcher<std::string,
        JsonDispatcherProcessor<int, boost::property_tree::ptree>> dispatcher;

    dispatcher.addObserver(1, callback1);
    dispatcher.addObserver(2, callback2);
    dispatcher.addObserver(3, callback3);
    dispatcher.dispatch(json);
    dispatcher.notify();

    BOOST_CHECK_EQUAL(1, id1);
    BOOST_CHECK_EQUAL(2, id2);
    BOOST_CHECK_EQUAL(3, id3);
    BOOST_TEST(20.0 == pdata1.get<double>("ROT"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(-30.0 == pdata2.get<double>("ROT"), boost::test_tools::tolerance(0.01));
    BOOST_TEST(66.6 == pdata3.get<double>("ROT"), boost::test_tools::tolerance(0.01));
}
