#ifndef ITHREAD_H_
#define ITHREAD_H_

class IThread {
public:
    IThread() {};
    virtual ~IThread() {};
public:
    virtual void start() = 0;
    virtual bool isRunning() = 0;
    virtual void stop() = 0;
    virtual bool join() = 0;
};

#endif  //ITHREAD_H_
