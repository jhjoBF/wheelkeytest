#ifndef ABSTRACTTHREADCONDITION_H_
#define ABSTRACTTHREADCONDITION_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include "IThread.h"

class AbstractThreadCondition : public IThread {
public:
    AbstractThreadCondition(int milliSeconds = 0);
    virtual ~AbstractThreadCondition();
private:
    std::thread _threadObject;
    const int _milliSeconds;
    std::condition_variable _cv;
protected:
    std::mutex  _mutex;
    bool _stop;
    bool _wakeUp;
private:
    void loopingThread();
protected:
    void wakeUp();
    void run();
    virtual void worker() = 0;
    virtual void preRun() {}
    virtual void postRun() {}
public: //IThread
    void start();
    bool isRunning();
    void stop();
    bool join();
};

#endif  // ABSTRACTTHREADCONDITION_H_
