#include <cassert>
#include <stdio.h>
#include "Time.h"

Time::Time()
    : _time(0) {}

Time::Time(time_t t)
    : _time(0) { set(t); }

Time::Time(uint32_t t)
    : _time(t) {}

Time::Time(const Time& t)
    : _time(t._time) {}

Time::Time(int hour, int min, int sec)
    : _time(0) {
    _time = (0x00ff0000 & (hour << 16)) |
            (0x0000ff00 & (min << 8)) |
            (0x000000ff & (sec));
}

Time::Time(const systemc::time_point& tp)
    : _time(0) {
    set(systemc::to_time_t(tp));
}

Time::~Time() {}

void Time::set(time_t t) {
    struct tm localTm;
    struct tm* ptm = nullptr;

    ptm = localtime_r(&t, &localTm);
    assert(ptm != nullptr);

    _time = (0x00ff0000 & (localTm.tm_hour << 16)) |
            (0x0000ff00 & (localTm.tm_min << 8)) |
            (0x000000ff & (localTm.tm_sec));
}

Time Time::current() {
    systemc::time_point tp = systemc::now();
    return Time(tp);
}

void Time::print() {
    printf("%d:%d(%d)\n", hour(), minute(), second());
}

Time& Time::operator=(const Time& time) {
    _time = time._time;
    return *this;
}

bool Time::operator==(const Time& time) const {
    return _time == time._time;
}

bool Time::operator<(const Time& time) const {
    return _time < time._time;
}

bool Time::operator>(const Time& time) const {
    return _time > time._time;
}
