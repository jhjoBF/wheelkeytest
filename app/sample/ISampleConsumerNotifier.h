#pragma once
#include <vector>

class ISampleProducerListener;
class ISampleConsumerNotifier
{
public:
    ISampleConsumerNotifier(){}
    virtual ~ISampleConsumerNotifier(){}
public:
    virtual void addListener(ISampleProducerListener* listener) = 0;
};
