#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>

#include "serialport.h"
#include "serialportinfo.h"

#include "chassisserviceimpl.h"

#include <boost/asio.hpp>
#include <future>

template<typename IdRetrievalAlgorithm>
class SerialDeviceFinder
{
    using result_type = std::pair<std::string, serial::PortInfo>;
    IdRetrievalAlgorithm retrieveId;
public:
    explicit SerialDeviceFinder(IdRetrievalAlgorithm alg)
        : retrieveId{alg}
    {}

    std::vector<result_type> list()
    {
        auto ports = serial::SerialPortInfo::listSerials();
        using port_future = std::future<result_type>;
        std::vector<port_future> futures;
        for (const auto &port : ports)
        {
            futures.push_back(std::async(std::launch::async, &SerialDeviceFinder::retrieve_id, this, std::ref(port)));
        }

        for (auto &future : futures)
        {
            auto result = future.get();
            std::cout << "result --> id: " << result.first << " port: " << result.second.getPortName() << std::endl;
        }
        return {};
    }

private:
    result_type retrieve_id(const serial::PortInfo &port)
    {
        using namespace boost::asio;
        io_context ctx;

        boost::asio::serial_port serial{ctx, port.getPortName()};
        serial.set_option(serial_port_base::baud_rate(115200));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::character_size(8));

        streambuf message;
        read_until(serial, message, '\n');
        std::istream in(&message);
        std::string data;
        std::getline(in, data);
        return {retrieveId(data), port};
    }
};

int main()
{
    int exit_code {0};
    auto retrieveId = [](const std::string &line) { return line; };
    SerialDeviceFinder<decltype(retrieveId)> finder(retrieveId);
    finder.list();

//    auto ports = serial::SerialPortInfo::listSerials();
//    for (auto &path : ports)
//    {
//        std::cout << path.getPortName()
//                  << std::endl;
//    }

//    boost::asio::io_context ctx;
//    serial::SerialPort<serial::ReadLineAlgorithm<serial::PrintContent>>
//            port{ctx, ports[0], serial::PrintContent{}};
//    boost::asio::signal_set sig{ctx, SIGINT, SIGTERM};
//    sig.async_wait([&ctx, &exit_code](const auto &err, int sig_num)
//    {
//        if (!err)
//        {
//            std::cerr << "Error: " << sig_num << std::endl;
//            ctx.stop();
//            exit_code = sig_num;
//        }
//    });
//    ctx.run();
//    std::cout << "Hello World!" << std::endl;
//    ChassisServiceImpl service;

//    grpc::ServerBuilder builder;
//    builder.AddListeningPort("0.0.0.0:5000", grpc::InsecureServerCredentials())
//            .RegisterService(&service);
//    auto server = std::unique_ptr<grpc::Server>(builder.BuildAndStart());

//    server->Wait();

    return exit_code;
}
