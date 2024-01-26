#include "MainThread.h"

MainThread::MainThread() : AbstractThread(300) {
    printf("%s, %s, %d Create\n", __FILE__, __func__, __LINE__);
}
MainThread::~MainThread() {}

void MainThread::preRun() {
    printf("%s, %s, %d\n", __FILE__, __func__, __LINE__);
}

void MainThread::postRun() {
    printf("%s, %s, %d\n", __FILE__, __func__, __LINE__);
}

void MainThread::worker() {
    ;
}
