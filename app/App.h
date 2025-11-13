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
    void initializePosition(int position);  // 포지션 모드
    void initializeStop();                  // 정지 모드
    void initializeMonitor();               // 모니터 모드
    void finalize();
    void forceRun();
private:
    void makeThreads();
    void makeThreadsPosition(int position);
    void makeThreadsStop();
    void makeThreadsMonitor();
    void startThreads();
};
#endif  // APP_H_
