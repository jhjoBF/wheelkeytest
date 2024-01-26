#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_
#include "AbstractThread.h"

class MainThread : public AbstractThread {
public:
    MainThread();
    virtual ~MainThread();
protected:
    void worker();
    void preRun();
    void postRun();
private:
    //uint32_t _id;
public:
    //void test();
};
#endif  // MAIN_THREAD_H_
