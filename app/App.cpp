#ifdef _WIN32
    #include <windows.h>
    #define usleep(x) Sleep((x)/1000)
#else
    #include <unistd.h>
#endif

#include <cstring>
#include <cstdio>
#include "App.h"
#include "MainThread.h"

App::App() : AbstractThread(300) {
}
App::~App() {}

#define CHAIR_COUNT 3       // 갯수
#define CHAIR_IP_START 20   // 시작주소 192.168.0.x, x+1, x+2, ...

void App::makeThreads() {
    MainThread* mainThread = new MainThread();
    // Chair IP 설정 (192.168.0.10 ~ 192.168.0.19)
    for (int i = 0; i < CHAIR_COUNT; i++) {
        char ip[20];
        sprintf(ip, "192.168.0.%d", CHAIR_IP_START + i);
        mainThread->setChairIp(i, ip);
    }
    // Chair 개수 설정
    mainThread->setChairCount(CHAIR_COUNT);

    // Map 추가 (외부에 정의된 Map 배열 사용)
    extern Maptype Map[];
    extern const int MapCount;
    for (int i = 0; i < MapCount; i++) {
        mainThread->addMap(Map[i]);
    }

    // Map 시퀀스 시작
    mainThread->startMapSequence();

    _threads.push_back(mainThread);
}

void App::makeThreads(int startMapIndex) {
    MainThread* mainThread = new MainThread();
    // Chair IP 설정 (192.168.0.10 ~ 192.168.0.19)
    for (int i = 0; i < CHAIR_COUNT; i++) {
        char ip[20];
        sprintf(ip, "192.168.0.%d", CHAIR_IP_START + i);
        mainThread->setChairIp(i, ip);
    }
    // Chair 개수 설정
    mainThread->setChairCount(CHAIR_COUNT);

    // Map 추가 (외부에 정의된 Map 배열 사용)
    extern Maptype Map[];
    extern const int MapCount;
    for (int i = 0; i < MapCount; i++) {
        mainThread->addMap(Map[i]);
    }

    // 시작 맵 인덱스 설정
    mainThread->setStartMapIndex(startMapIndex);

    // Map 시퀀스 시작
    mainThread->startMapSequence();

    _threads.push_back(mainThread);
}

void App::startThreads() {
    for (const auto& thread : _threads)
        thread->start();
}

void App::initialize() {
    makeThreads();
    startThreads();
}

void App::initialize(int startMapIndex) {
    makeThreads(startMapIndex);
    startThreads();
}

void App::initializePosition(int position) {
    makeThreadsPosition(position);
    startThreads();
}

void App::initializeMonitor() {
    makeThreadsMonitor();
    startThreads();
}

void App::initializeStop() {
    makeThreadsStop();
    startThreads();
}

void App::makeThreadsPosition(int position) {
    MainThread* mainThread = new MainThread();
    // Chair IP 설정 (192.168.0.10 ~ 192.168.0.19)
    for (int i = 0; i < CHAIR_COUNT; i++) {
        char ip[20];
        sprintf(ip, "192.168.0.%d", CHAIR_IP_START + i);
        mainThread->setChairIp(i, ip);
    }
    // Chair 개수 설정
    mainThread->setChairCount(CHAIR_COUNT);
    // 포지션 모드 설정
    mainThread->setPositionMode(position);
    _threads.push_back(mainThread);
}

void App::makeThreadsStop() {
    MainThread* mainThread = new MainThread();
    // Chair IP 설정 (192.168.0.10 ~ 192.168.0.19)
    for (int i = 0; i < CHAIR_COUNT; i++) {
        char ip[20];
        sprintf(ip, "192.168.0.%d", CHAIR_IP_START + i);
        mainThread->setChairIp(i, ip);
    }
    // Chair 개수 설정
    mainThread->setChairCount(CHAIR_COUNT);
    // 정지 모드 설정
    mainThread->setStopMode(true);
    _threads.push_back(mainThread);
}

void App::makeThreadsMonitor() {
    MainThread* mainThread = new MainThread();
    // Chair IP 설정 (192.168.0.10 ~ 192.168.0.19)
    for (int i = 0; i < CHAIR_COUNT; i++) {
        char ip[20];
        sprintf(ip, "192.168.0.%d", CHAIR_IP_START + i);
        mainThread->setChairIp(i, ip);
    }
    // Chair 개수 설정
    mainThread->setChairCount(CHAIR_COUNT);
    // 모니터 모드 설정
    mainThread->setMonitorMode(true);
    _threads.push_back(mainThread);
}

void App::finalize() {
    for (const auto& thread : _threads)
        thread->stop();
    usleep(30 * 1000);
    for (const auto& thread : _threads)
        thread->join();
    for (const auto& thread : _threads)
        delete thread;
}

void App::worker() {
    ;
}

void App::forceRun() {
    _stop = false;
    AbstractThread::run();
}
