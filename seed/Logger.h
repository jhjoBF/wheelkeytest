#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <vector>
#include "AbstractThread.h"
#include "Singleton.h"
#include "logConst.h"

class Log;
class DateTime;
class Logger : public AbstractThread, public Singleton<Logger> {
public:
    Logger();
    virtual ~Logger();
private:
    uint8_t* _bufferPtr;
    bool _update;
    uint32_t _curIndex;
    uint32_t _position;
    uint32_t _totalCnt;
    const uint32_t _logLen;
    const uint32_t MaxLogCnt = 1000;
    const uint32_t MaxIndex = MaxLogCnt * 3;
    std::vector <std::string> _logTypeStr;
    //FILE* _fp;
    const char* _fileName = "/param/systemLog.bin";

    void load();
    void save();
    void clear();
    void writeLog(const Log& log);
    void loadLogTypeStr();
    bool isInvalidFile(int len);
    void getInformation(int len);
    static bool compare(uint8_t* a, uint8_t* b);
public:
    void write(ELogId id, const char* fmt, ...);
    void write(ELogId id);
    std::string readLog(const DateTime fDateTime, const DateTime tDateTime);
protected:  // IThread
    virtual void worker() override;
};

#endif  // LOGGER_H
