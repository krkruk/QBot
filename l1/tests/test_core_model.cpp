#define BOOST_TEST_MODULE test_core_model
#include <boost/test/included/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <algorithm>
#include <string>
#include "serialport.h"
#include "wheelsendmessage.h"
#include "wheelrecvmessage.h"


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
    BOOST_TEST(contains(raw, R"("ROT":"10")"));
    BOOST_TEST(contains(raw, R"("KP":"0.20")"));
    BOOST_TEST(contains(raw, R"("KI":"1.00")"));
    BOOST_TEST(contains(raw, R"("KD":"1.00")"));
}


BOOST_AUTO_TEST_CASE(parse_received_message)
{
    std::string json{
        R"({"1":{"ROT":20,"CUR":1260,"TMP":26.0,"PWM":-200,"ERR":0}})"
    };
    WheelRecvMessage msg {WheelRecvMessage::fromRaw(1, json)};

    BOOST_CHECK_EQUAL(1, msg.getId());
    BOOST_CHECK_EQUAL(20, msg.getAngularVelocity());
    BOOST_CHECK_EQUAL(1260, msg.getMilliamps());
    BOOST_CHECK_EQUAL(26.0, msg.getCelsius());
    BOOST_CHECK_EQUAL(-200, msg.getPwm());
    BOOST_CHECK_EQUAL(0, msg.getErrorCode());
}
