#include "Uart.h"
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <error.h>
#include <cerrno>

Uart::Uart() : _fd(0xffffffff) {}
Uart::Uart(const char* path) : _fd(0xffffffff) { _fd = create(path); }
Uart::~Uart() {
    if (_fd != -1) {
        printf("%s, %s:%d close uart 0x%x\n", __FILENAME__, __func__, __LINE__, _fd);
        close(_fd);
    }
}

int Uart::create(const char* path) {
    int fd;
    printf("%s, %s:%d open uart %s\n", __FILENAME__, __func__, __LINE__, path);
    //fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
    fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        printf("open error!!\n");
        return -1;
    }
    /*if (fcntl(fd, F_SETFL, 0) < 0) {
        printf("fcntl failed!\n");
    }
    if (isatty(STDIN_FILENO) == 0) {
        printf("standard input is not a terminal device!!\n");
    }*/
    _fd = fd;
    return fd;
}

int Uart::setOption(int baud, int dataBit, int stopBit, const char* Parity) {
    struct termios newtio;
    //printf("baudRate %d, dataBit %d, stopBit %d, parityBit %s\n", baud, dataBit, stopBit, Parity);
    memset(&newtio, 0, sizeof(newtio));
#if 0
    //newtio.c_oflag = 0;
    newtio.c_oflag = OPOST | ONLCR;
    newtio.c_cflag = CLOCAL | CREAD;  // no rts/cts
#else
    cfmakeraw(&newtio);
#endif

    if (dataBit == 7) newtio.c_cflag |= CS7;
    else newtio.c_cflag |= CS8;

    if (stopBit == 2) newtio.c_cflag |= CSTOPB;

    if (strncmp(Parity, "ODD", strlen("ODD") == 0)) {
        newtio.c_iflag = PARODD;
    } else if(strncmp(Parity, "EVEN", strlen("EVEN") == 0)) {
        newtio.c_iflag = PARENB;
    } else {
        newtio.c_iflag = IGNPAR;  // no parity
    }

    switch (baud) {
        case 921600: newtio.c_cflag |= B921600; break;
        case 460800: newtio.c_cflag |= B460800; break;
        case 230400: newtio.c_cflag |= B230400; break;
        case 115200: newtio.c_cflag |= B115200; break;
        case 57600: newtio.c_cflag |= B57600; break;
        case 38400: newtio.c_cflag |= B38400; break;
        case 19200: newtio.c_cflag |= B19200; break;
        case 9600: newtio.c_cflag |= B9600; break;
        case 4800: newtio.c_cflag |= B4800; break;
        case 2400: newtio.c_cflag |= B2400; break;
        default: newtio.c_cflag |= B115200; break;
    }
    //newtio.c_cflag |= B115200;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(_fd, TCIFLUSH);
    tcsetattr(_fd, TCSANOW, &newtio);

    return 0;
}

int Uart::read(char* rbuf, uint32_t size, uint32_t timeoutMilli/* =1000 */) {
    int ret;
    int rlen;
    //char rbuf[100] = {'\0'};
    fd_set fdr;
    struct timeval tv;

    FD_ZERO(&fdr);
    FD_SET(_fd, &fdr);

    tv.tv_sec = timeoutMilli / 1000;
    tv.tv_usec = (timeoutMilli % 1000) * 1000;

    ret = select(_fd + 1, &fdr, NULL, NULL, &tv);
    if (ret == 0) {
        //printf("%s, %s:%d\n", __FILENAME__, __FUNCTION__, __LINE__);
        return 0;
    } else if (ret < 0) {
        printf("%s, select err\r\n", __FUNCTION__);
        return -1;
    }

    if (FD_ISSET(_fd, &fdr)) {
        rlen = ::read(_fd, rbuf, size);
        //rlen = ::read(_fd, rbuf, 1);
        //printf("%s, %s:%d, rlen %d, bufSize %d\n", __FILENAME__, __FUNCTION__, __LINE__, rlen, sizeof(rbuf));
        if (rlen == 0) printf("%s, select err\r\n", __FUNCTION__);
        if (rlen < 0) return -1;
    }
    return rlen;
}

int Uart::write(char* sbuf, int slen) {
    int res;
    res = ::write(_fd, sbuf, slen);
    if (res != slen) {
        printf("%s, slen %d, wirte err[%s]\r\n", __FUNCTION__, slen, strerror(errno));
    }
    return res;
}
