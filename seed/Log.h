#ifndef LOG_H_
#define LOG_H_

#include "IBuffering.h"
#include "DateTime.h"
#include "logConst.h"

class Log : public IBuffering {
public:
    Log(uint16_t index, ELogId id);
    Log(uint16_t index, ELogId id, uint8_t* opt);
    Log();
    virtual ~Log();
private:
    uint16_t _index;
    ELogId _id;
    DateTime _dateTime;
    //LogID _id;
    uint8_t* _optionStr;
    static const uint32_t HeaderLen = 2 + 2 + 4 + 4 + 4;
public:
    static const uint32_t StringLen = 64;
    static const uint32_t LogLen = HeaderLen + StringLen;
    uint16_t index() { return _index; }
    uint16_t id() { return (uint16_t)_id; }
    const DateTime& dateTime() { return _dateTime; }
    uint8_t* optionStr() { return _optionStr; }
public: //IBuffering
    void writeToBuffer(BufferRw* rw) const;
    void readFromBuffer(BufferRw* rw);
    uint32_t length() const;
};

#endif  // LOG_H_
