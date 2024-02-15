#include "SampleConsumerQueue.h"
#include "ISampleConsumerNotifier.h"

SampleConsumerQueue::SampleConsumerQueue(ISampleConsumerNotifier* notifier)
:AbstractThreadCondition(0)
{
    notifier->addListener(this);
}
SampleConsumerQueue::~SampleConsumerQueue()
{
}
void SampleConsumerQueue::worker()
{
    int data  = 0;
    {
        std::unique_lock<std::mutex> m(_mutex);
        if(!_queue.empty()) {
            data  = _queue.front();
            _queue.pop();
        }
    }
    printf("%s, %s, %d data %d !!\n", __FILE__, __func__, __LINE__, data);
}   

void SampleConsumerQueue::update(int data)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(data); 
    }
    wakeUp();
} 
