#include <stdio.h>
#include <string.h>
#include "DateTime.h"
#include "BufferRw.h"

DateTime::DateTime()
    : _date((uint32_t)0), _time((uint32_t)0) {}

DateTime::DateTime(uint32_t date, uint32_t time)
    : _date(date), _time(time) {}

DateTime::DateTime(time_t t)
    : _date(t), _time(t) {}

DateTime::DateTime(const DateTime& t)
    : _date(t._date), _time(t._time) {}

DateTime::DateTime(const systemc::time_point& tp)
    : _date(tp), _time(tp) {}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second)
    :_date(year, month, day), _time(hour, minute, second) {}

DateTime DateTime::current() {
    return DateTime(systemc::now());
}

void DateTime::print() {
    _date.print();
    _time.print();
}

DateTime& DateTime::operator=(const DateTime& dt) {
    _date = dt._date;
    _time = dt._time;
    return *this;
}

bool DateTime::operator==(const DateTime& dt) const {
    return _date == dt._date && _time == dt._time;
}

bool DateTime::operator<(const DateTime& dt) const {
    return _date < dt._date || (_date == dt._date && _time < dt._time);
}

bool DateTime::operator>(const DateTime& dt) const {
    return _date > dt._date || (_date == dt._date && _time > dt._time);
}

void DateTime::writeToBuffer(BufferRw* rw) const {
    rw->setUint32(_time.time());
    rw->setUint32(_date.date());
}

void DateTime::readFromBuffer(BufferRw* rw) {
    _time.setTime(rw->getUint32());
    _date.setDate(rw->getUint32());
}

uint32_t DateTime::length() const {
    return 4 + 4;
}
