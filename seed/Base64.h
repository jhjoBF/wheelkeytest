#ifndef BASE64_H_
#define BASE64_H_

#include <stdint.h>
class Base64 {
protected:
    //static const uint8_t s_b64EncCode[];
    //static const uint8_t s_b64DecCode[];
    static void encodeBlock(const uint8_t block[3], uint32_t len, char encodedBlock[4]);
    static uint32_t decodeBlock(const char encodedBlock[4], uint8_t block[3]);

public:
    Base64() {}
    ~Base64() {}

    static uint32_t encode(/*in*/const uint8_t* btInput,
                           /*in*/uint32_t len,
                           /*out*/char* encodedBuffer,
                           /*in*/ uint32_t encodedBufferLen);

    static uint32_t decode(/*in*/const char* encoded,
                           /*in*/ uint32_t len,
                           /*out*/ uint8_t* btOutput);

    static uint32_t encodedBufferLen(/*in*/const uint32_t inputLen);
};

#endif  // BASE64_H_
