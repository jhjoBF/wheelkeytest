#include "Log.h"
#include "BufferRw.h"

Log::Log()
    :_index(0), _id(ELogId::None), _optionStr(nullptr), _dateTime(DateTime::current()) {}

Log::Log(uint16_t index, ELogId id)
    :_index(index), _id(id), _optionStr(nullptr), _dateTime(DateTime::current()) {}

Log::Log(uint16_t index, ELogId id, uint8_t* opt)
    :_index(index), _id(id), _optionStr(opt), _dateTime(DateTime::current()) {}

Log::~Log() {}

void Log::writeToBuffer(BufferRw* rw) const {
    rw->setUint16(_index);
    rw->setUint16((uint16_t)_id);
    _dateTime.writeToBuffer(rw);
    if (_optionStr != nullptr) {
        rw->set(_optionStr, StringLen - 1);
    }
}

void Log::readFromBuffer(BufferRw* rw) {
    _index = rw->getUint16();
    _id = (ELogId)rw->getUint16();
    _dateTime.readFromBuffer(rw);
    if (_optionStr != nullptr) {
        rw->get(_optionStr, StringLen - 1);
    }
}

uint32_t Log::length() const {
    return HeaderLen+StringLen;
}
