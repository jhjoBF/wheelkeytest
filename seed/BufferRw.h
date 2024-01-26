#ifndef BUFFERRW_H_
#define BUFFERRW_H_

#include <cstdint>
#include <assert.h>
#include <string.h>
#include <stdio.h>

class BufferRw {
private:
    uint8_t* _buffer;
    uint32_t _position;
    uint32_t _capacity;

public:
    BufferRw(uint8_t* buffer, uint32_t capacity) : _buffer(buffer), _position(0), _capacity(capacity) {}
    virtual ~BufferRw() {}

public:
    uint32_t position() { return _position; }
    uint32_t capacity() { return _capacity; }
    uint32_t remain() { return (_capacity - _position); }
    void set(void* data, uint32_t len);
    void get(void* data, uint32_t len);

public:
    uint8_t getUint8();
    uint16_t getUint16();
    uint32_t getUint32();
    uint64_t getUint64();
    int8_t getInt8();
    int16_t getInt16();
    int32_t getInt32();
    int64_t getInt64();

    void setUint8(uint8_t);
    void setUint16(uint16_t);
    void setUint32(uint32_t);
    void setUint64(uint64_t);
    void setInt8(int8_t);
    void setInt16(int16_t);
    void setInt32(int32_t);
    void setInt64(int64_t);
};

inline void BufferRw::set(void* data, uint32_t len) {
    assert(_buffer);
    assert(_capacity >= (_position + len));
    memcpy(_buffer+_position, data, len);
    _position += len;
}

inline void BufferRw::get(void* data, uint32_t len) {
    assert(_buffer);
    assert(_capacity > (len + _position));
    memcpy(data, _buffer +_position, len);
    _position += len;
}

inline uint8_t BufferRw::getUint8() {
    uint8_t ret;
    get((void*)&ret, sizeof(uint8_t));
    return ret;
}

inline uint16_t BufferRw::getUint16() {
    uint16_t ret;
    get((void*)&ret, sizeof(uint16_t));
    return ret;
}

inline uint32_t BufferRw::getUint32() {
    uint32_t ret;
    get((void*)&ret, sizeof(uint32_t));
    return ret;
}

inline uint64_t BufferRw::getUint64() {
    uint64_t ret;
    get((void*)&ret, sizeof(uint64_t));
    return ret;
}

inline int8_t BufferRw::getInt8() {
    int8_t ret;
    get((void*)&ret, sizeof(int8_t));
    return ret;
}

inline int16_t BufferRw::getInt16() {
    int16_t ret;
    get((void*)&ret, sizeof(int16_t));
    return ret;
}

inline int32_t BufferRw::getInt32() {
    int32_t ret;
    get((void*)&ret, sizeof(int32_t));
    return ret;
}

inline int64_t BufferRw::getInt64() {
    int64_t ret;
    get((void*)&ret, sizeof(int64_t));
    return ret;
}

inline void BufferRw::setUint8 (uint8_t param)  { set((void*)&param, sizeof(uint8_t)); }
inline void BufferRw::setUint16(uint16_t param) { set((void*)&param, sizeof(uint16_t)); }
inline void BufferRw::setUint32(uint32_t param) { set((void*)&param, sizeof(uint32_t)); }
inline void BufferRw::setUint64(uint64_t param) { set((void*)&param, sizeof(uint64_t)); }
inline void BufferRw::setInt8 (int8_t param)  { set((void*)&param, sizeof(int8_t)); }
inline void BufferRw::setInt16(int16_t param) { set((void*)&param, sizeof(int16_t)); }
inline void BufferRw::setInt32(int32_t param) { set((void*)&param, sizeof(int32_t)); }
inline void BufferRw::setInt64(int64_t param) { set((void*)&param, sizeof(int64_t)); }

#endif  // BUFFERRW_H_
