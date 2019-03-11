#include "wheel.h"


template<typename DataSink>
model::Wheel<DataSink>::Wheel(std::weak_ptr<DataSink> sink)
    : sink{sink}
{ }


template<typename DataSink>
template<typename Message>
void model::Wheel<DataSink>::sendMessage(const Message &msg)
{
    if (auto sh_sink = sink.lock())
    {
        sh_sink->write(msg);
    }
}
