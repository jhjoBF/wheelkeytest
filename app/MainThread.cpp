#include <cstring>
#include <unistd.h>
#include "MainThread.h"
#include "Utility.h"

#define BUF_SIZE 512

#define HEAD_CODE               0xaa
#define CMD_CONNECT             0x01  // 1: Connect, 0: Disconnect
#define CMD_IMG                 0x02
#define CMD_FILL_RECT           0x03
#define CMD_SHOW                0x04  // 1: Display on, 0: Off
#define CMD_SEND_BIN            0x05
#define CMD_GET_VERSION         0x06

#define RESP_CODE               0xff
#define RESP_ACK                0xfe
#define RESP_NACK_CHECKSUM      0xfd
#define RESP_NACK_NOTSUPPORT    0xfc
#define RESP_NACK_INUSE         0xfb
#define RESP_NACK_DICONNECTED   0xfa

#define RESP_VERSION            0xf1
#define RESP_ACTION             0xf2
#define RESP_DISPLAY_COMPLETE   0xf3

#define SHORT_PUSH              0x01
#define LONG_PUSH               0x02
#define WHEEL_LEFT              0x03
#define WHEEL_RIGHT             0x04
#if USE_TOUCH
#define TOUCH                   0x05
#define TOUCH_LONG              0x06
#define TOUCH_LEFT              0x07
#define TOUCH_RIGHT             0x08
#define TOUCH_UP                0x09
#define TOUCH_DOWN              0x0a
#endif

//#define WIDTH_MAX   466
#define WIDTH_MAX   320

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xffff
const uint16_t colorCode[7] = {0xf800, 0xfc60, 0xffe0, 0x0400, 0x00001f, 0x4810, 0x8010};

MainThread::MainThread() : AbstractThread(100), _state(Init) {
    printf("%s, %s:%d Create\n", __FILENAME__, __func__, __LINE__);
}
MainThread::~MainThread() {}
void MainThread::preRun() { uartInit(); }
void MainThread::postRun() {}

uint8_t MainThread::checkPacketLenth(std::queue<uint8_t> q) {
    q.pop();  // HEAD_CODE
    q.pop();  // COMMAND
    return q.front();  // LENGTH
}

void MainThread::worker() {
    respData resp;
    uint8_t c;
    uint8_t packetlen;

    if (_state == Init) {
        printf(">> Connect\n");
        sendCmd(CMD_CONNECT, 1);
        _state = Waiting;
        _nextState = CheckVersion;
    }
    else if (_state == CheckVersion) {
        uint8_t value = 0;
        printf(">> Check version\n");
        sendCmd(CMD_GET_VERSION, 0);
        _state = Waiting;
        _nextState = Welcome;
    }
    else if (_state == Welcome) {
        showImage(0, 0, 0);
        //sendCmd(CMD_SHOW, 1);
        _state = Runing;
    }

    readUartData();
    while (_rxQueue.size() >= 5) {
        c = _rxQueue.front();
        if (c != HEAD_CODE) {
            _rxQueue.pop();
            printf(".");
            //printf("drop 0x%x\n", c);
            return;
        }
        packetlen = checkPacketLenth(_rxQueue);
        packetlen += 4;  // HEAD_CODE, COMMAND, LENGTH, ..., CHECKSUM
        if (_rxQueue.size() < packetlen) {
            printf(",");
            return;
        }
        resp = checkResponse();
        if (!resp.crcOk) continue;
        if (resp.code == RESP_VERSION) {
            printf("Version : %s\n", resp.data);
        }
        else if (resp.code == RESP_ACTION) {
            actionMode(resp);
        }
        else if (resp.code == RESP_CODE) {
            switch (resp.data[0]) {
                case RESP_ACK: printf("  << OK\n"); break;
                case RESP_NACK_CHECKSUM: printf("  << NACK checksum\n"); break;
                case RESP_NACK_NOTSUPPORT: printf("  << NACK not support\n"); break;
                case RESP_NACK_INUSE: printf("  << NACK inuse\n"); break;
                case RESP_NACK_DICONNECTED: printf("  << NACK disconnect\n"); break;
                default: printf("  << Resp code unknown : %d\n", resp.data[0]); break;
            }
            if (_state == Waiting) {
                _state = _nextState;
            }
        }
        else if (resp.code == RESP_DISPLAY_COMPLETE) {
            printf("  << Display\n");
            sendCmd(CMD_SHOW, 1);
        }
        else {
            printf("[%02x](%02X) ", resp.code, resp.len);
            for (int i = 0; i < resp.len; i++) printf("%02x ", resp.data[i]);
            printf("\n");
        }
    }
}

