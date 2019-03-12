#include "wheel.h"


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
        sh_sink->write(msg);
    }
}