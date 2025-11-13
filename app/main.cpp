#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
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
        fprintf(stderr, "[%2d] : %s\n", (int)i, strings[i]);
    }
    free(strings);
}

void signalHander(int signal) {
    printf("%s %s %d, signal %d\n", __FILENAME__, __func__, __LINE__, signal);
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

void printUsage(const char* programName) {
    printf("Usage: %s [OPTIONS]\n", programName);
    printf("Options:\n");
    printf("  (no option)    : Run map sequence (default)\n");
    printf("  -p <position>  : Send position command once to all chairs\n");
    printf("  -s             : Send stop command once to all chairs\n");
    printf("  -m             : Monitor mode (info only, no map sending)\n");
    printf("\nExamples:\n");
    printf("  %s           # Run map sequence\n", programName);
    printf("  %s -p 1      # Send position 1 to all chairs\n", programName);
    printf("  %s -s        # Send stop command to all chairs\n", programName);
    printf("  %s -m        # Monitor mode\n", programName);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> msg {"Hello", "world"};
    for (const std::string& word : msg)
        std::cout << word << " ";
    std::cout << std::endl;

    // 옵션 파싱
    enum class RunMode {
        MAP_SEQUENCE,   // 기본: 맵 시퀀스 실행
        POSITION,       // -p: 포지션 명령 전송
        STOP,           // -s: 정지 명령 전송
        MONITOR         // -m: 모니터링만
    };
    
    RunMode mode = RunMode::MAP_SEQUENCE;
    int position = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                position = atoi(argv[i + 1]);
                mode = RunMode::POSITION;
                i++;
            } else {
                printf("Error: -p requires a position number\n");
                printUsage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-s") == 0) {
            mode = RunMode::STOP;
        } else if (strcmp(argv[i], "-m") == 0) {
            mode = RunMode::MONITOR;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else {
            printf("Error: Unknown option '%s'\n", argv[i]);
            printUsage(argv[0]);
            return 1;
        }
    }

    initSignal();
    App app;
    gApp = &app;
    
    if (mode == RunMode::MAP_SEQUENCE) {
        printf("[INFO] Running in MAP_SEQUENCE mode\n");
        app.initialize();
    } else if (mode == RunMode::POSITION) {
        printf("[INFO] Running in POSITION mode (position=%d)\n", position);
        app.initializePosition(position);
    } else if (mode == RunMode::STOP) {
        printf("[INFO] Running in STOP mode\n");
        app.initializeStop();
    } else if (mode == RunMode::MONITOR) {
        printf("[INFO] Running in MONITOR mode\n");
        app.initializeMonitor();
    }
    
    app.forceRun();
    app.finalize();
    app.join();
    return 0;
}
