#ifndef IBUFFERING_H_
#define IBUFFERING_H_

#include <stdint.h>
class BufferRw;
class IBuffering {
public:
    virtual void writeToBuffer(BufferRw* rw) const = 0;
    virtual void readFromBuffer(BufferRw* rw) = 0;
    virtual uint32_t length() const = 0;
};

#endif  // IBUFFERING_H_
