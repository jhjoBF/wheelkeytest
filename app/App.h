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
    void initialize(int startMapIndex);     // 시작 맵 인덱스 지정
    void initializePosition(int position);  // 포지션 모드
    void initializeStop();                  // 정지 모드
    void initializeReboot();                // 재부팅 모드
    void initializeMonitor();               // 모니터 모드
    void finalize();
    void forceRun();
private:
    void makeThreads();
    void makeThreads(int startMapIndex);    // 시작 맵 인덱스 지정
    void makeThreadsPosition(int position);
    void makeThreadsStop();
    void makeThreadsReboot();
    void makeThreadsMonitor();
    void startThreads();
};
#endif  // APP_H_
