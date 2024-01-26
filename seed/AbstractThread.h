#ifndef ABSTRACTTHREAD_H_
#define ABSTRACTTHREAD_H_

#include <thread>
#include <mutex>
#include "IThread.h"

class AbstractThread : public IThread {
public:
    AbstractThread(int milliSeconds = 300);
    virtual ~AbstractThread();
private:
    std::thread _threadObject;
protected:
    int _defaultSeconds;
    int _milliSeconds;
    std::mutex  _mutex;
    bool _stop;
private:
    void loopingThread();
protected:
    void run();
    void setSleepTime(int milliSeconds) { _milliSeconds = milliSeconds; };
    virtual void worker() = 0;
    virtual void preRun() {}
    virtual void postRun() {}
public:
    void start();
    bool isRunning();
    void stop();
    bool join();
public:
    virtual void init() {}
};

#endif  // ABSTRACTTHREAD_H_
