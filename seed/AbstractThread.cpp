#include "AbstractThread.h"

AbstractThread::AbstractThread(int milliSeconds)
    : _stop(true), _milliSeconds(milliSeconds), _defaultSeconds(milliSeconds) {}

AbstractThread::~AbstractThread() {}

void AbstractThread::run() {
    preRun();
    while (_stop == false) {
        worker();
        if (_milliSeconds)
            std::this_thread::sleep_for(std::chrono::milliseconds(_milliSeconds));
    }
    postRun();
}

void AbstractThread::loopingThread() {
    run();
}

void AbstractThread::start() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_threadObject.joinable() == false) {
        _stop = false;
        _threadObject = std::thread(&AbstractThread::loopingThread, this);
    }
}

void AbstractThread::stop() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stop = true;
    }
}

bool AbstractThread::join() {
    if (_threadObject.joinable() == true) {
        printf("%s, %s, %d\n", __FILE__, __func__, __LINE__);
        _threadObject.join();
        return true;
    }
    return false;
}

bool AbstractThread::isRunning() {
    return _threadObject.joinable();
}