void MainThread::drawRectHalf(uint8_t mode, uint16_t color) {
    const uint16_t widthHalf = WIDTH_MAX / 2;
    if (mode == HalfUp)
        drawRect(color, 0, 0, WIDTH_MAX, widthHalf);
    else if (mode == HalfRight)
        drawRect(color, widthHalf, 0, widthHalf, WIDTH_MAX);
    else if (mode == HalfDown)
        drawRect(color, 0, widthHalf, WIDTH_MAX, widthHalf);
    else if (mode == HalfLeft)
        drawRect(color, 0, 0, widthHalf, WIDTH_MAX);
}

void MainThread::actionMode(respData resp) {
    static int mode = ModeIdle;
    static int idx = 0;
    static uint8_t halfMode = HalfNone;

    if (resp.data[0] == SHORT_PUSH) {
        mode++;
        if (mode >= ModeMax)
            mode = ModeIdle;
        printf("Mode changed : %d\n", mode);
        idx = 0;

        switch (mode) {
            case ModeIdle:
                showImage(0, 0, 0);
                //sendCmd(CMD_SHOW, 1);
                break;
            case ModeRainbow:
                halfMode = HalfNone;
                drawRect(colorCode[idx], 0, 0, WIDTH_MAX, WIDTH_MAX);
                //sendCmd(CMD_SHOW, 1);
                break;
            case ModePicture:
                idx = 1;
                halfMode = HalfNone;
                showImage(idx, 0, 0);
                //sendCmd(CMD_SHOW, 1);
                break;
#if USE_TOUCH
            case ModeTouch:
                drawRect(COLOR_BLACK, 0, 0, WIDTH_MAX, WIDTH_MAX);
                //sendCmd(CMD_SHOW, 1);
#endif
            default:
                printf("Mode error: %d\n", mode);
                break;
        }
        return;
    }

    if (mode == ModeRainbow) {
        switch (resp.data[0]) {
        case WHEEL_LEFT:
            if (halfMode) {
                halfMode--;
                if (halfMode == HalfNone) halfMode = HalfLeft;
                drawRectHalf(halfMode, colorCode[idx]);
                //sendCmd(CMD_SHOW, 1);
            }
            else {
                if (idx > 0) {
                    idx--;
                    drawRect(colorCode[idx], 0, 0, WIDTH_MAX, WIDTH_MAX);
                    //sendCmd(CMD_SHOW, 1);
                }
            }
            break;
        case WHEEL_RIGHT:
            if (halfMode) {
                if (halfMode == HalfLeft) halfMode = HalfUp;
                else halfMode++;
                drawRectHalf(halfMode, colorCode[idx]);
                //sendCmd(CMD_SHOW, 1);
            }
            else {
                if (idx < 6) {
                    idx++;
                    drawRect(colorCode[idx], 0, 0, WIDTH_MAX, WIDTH_MAX);
                    //sendCmd(CMD_SHOW, 1);
                }
            }
            break;
#if USE_TOUCH
        case TOUCH:
            if (halfMode)
                halfMode = HalfNone;
            else
                if (++idx > 6) idx = 0;
            drawRect(colorCode[idx], 0, 0, WIDTH_MAX, WIDTH_MAX);
            //sendCmd(CMD_SHOW, 1);
            break;
        case TOUCH_LEFT:
            halfMode = HalfLeft;
            drawRectHalf(halfMode, colorCode[idx]);
            //sendCmd(CMD_SHOW, 1);
            break;
        case TOUCH_RIGHT:
            halfMode = HalfRight;
            drawRectHalf(halfMode, colorCode[idx]);
            //sendCmd(CMD_SHOW, 1);
            break;
        case TOUCH_UP:
            halfMode = HalfUp;
            drawRectHalf(halfMode, colorCode[idx]);
            //sendCmd(CMD_SHOW, 1);
            break;
        case TOUCH_DOWN:
            halfMode = HalfDown;
            drawRectHalf(halfMode, colorCode[idx]);
            //sendCmd(CMD_SHOW, 1);
            break;
#endif
        default:
            printf("unknown cmd 0x%x\n", resp.data[0]);
            break;
        }
    }
    else if (mode == ModePicture) {
        switch (resp.data[0]) {
            case WHEEL_LEFT:
                if (halfMode) {
                    halfMode--;
                    if (halfMode == HalfNone) halfMode = HalfLeft;
                    showImage(idx, 0, 0);
                    drawRectHalf(halfMode, COLOR_BLACK);
                    //sendCmd(CMD_SHOW, 1);
                }
                else if (idx > 1) {
                    idx--;
                    showImage(idx, 0, 0);
                    //sendCmd(CMD_SHOW, 1);
                }
                break;
            case WHEEL_RIGHT:
                if (halfMode) {
                    if (halfMode == HalfLeft) halfMode = HalfUp;
                    else halfMode++;
                    showImage(idx, 0, 0);
                    drawRectHalf(halfMode, COLOR_BLACK);
                    //sendCmd(CMD_SHOW, 1);
                } else if (idx < 7) {
                    idx++;
                    showImage(idx, 0, 0);
                    //sendCmd(CMD_SHOW, 1);
                }
                break;
#if USE_TOUCH
            case TOUCH:
                if (halfMode)
                    halfMode = HalfNone;
                else
                    if (++idx > 7) idx = 1;
                showImage(idx, 0, 0);
                //sendCmd(CMD_SHOW, 1);
                break;
            case TOUCH_LEFT:
                halfMode = HalfRight;
                showImage(idx, 0, 0);
                drawRectHalf(halfMode, COLOR_BLACK);
                //sendCmd(CMD_SHOW, 1);
                break;
            case TOUCH_RIGHT:
                halfMode = HalfLeft;
                showImage(idx, 0, 0);
                drawRectHalf(halfMode, COLOR_BLACK);
                //sendCmd(CMD_SHOW, 1);
                break;
            case TOUCH_UP:
                halfMode = HalfDown;
                showImage(idx, 0, 0);
                drawRectHalf(halfMode, COLOR_BLACK);
                //sendCmd(CMD_SHOW, 1);
                break;
            case TOUCH_DOWN:
                halfMode = HalfUp;
                showImage(idx, 0, 0);
                drawRectHalf(halfMode, COLOR_BLACK);
                //sendCmd(CMD_SHOW, 1);
                break;
#endif
            default:
                printf("ModePicture unknown : %d\n", resp.data[0]);
                break;
        }
    }
#if USE_TOUCH
    else if (mode == ModeTouch) {
        if (resp.data[0] == TOUCH) {
            //printHexDump(resp.data, resp.len, 16);
            uint16_t x = (resp.data[1] << 8) | resp.data[2];
            uint16_t y = (resp.data[3] << 8) | resp.data[4];
            printf("%d, %d\n", x, y);
            drawRect(COLOR_WHITE, x - 10, y - 10, 20, 20);
            //sendCmd(CMD_SHOW, 1);
        }
    }
#endif
}

