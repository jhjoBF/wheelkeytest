#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>
#include <iomanip>
#include <cstring>
#include <time.h>

#define DEBUG_BUF_SIZE 256
#define FILE_NAME_LEN  16
#define FUNC_NAME_LEN  16
#define LINE_NUM_LEN   5

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#if CONFIG_DEBUG
#define _N_ "\033[0m"    //normal
#define _R_ "\033[1;31m" //red
#define _G_ "\033[1;32m" //green
#define _Y_ "\033[1;33m" //yellow
#define _B_ "\033[1;34m" //blue
#define _P_ "\033[1;35m" //purple
#define _C_ "\033[1;36m" //cyan
#define _GRAY_ "\033[0;37m"
#define _DG_ "\033[32m" //darkgreen
#else
#define _N_
#define _R_
#define _G_
#define _Y_
#define _B_
#define _P_
#define _C_
#define _GRAY_
#endif

#if CONFIG_DEBUG
#define SHOW_TIME {\
    char str[DEBUG_BUF_SIZE]; std::time_t rawtime; std::tm* timeinfo;\
    time(&rawtime); timeinfo = std::localtime(&rawtime);\
    std::strftime(str, 80, "[%m/%d %H:%M:%S] ", timeinfo);\
    std::cout << _DG_ << str;\
}

#define SHOW_TICK {\
    char str[DEBUG_BUF_SIZE]; struct timespec tick; clock_gettime(CLOCK_MONOTONIC, &tick);\
    sprintf(str, "[%5lu.%06lu] ", tick.tv_sec, tick.tv_nsec / 1000);\
    std::cout << _DG_ << str;\
}

#define SHOW_FILE_NAME {\
    char str[DEBUG_BUF_SIZE]; sprintf(str, "%s", basename(__FILE__)); str[FILE_NAME_LEN] = 0;\
    std::cout << std::left << _G_ << std::setw(FILE_NAME_LEN) << str;\
}
#define SHOW_FUNC_NAME {\
    char str[DEBUG_BUF_SIZE]; sprintf(str, "%s():", __func__); str[FUNC_NAME_LEN] = 0;\
    std::cout << std::right << _Y_ << std::setw(FUNC_NAME_LEN) << str;\
}
#define SHOW_LINE std::cout << std::setw(LINE_NUM_LEN) << __LINE__ << ": ";

#define DEBUG(...) {\
    char strDbg[DEBUG_BUF_SIZE];\
    snprintf(strDbg, sizeof(strDbg) - 1, __VA_ARGS__);\
    SHOW_TIME\
    SHOW_TICK\
    SHOW_FILE_NAME\
    SHOW_FUNC_NAME\
    SHOW_LINE\
    std::cout << std::left << _N_ << strDbg << _N_;\
    if (strDbg[strlen(strDbg) - 1] != '\n') std::cout << std::endl;\
}
#else
#define DEBUG(...)
#endif

#define LFCR_TO_SPACE(str) {\
    int len, i;\
    len = strlen(str);\
    for (i = 0; i < len; i++) if (str[i] == '\r' || str[i] == '\n') str[i] = ' ';\
}

#endif  // DEBUG_H_
