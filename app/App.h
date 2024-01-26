#ifndef APP_H_
#define APP_H_
#include <vector>
#include "AbstractThread.h"

class App : public AbstractThread {
public:
    App();
    virtual ~App();
private:
    std::vector<IThread*> _threads;
protected:
    void worker();
public:
    void initialize();
    void finalize();
    void forceRun();
private:
    void makeThreads();
    void startThreads();
};
#endif  // APP_H_
