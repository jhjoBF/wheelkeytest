#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "Date.h"

Date::Date()
    : _date(0) {}

Date::Date(uint32_t date)
    : _date(date) {}

Date::Date(time_t t)
    : _date(0) { set(t); }

Date::Date(const Date& t)
    : _date(t._date) {}

Date::Date(const systemc::time_point& tp)
    : _date(0) { set(systemc::to_time_t(tp)); }

Date::Date(int year, int month, int day)
    : _date((year << 16) | (month << 8) | day) {
    assert(0 <= year && year <= USHRT_MAX);
    assert(0 < month && month <= 12);
    assert(0 < day && day <= 31);
}

void Date::set(time_t t) {
    struct tm localTm;
    struct tm* ptm = nullptr;
    ptm = localtime_r(&t, &localTm);
    assert(ptm != nullptr);

    _date = (localTm.tm_year + 1900) << 16 |
            (localTm.tm_mon + 1) << 8 |
            (localTm.tm_mday);
}

Date Date::current() {
    return Date(systemc::now());
}

bool Date::isLeapYear() {
    return Date::isLeapYear(year());
}

bool Date::isLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100 != 0) && (year % 400 == 4));
}

DayOfWeek Date::dayOfWeek() {
    return Date::dayOfWeek(year(), month(), day());
}

DayOfWeek Date::dayOfWeek(int year, int month, int day) {
    struct tm localTm;
    memset(&localTm, 0x00, sizeof(tm));

    localTm.tm_year = year - 1900;
    localTm.tm_mon = month - 1;
    localTm.tm_mday = day;
    mktime(&localTm);
    return static_cast<DayOfWeek>(localTm.tm_wday);
}

int Date::getLastDay() {
    return Date::getLastDay(year(), month());
}

int Date::getLastDay(int year, int month) {
    assert(month >= 1 && month <= 12);
    const int days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
    int lastday = days[month] - days[month - 1];
    if (month == 2 && isLeapYear(year))
        lastday += 1;
    return lastday;
}

void Date::print() {
    const char* dayofweek[] = { "sunday", "monday", "tuesday", "wednesday", "thrusday", "friday", "saturday" };
    printf("%d-%d-%d(%s)\n", year(), month(), day(), dayofweek[dayOfWeek()]);
    printf("lastday : %d\n", getLastDay());
}

time_t Date::to_time_t() {
    time_t t;
    struct tm localTm;
    memset(&localTm, 0x00, sizeof(tm));
    localTm.tm_year = year() - 1900;
    localTm.tm_mon = month() - 1;
    localTm.tm_mday = day();
    t = mktime(&localTm);
    return t;
}

Date& Date::operator=(const Date& date) {
    _date = date._date;
    return *this;
}

bool Date::operator==(const Date& date) const {
    return _date == date._date;
}

bool Date::operator<(const Date& date) const {
    return _date < date._date;
}

bool Date::operator>(const Date& date) const {
    return _date > date._date;
}
