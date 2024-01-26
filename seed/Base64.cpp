#include "Base64.h"

static const uint8_t s_b64EncCode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const uint8_t s_b64DecCode[] = {
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 62,    0xff, 0xff, 0xff, 63,
        52  , 53  , 54  , 55  ,  56  , 57  , 58  , 59  ,  60  , 61  , 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0   , 1   , 2   ,  3   , 4   , 5   , 6   ,  7   , 8   , 9   , 10  ,  11  , 12  , 13  , 14  ,
        15  , 16  , 17  , 18  ,  19  , 20  , 21  , 22  ,  23  , 24  , 25  , 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 26  , 27  , 28  ,  29  , 30  , 31  , 32  ,  33  , 34  , 35  , 36  ,  37  , 38  , 39  , 40  ,
        41  , 42  , 43  , 44  ,  45  , 46  , 47  , 48  ,  49  , 50  , 51  , 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff};


void Base64::encodeBlock(const uint8_t block[3], uint32_t len, char encodedBlock[4]) {
    encodedBlock[0] = s_b64EncCode[(block[0] >> 2)];
    encodedBlock[1] = s_b64EncCode[((block[0] << 4) & 0x30) | ((block[1] >> 4) & 0x0f)];
    encodedBlock[2] = (len > 1) ? s_b64EncCode[(((block[1] << 2 ) & 0x3c) | ((block[2] >> 6) & 0x03))] : '=';
    encodedBlock[3] = (len > 2) ? s_b64EncCode[(block[2] & 0x3f)] : '=';
}

uint32_t  Base64::decodeBlock(const char encodedBlock[4], uint8_t block[3]) {
    uint32_t ret = 0;
    block[0] = (uint8_t)((s_b64DecCode[encodedBlock[0]] << 2) | (s_b64DecCode[encodedBlock[1]] >> 4));
    ret = 1;
    if (encodedBlock[2] != '=') {
        block[1] = (uint8_t)((s_b64DecCode[encodedBlock[1]] << 4) | (s_b64DecCode[encodedBlock[2]] >> 2));
        ret = 2;
    }
    if (encodedBlock[3] != '=') {
        block[2] = (uint8_t)((s_b64DecCode[encodedBlock[2]] << 6) | (s_b64DecCode[encodedBlock[3]]));
        ret = 3;
    }
    return ret;
}

uint32_t Base64::encode(const uint8_t* input, uint32_t len, char* encoded, uint32_t encodedLen) {
    uint32_t ret = 0;
    uint32_t blockLen = len / 3;
    uint32_t remainCharLen = len % 3;
    do {
        if (encodedBufferLen(len) > encodedLen) {
            break;
        }
        for (uint32_t i = 0; i < blockLen; i++) {
            encodeBlock(input + 3 * i, 3, encoded + 4 * i);
        }

        if (remainCharLen != 0) {
            encodeBlock(input + blockLen * 3, remainCharLen, encoded + blockLen * 4);
        }
        if (encodedBufferLen(len) < encodedLen) {
            encoded[encodedBufferLen(len)] = 0;
        }
        ret = encodedBufferLen(len);
    } while (false);
    return ret;
}

uint32_t Base64::decode(const char* encoded/*in*/, uint32_t len/*out*/, uint8_t* output/*out*/) {
    uint32_t ret = 0;
    // the length of encoded buffer must be multiple of 4.
    uint32_t blockLen = len / 4;
    do {
        if (len % 4 != 0) {
            break;
        }
        uint32_t paddingLen = (encoded[len - 1] == '=') ? (encoded[len - 2] == '=' ? 2 : 1) : 0;
        if (len < (blockLen * 3 - paddingLen)) {
            break;
        }
        for (uint32_t i = 0; i < blockLen; i++) {
            ret += decodeBlock(encoded + i * 4, output + i * 3);
        }
    } while (false);
    return ret;
}

uint32_t Base64::encodedBufferLen(const uint32_t inputLen) {
    return (inputLen / 3 + (((inputLen % 3) > 0) ? 1 : 0)) * 4;
}
