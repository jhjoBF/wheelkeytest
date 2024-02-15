#include <SampleConsumer.h>

SampleConsumer::SampleConsumer(ISampleConsumerNotifier* notifier)
:AbstractThreadCondition(0)
{
    notifier->addListener(this); 
}
SampleConsumer::~SampleConsumer()
{
}
void SampleConsumer::worker()
{
    printf("%s, %s, %d Data %d!!\n", __FILE__, __func__, __LINE__, _data);
}   

void SampleConsumer::update(int data)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _data += data;
    }
    wakeUp();
} 
