#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <execinfo.h>
#include "App.h"

App* gApp = nullptr;

void callTrace() {
    const uint32_t MaxSize = 32;
    void* array[MaxSize];
    size_t size;
    char** strings;
    size = ::backtrace(array, MaxSize);
    strings = ::backtrace_symbols(array, size);

    for (size_t i = 0; i < size; i++) {
    //for (size_t i = size - 1; i >= 2; i--) {
        fprintf(stderr, "[%2d] : %s\n", i, strings[i]);
    }
    free(strings);
}

void signalHander(int signal) {
    printf("%s %s %d, signal %d\n", __FILE__, __func__, __LINE__, signal);
    if ((signal != SIGTERM) && (signal != SIGINT)) {
        callTrace();
        SIG_DFL(signal);
        //exit(1);
    }
    if (gApp != nullptr) gApp->stop();
}

void initSignal() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, signalHander);
    signal(SIGINT, signalHander);
    signal(SIGSEGV, signalHander);
}

int main() {
    std::vector<std::string> msg {"Hello", "World", "!"};
    for (const std::string& word : msg)
        std::cout << word << " ";
    std::cout << std::endl;

    initSignal();
    App app;
    gApp = &app;
    app.initialize();
    app.forceRun();
    app.finalize();
    app.join();
    return 0;
}
