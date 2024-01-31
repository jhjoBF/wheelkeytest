#ifndef UTILITY_H_
#define UTILITY_H_

#include <ctime>

char* getHwaddr(const char* dev, bool colonEn);
char* getAddr(const char* dev);
char* getEthName(int index);
char* getUuid(const char* path);
char* urlDecode(char* str);
char* getTimeStr(time_t rawtime, const char* format);
char* getUptimeStr(void);
//int decryptAes128Base64UrlEncoding(const char* input, char* output, const char* key, const char* iv);
const char* generateKey(int bits);
unsigned int createCrc(unsigned int crc, const char* pBuffer, unsigned int len);
bool checkInetAddr(char* addr);
void printHexDump(unsigned char* data, unsigned int size, unsigned int width);
#endif  // UTILITY_H_
