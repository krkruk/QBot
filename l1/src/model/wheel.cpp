#include "wheel.h"
#include <boost/log/trivial.hpp>


template<typename DataSink>
model::Wheel<DataSink>::Wheel(int id, std::weak_ptr<DataSink> sink)
    : id{id}, sink{sink}
{ }

template<typename DataSink>
int model::Wheel<DataSink>::getId() const
{
    return id;
}

template<typename DataSink>
WheelSendMessage::Builder model::Wheel<DataSink>::generateMessage() const
{
    return WheelSendMessage::Builder(id);
}


template<typename DataSink>
template<typename Message>
void model::Wheel<DataSink>::sendMessage(const Message &msg)
{
    if (auto sh_sink = sink.lock())
    {
        if (*sh_sink)
        {
            sh_sink->write(msg);
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot write data. The data sink is unavailable.";
        }
    }
}
