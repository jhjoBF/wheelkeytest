#include <SampleStateMachine.h>

SampleStateMachine::SampleStateMachine()
:AbstractThread(300)
{
}
SampleStateMachine::~SampleStateMachine()
{
}
void SampleStateMachine::applyCmd()
{
    std::unique_lock<std::mutex> m(_mutex);
    int cmd  = None;
    if(!_queue.empty()) {
        cmd  = _queue.front();
        _queue.pop();
        printf("%s, %s, %d cmd %d !!\n", __FILE__, __func__, __LINE__, cmd);
        if(cmd == RunCmd){
            _state = RunState0;
            setSleepTime(10);
        }
        else if(cmd == StopCmd){
            _state = StopState;
        }
    }
}
void SampleStateMachine::worker()
{
    setSleepTime(_defaultSeconds);
    if(_state != IdleState) 
        printf("%s, %s, %d state %d!!\n", __FILE__, __func__, __LINE__,_state);
    
    if(_state == IdleState) { 
        applyCmd();
        return;
    }
    else if(_state == RunState0) {
        //execute Run0
        _state = RunState1;
        setSleepTime(10);
        return;
    }
    else if(_state == RunState1) {
        //execute Run1
        _state = RunState2;
        setSleepTime(10);
        return;
    }
    else if(_state == RunState2) {
        //execute Run2
        _state = RunState3;
        setSleepTime(10);
        return;
    }
    else if(_state == RunState3) {
        //execute Run3
        _state = IdleState;
        setSleepTime(_defaultSeconds);
        return;
    }
    else if(_state == StopState) {
        stop();
        setSleepTime(10);
        return;
    }
}   

void SampleStateMachine::update(int data)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(RunCmd);
        _data = data;
    }
} 
