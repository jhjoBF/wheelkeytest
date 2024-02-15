#include <chrono>
#include "SampleProducer.h"

SampleProducer::SampleProducer()
: AbstractThread(300)
{
}
SampleProducer::~SampleProducer()
{
}
void SampleProducer::worker()
{
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now(); 
    auto diff = now - start;
    int sec = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    if(sec >= 5) {
        start = now;
        printf("%s, %s, %d !!\n", __FILE__, __func__, __LINE__);
        for(const auto& listener: _listeners)
            listener->update(sec);
    }
}   
void SampleProducer::addListener(ISampleProducerListener* listener)
{
   _listeners.push_back(listener); 
}
