#pragma once
#include "AbstractThreadCondition.h"
#include "SampleProducer.h"

class ISampleConsumerNotifier;
class SampleConsumer: public AbstractThreadCondition , public ISampleProducerListener {
public:
    SampleConsumer(ISampleConsumerNotifier* notifier);
    virtual ~SampleConsumer();
  
private:
    int _data;
protected:
    void worker();
public://ISampleProducerListener
    void update(int data);
};