MainThread::respData MainThread::checkResponse() {
    respData ret;
    uint8_t c = 0;
    uint8_t checksum = 0;

    memset(&ret, 0, sizeof(respData));
    c = _rxQueue.front();
    _rxQueue.pop();

    /*if (c != HEAD_CODE) {
        printf("HEAD_CODE Error : ");
        while (!_rxQueue.empty()) {
            printf("%d", _rxQueue.front());
            _rxQueue.pop();
        }
        printf("\n");
        exit(1);
    }*/

    ret.code = _rxQueue.front();
    _rxQueue.pop();
    checksum += ret.code;

    ret.len = _rxQueue.front();
    _rxQueue.pop();
    checksum += ret.len;

    for (int i = 0; i < ret.len; i++) {
        ret.data[i] = _rxQueue.front();
        _rxQueue.pop();
        checksum += ret.data[i];
    }

    checksum ^= 0xff;
    c = _rxQueue.front();
    _rxQueue.pop();
    if (checksum == c) {
        ret.crcOk = true;
    } else {
        ret.crcOk = false;
        printf("CHECKSUM Error : 0x%02x, 0x%02x\n", checksum, c);
        //exit(1);
    }

    return ret;
}

void MainThread::uartInit() {
    printf("%s, %s:%d\n", __FILENAME__, __func__, __LINE__);
    _uart.create("/dev/ttyUSB0");
    _uart.setOption(115200, 8, 1, "NONE");
}

