#pragma once
#include <queue>
#include "AbstractThreadCondition.h"

template <typename T> 
class SampleThreadConditionT: public AbstractThreadCondition {
public:
    SampleThreadConditionT();
    virtual ~SampleThreadConditionT();
  
private:
   T _data; 
   std::queue<T> _queue;
protected:
    void worker();
public:
    void setData(int data);
};

#include "SampleThreadConditionT.hpp"
