#ifndef TIME_H_
#define TIME_H_

#include <time.h>
#include <chrono>

using systemc = std::chrono::system_clock;
using steadyc = std::chrono::steady_clock;

class Time {
private:
    uint32_t _time;
    void set(time_t t);
public:
    Time();
    explicit Time(time_t t);
    explicit Time(uint32_t t);
    Time(const Time& t);
    Time(int hour, int minute, int second);
    Time(const systemc::time_point& tp);
    virtual ~Time();

    Time& operator=(const Time& time);
    bool operator==(const Time& time) const;
    bool operator<(const Time& time) const;
    bool operator>(const Time& time) const;

public:
    int hour() const   { return (_time & 0xff0000) >> 16; }
    int minute() const { return (_time & 0xff00) >> 8; }
    int second() const { return  _time & 0xff; }

    //int offsetUTC() const { return (10 * 60) * (signed char)(((_time & 0xff000000) >> 24)); }  // unit signed secs
    //int totalSeconds() const { return hour() * 60 * 60 + minute() * 60 + second(); }

    uint32_t time() const { return _time; }
    void setTime(uint32_t time) { _time = time; }
    static Time current();

    void print();
};

#endif  // TIME_H_