int MainThread::readUartData() {
    uint8_t buf[BUF_SIZE] = {0,};
    int len = 0;
    len = _uart.read((char*)buf, BUF_SIZE, 0);
    if (len > 0) {
        for (int i = 0; i < len; i++) _rxQueue.push(buf[i]);
    }
    else if (len < 0 ) {
        exit(1);
    }
    return len;
}

uint8_t MainThread::getCheckSum(uint8_t* data, uint16_t len) {
    uint16_t checksum = 0;
    for (uint16_t i = 1; i < len; i++)
        checksum += data[i];
    checksum ^= 0xff;
    return (uint8_t)checksum;
}

void MainThread::sendCmd(uint8_t cmd, uint8_t data) {
    uint8_t buf[BUF_SIZE] = {0,};
    uint8_t idx = 0;
    buf[idx++] = HEAD_CODE;
    buf[idx++] = cmd;
    buf[idx++] = 0x1;
    buf[idx++] = data;
    buf[idx++] = getCheckSum(buf, idx);
    _uart.write((char*)buf, idx);
}
void MainThread::showImage(uint16_t id, uint16_t x, uint16_t y) {
    uint8_t buf[BUF_SIZE] = {0,};
    uint8_t idx = 0;
    buf[idx++] = HEAD_CODE;
    buf[idx++] = CMD_IMG;
    buf[idx++] = 0x6;   // one image 6Byte;
    buf[idx++] = (id >> 8) & 0xff;
    buf[idx++] = id & 0xff;
    buf[idx++] = (x >> 8) & 0xff;
    buf[idx++] = x & 0xff;
    buf[idx++] = (y >> 8) & 0xff;
    buf[idx++] = y & 0xff;
    buf[idx++] = getCheckSum(buf, idx);
    _uart.write((char*)buf, idx);
}

void MainThread::drawRect(uint16_t color, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    uint8_t buf[BUF_SIZE] = {0,};
    uint8_t idx = 0;
    buf[idx++] = HEAD_CODE;
    buf[idx++] = CMD_FILL_RECT;
    buf[idx++] = 0x0A;
    buf[idx++] = (color >> 8) & 0xff;
    buf[idx++] = color & 0xff;
    buf[idx++] = (x >> 8) & 0xff;
    buf[idx++] = x & 0xff;
    buf[idx++] = (y >> 8) & 0xff;
    buf[idx++] = y & 0xff;
    buf[idx++] = (width >> 8) & 0xff;
    buf[idx++] = width & 0xff;
    buf[idx++] = (height >> 8) & 0xff;
    buf[idx++] = height & 0xff;
    buf[idx++] = getCheckSum(buf, idx);
    _uart.write((char*)buf, idx);
}
