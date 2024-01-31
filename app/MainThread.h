#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_
#include <queue>
#include "AbstractThread.h"
#include "Uart.h"

class MainThread : public AbstractThread {
public:
    MainThread();
    virtual ~MainThread();
protected:
    void worker();
    void preRun();
    void postRun();
private:
    typedef struct {
        uint8_t code;
        uint8_t len;
        uint8_t data[32];
        bool crcOk;
    } respData;

    enum { ModeIdle , ModeRainbow, ModePicture, ModeTouch, ModeMax };
    enum { Init, Waiting, CheckVersion, Welcome, Runing, };
    enum { HalfNone, HalfUp, HalfRight, HalfDown, HalfLeft };

    int _state;
    int _nextState;
    Uart _uart;
    std::queue<uint8_t> _rxQueue;

    void uartInit();
    int readUartData();
    uint8_t checkPacketLenth(std::queue<uint8_t> q);
    respData checkResponse();
    uint8_t getCheckSum(uint8_t* data, uint16_t len);
    void sendCmd(uint8_t cmd, uint8_t data);
    void showImage(uint16_t id, uint16_t x, uint16_t y);
    void drawRect(uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    void drawRectHalf(uint8_t mode, uint16_t color);
    void actionMode(respData resp);
};

#endif  // MAIN_THREAD_H_
