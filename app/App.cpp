#include <unistd.h>
#include "App.h"
#include "MainThread.h"

App::App() : AbstractThread(300) {
    printf("%s, %s, %d #### Start App\n", __FILE__, __func__, __LINE__);
}
App::~App() {}

void App::makeThreads() {
    _threads.push_back(new MainThread());
}

void App::startThreads() {
    for (const auto& thread : _threads)
        thread->start();
}

void App::initialize() {
    makeThreads();
    startThreads();
}

void App::finalize() {
    for (const auto& thread : _threads)
        thread->stop();
    usleep(30 * 1000);
    for (const auto& thread : _threads)
        thread->join();
    for (const auto& thread : _threads)
        delete thread;
    printf("%s, %s, %d #### Finish App\n", __FILE__, __func__, __LINE__);
}

void App::worker() {
    ;
}

void App::forceRun() {
    _stop = false;
    AbstractThread::run();
}
