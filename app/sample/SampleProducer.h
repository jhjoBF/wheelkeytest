#pragma once
#include "AbstractThread.h"
#include "ISampleConsumerNotifier.h"

class ISampleProducerListener
{
public:
    ISampleProducerListener(){}
    virtual ~ISampleProducerListener(){}
public:
    virtual void update(int data) = 0;
};
class SampleProducer: public AbstractThread, public ISampleConsumerNotifier {
public:
    SampleProducer();
    virtual ~SampleProducer();
protected:
    std::vector<ISampleProducerListener*> _listeners;
protected:
    void worker();
public:
    void addListener(ISampleProducerListener* listener);
};

