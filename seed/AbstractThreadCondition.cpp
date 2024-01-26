#include "AbstractThreadCondition.h"

AbstractThreadCondition::AbstractThreadCondition(int milliSeconds)
    : _stop(true), _wakeUp(false),
      _milliSeconds(milliSeconds) {
}

AbstractThreadCondition::~AbstractThreadCondition() { }

void AbstractThreadCondition::run() {
    preRun();
    while (_stop == false) {
        std::unique_lock<std::mutex> m(_mutex);
        _cv.wait(m, [&]{ return _wakeUp;});
        _wakeUp = false;
        m.unlock();
        worker();
        if (_milliSeconds)
            std::this_thread::sleep_for(std::chrono::milliseconds(_milliSeconds));
    }
    postRun();
}

void AbstractThreadCondition::loopingThread() {
    run();
}

void AbstractThreadCondition::start() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_threadObject.joinable() == false) {
        _stop = false;
        _threadObject = std::thread(&AbstractThreadCondition::loopingThread, this);
    }
}

void AbstractThreadCondition::stop() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stop = true;
    }
    wakeUp();
}

void AbstractThreadCondition::wakeUp() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _wakeUp = true;
    }
    _cv.notify_one();
}

bool AbstractThreadCondition::join() {
    if (_threadObject.joinable() == true) {
        printf("%s, %s, %d !!\n", __FILE__, __func__, __LINE__);
        _threadObject.join();
        return true;
    }
    return false;
}

bool AbstractThreadCondition::isRunning() {
    return _threadObject.joinable();
}
