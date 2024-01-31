#ifndef UART_H_
#define UART_H_

#include <stdint.h>

class Uart {
public:
   Uart(const char* path);
   Uart();
   virtual ~Uart();
private:
    int _fd;
public:
    int create(const char* path);
    int setOption(int baud, int dataBit = 8, int stopBit = 1, const char* Parity = "NONE");
    int read(char* rbuf, uint32_t size, uint32_t timeoutMilli = 1000);
    int write(char* sbuf, int slen);
};

#endif  // UART_H_
