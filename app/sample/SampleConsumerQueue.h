#pragma once
#include <queue>
#include "AbstractThreadCondition.h"
#include "SampleProducer.h"

class ISampleConsumerNotifier;
class SampleConsumerQueue: public AbstractThreadCondition , public ISampleProducerListener {
public:
    SampleConsumerQueue(ISampleConsumerNotifier* notifier);
    virtual ~SampleConsumerQueue();
  
private:
    std::queue<int> _queue;
protected:
    void worker();
public://ISampleProducerListener
    void update(int data);
};

