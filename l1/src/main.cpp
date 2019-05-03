#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/post.hpp>
#include <unordered_map>
#include <iostream>
#include <vector>

#include "serialdevicefinder.h"
#include "multipleucchassismodel.h"
#include "singleucchassismodel.h"
#include "grpcserver.h"

auto detect_serials()
{
    serial::SerialDeviceFinder finder;
    auto mapped_serials = finder.map();
    for (auto &serial : mapped_serials)
    {
        BOOST_LOG_TRIVIAL(info) << "Detected ID=" << serial.first
                                << " @ port=" << serial.second.getPortName();
    }

    return mapped_serials;
}

constexpr bool single_uc()
{
#ifdef SINGLE_UC
    return true;
#else
    return false;
#endif
}

class App
{
    static constexpr int HEALTH_CHECK_TIMEOUT {5};
    boost::asio::io_context &io_ctx;
    boost::asio::signal_set sig;
    int exit_code;
    std::shared_ptr<ChassisModel> model;
    std::unique_ptr<GrpcServer> grpc_server;
    boost::asio::steady_timer check_health_timer;
public:
    App(boost::asio::io_context &ctx)
        : io_ctx{ctx},
          sig{io_ctx, SIGINT, SIGTERM},
          exit_code{0},
          model{single_uc()
                ? std::shared_ptr<ChassisModel>(
                      new SingleUcChassisModel(io_ctx, detect_serials()))
                : std::shared_ptr<ChassisModel>(
                      new MultipleUcChassisModel(io_ctx, detect_serials()))},
          grpc_server{std::make_unique<GrpcServer>(model)},
          check_health_timer{io_ctx, boost::asio::chrono::seconds{HEALTH_CHECK_TIMEOUT}}
    {
        sig.async_wait(std::bind(&App::handle_signal, this,
                                 std::placeholders::_1, std::placeholders::_2));
    }

    int run()
    {
        check_health_timer.async_wait(std::bind(&App::check_health,
                                                this, std::placeholders::_1));
        io_ctx.run();
        return exit_code;
    }

private:
    void handle_signal(const boost::system::error_code &err, int sig_num)
    {
        if (!err)
        {
            BOOST_LOG_TRIVIAL(error) << "Signal received: " << sig_num
                                     << ", " << err;
            io_ctx.stop();
            exit_code = sig_num;
            BOOST_LOG_TRIVIAL(info) << "IO Context has been stopped.";
        }
    }

    void check_health(const boost::system::error_code &)
    {
        if (model->checkSerialHealth())
        {
            BOOST_LOG_TRIVIAL(info) << "Serial health: OK";
        }
        else
        {
            BOOST_LOG_TRIVIAL(info) << "Serial health: NOT OK";
            // An initial try to implement hot-plug of serials
//            try
//            {
//                grpc_server->kill();
//                model.reset(); grpc_server.reset();
//                model = std::make_shared<ChassisModel>(io_ctx, detect_serials());
//                grpc_server = std::make_unique<GrpcServer>(model);
//            }
//            catch (std::runtime_error &e)
//            {
//                BOOST_LOG_TRIVIAL(error) << "Cannot reestablish serial connection due to: "
//                                         << e.what();
//            }
        }
        check_health_timer.expires_at(check_health_timer.expires_at()
                                      + boost::asio::chrono::seconds(HEALTH_CHECK_TIMEOUT));

        check_health_timer.async_wait(std::bind(&App::check_health,
                                                this, std::placeholders::_1));
    }
};

constexpr int App::HEALTH_CHECK_TIMEOUT;

int main()
{
    boost::asio::io_context io_ctx;
    try
    {
        App app{io_ctx};
        return app.run();
    }
    catch (std::runtime_error &e)
    {
        BOOST_LOG_TRIVIAL(error) << e.what();
        BOOST_LOG_TRIVIAL(error) << "Abort the application.";
        return -1;
    }
}
