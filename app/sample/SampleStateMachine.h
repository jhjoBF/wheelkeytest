#pragma once
#include <queue>
#include "AbstractThread.h"
#include "SampleProducer.h"

class SampleStateMachine: public AbstractThread, public ISampleProducerListener {
public:
    SampleStateMachine();
    virtual ~SampleStateMachine();
  
private:
    int _state;
    int _data;
    std::queue<int> _queue;
    
    enum {
        RunCmd = 0,
        StopCmd,
        None = 99
    };
    
    enum {
        IdleState = 0,
        RunState0,
        RunState1,
        RunState2,
        RunState3,
        StopState,
    };
protected:
    void worker();
public://ISampleProducerListener
    void update(int data);
private:
    void applyCmd();
};

