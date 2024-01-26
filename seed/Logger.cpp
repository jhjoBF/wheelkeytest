#include <stdio.h>
#include <iostream>
#include <stdarg.h>
#include <sstream>
#include <algorithm>
#include "Logger.h"
#include "Log.h"
#include "BufferRw.h"

Logger::Logger()
    : AbstractThread(1000),
      _curIndex(0), _totalCnt(0), _position(0), _logLen(Log::LogLen), _update(false),
      _bufferPtr(nullptr) {
    _bufferPtr = new uint8_t[_logLen * MaxLogCnt];
    loadLogTypeStr();
    load();
    start();
}

Logger::~Logger() {
    clear();
    stop();
}

void Logger::clear() {
    delete [] _bufferPtr;
}

void Logger::write(ELogId id) {
    Log log(_curIndex, id);
    writeLog(log);
}

void Logger::write(ELogId id, const char* fmt, ...) {
    const uint32_t BUFFER_SIZE = Log::StringLen;
    char buffer[BUFFER_SIZE];
    va_list vargs;
    va_start(vargs, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmt, vargs);
    buffer[BUFFER_SIZE - 1] = '\0';
    va_end(vargs);

    Log log(_curIndex, id, (uint8_t*)buffer);
    writeLog(log);
}

void Logger::writeLog(const Log& log) {
    memset(_bufferPtr+_position * _logLen, 0, _logLen);
    BufferRw rw(_bufferPtr+_position * _logLen, _logLen);
    log.writeToBuffer(&rw);
    _curIndex++;
    _position++;
    if (_totalCnt < MaxLogCnt)
        _totalCnt++;
    if (_position >= MaxLogCnt)
        _position = 0;
    if (_curIndex >= MaxIndex)
        _curIndex = 0;
    _update = true;
}

bool Logger::isInvalidFile(int len) {
    if (len <= 0) return true;
    if ((len / _logLen) > MaxLogCnt) return true;
    if (len % _logLen) return true;

    return false;
}

void Logger::getInformation(int len) {
    uint8_t buffer[_logLen];
    Log log(0, ELogId::None, buffer);

    int position = 0;
    uint16_t lastIndex = 0;
    int totalCnt = len / _logLen;
    for (int i = 0; i < totalCnt; i++) {
        memset(buffer, 0, _logLen);
        BufferRw rw(_bufferPtr + position * _logLen, _logLen);
        log.readFromBuffer(&rw);
        if (log.index() >= MaxIndex || log.id() >= (uint16_t)ELogId::None) {
            lastIndex = 0;
            position = 0;
            totalCnt = 0;
            break;
        }
        position++;
        if (i == 0) {
            lastIndex = log.index();
            continue;
        }
        if (lastIndex + 1 == log.index()) {
            lastIndex = log.index();
        } else {
            if (log.index() == 0) {
                lastIndex = log.index();
                continue;
            }
            lastIndex++;
            position--;
            break;
        }
    }
    _curIndex = lastIndex;
    _position = position;
    _totalCnt = totalCnt;
    //printf("_curIndex %d,_position %d, _totalCnt %d\n",_curIndex, _position, _totalCnt);
}

void Logger::load() {
    FILE* fp = fopen(_fileName, "a+b");
    if (fp == NULL) {
        printf("file open failed !!\n");
        return;
    }
    int len;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);

    if (isInvalidFile(len)) {
        fclose(fp);
        return;
    }
    fseek(fp, 0, SEEK_SET);
    fread(_bufferPtr, 1, len, fp);
    fclose(fp);

    getInformation(len);
}

void Logger::save() {
    uint8_t buffer[_logLen];
    Log log(0, ELogId::None, buffer);
    int position = _position;
    FILE* fp = fopen(_fileName, "wb");
    int totalCnt = _totalCnt;
    for (int i = 0; i < totalCnt; i++) {
        fwrite(_bufferPtr + i * _logLen, 1, _logLen, fp);
    }
    fclose(fp);
#if 0
    for (int i = 0; i < _totalCnt; i++) {
        memset(buffer, 0, _logLen);
        BufferRw rw(_bufferPtr + i * _logLen, _logLen);
        log.readFromBuffer(&rw);
        printf("%d, [%02d-%02d-%02d],[%02d:%02d:%02d]%s %s\n",
                log.index(), log.dateTime().year(), log.dateTime().month(), log.dateTime().day(),
                log.dateTime().hour(), log.dateTime().minute(), log.dateTime().second(),
                _logTypeStr[log.id()].c_str(), log.optionStr());
    }
    printf("\n");
#endif
}

bool Logger::compare(uint8_t* a, uint8_t* b) {  // ascending
    uint8_t bufferA[Log::LogLen];
    Log logA(0, ELogId::None, bufferA);
    memset(bufferA, 0, Log::LogLen);
    BufferRw rw(a, Log::LogLen);
    logA.readFromBuffer(&rw);

    uint8_t bufferB[Log::LogLen];
    Log logB(0, ELogId::None, bufferB);
    memset(bufferB, 0, Log::LogLen);
    BufferRw rwB(b, Log::LogLen);
    logB.readFromBuffer(&rwB);
    if (logA.dateTime() < logB.dateTime()) {
        return true;
    }
    else if (logA.dateTime() == logB.dateTime()) {
        if (logA.index() < logB.index()) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

std::string Logger::readLog(const DateTime from, const DateTime to) {
    uint8_t buffer[_logLen];
    Log log(0, ELogId::None, buffer);

    std::vector<uint8_t*> logs;
    for (uint32_t i = 0; i < _totalCnt; i++) {
        memset(buffer, 0, _logLen);
        BufferRw rw(_bufferPtr + i * _logLen, _logLen);
        log.readFromBuffer(&rw);
        if (log.dateTime() >= from && log.dateTime() <= to) {
            logs.push_back(_bufferPtr + i * _logLen);
        }
    }
    std::sort(logs.begin(), logs.end(), compare);

    std::ostringstream allLogStr;
    int index = 0;
    for (unsigned int i = 0; i < logs.size(); i++) {
        memset(buffer, 0, _logLen);
        BufferRw rw(logs[i], _logLen);
        log.readFromBuffer(&rw);
        std::string timeStr;
        sprintf(&timeStr[0], "%d|%02d-%02d-%02d|%02d:%02d:%02d|%d|%s\\r\\n",
                //log.index(), log.dateTime().year(), log.dateTime().month(), log.dateTime().day(),
                index, log.dateTime().year(), log.dateTime().month(), log.dateTime().day(),
                log.dateTime().hour(), log.dateTime().minute(), log.dateTime().second(),
                log.id(), log.optionStr());
        allLogStr << timeStr.c_str();
        index++;
    }
    std::string str = allLogStr.str();
    //printf("%s\n", str.c_str());
    return str;
}

void Logger::worker() {
    static int count = 0;
    count++;
    if (count >= 1 && _update) {
        _update = false;
        save();
        count = 0;
    }
}

void Logger::loadLogTypeStr() {
   _logTypeStr.push_back("System Start");
   _logTypeStr.push_back("System Shutdown");
   _logTypeStr.push_back("System Reboot");
   _logTypeStr.push_back("System UpgradeStart");
   _logTypeStr.push_back("System UpgradeFinish");
   _logTypeStr.push_back("System UpgradeFail");
   _logTypeStr.push_back("System Default");

   _logTypeStr.push_back("Login Success");
   _logTypeStr.push_back("Login Failed");
   _logTypeStr.push_back("User Log Out");

   _logTypeStr.push_back("Setup Changed:System");
   _logTypeStr.push_back("Setup Changed:User");
   _logTypeStr.push_back("Setup Changed:Motion");
   _logTypeStr.push_back("Setup Changed:AlarmIn");
   _logTypeStr.push_back("Setup Changed:AlarmOut");
   _logTypeStr.push_back("Setup Changed:Network");
   _logTypeStr.push_back("Setup Changed:VideoSteam");
   _logTypeStr.push_back("Setup Changed:AudioSteam");
   _logTypeStr.push_back("Setup Changed:DateTime");

   _logTypeStr.push_back("Network Down");
   _logTypeStr.push_back("Network Up");
   _logTypeStr.push_back("Video Loss");
   _logTypeStr.push_back("Search:Log");
}
