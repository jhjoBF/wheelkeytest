#ifndef DATE_H_
#define DATE_H_

#include <chrono>
#include <cassert>
#include <cstdint>

using systemc = std::chrono::system_clock;
using steadyc = std::chrono::steady_clock;

enum DayOfWeek {
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};

class Date {
private:
    uint32_t _date;
    void set(time_t t);
public:
    Date();
    explicit Date(uint32_t date);
    explicit Date(time_t t);
    Date(const Date& t);
    Date(const systemc::time_point& t);
    Date(int year, int month, int day);

    Date& operator=(const Date& date);
    bool operator==(const Date& date) const;
    bool operator<(const Date& date) const;
    bool operator>(const Date& date) const;
public:
    int year() const  { return (_date& 0xffff0000) >> 16; }
    int month() const { return (_date& 0x0000ff00) >> 8; }
    int day() const   { return (_date& 0x000000ff); }
    uint32_t date() const { return _date; }

    static Date current();
    time_t to_time_t();

    bool isLeapYear();
    static bool isLeapYear(int year);

    int getLastDay();
    static int getLastDay(int year, int month);

    DayOfWeek dayOfWeek();
    static DayOfWeek dayOfWeek(int year, int month, int day);

    void setDate(uint32_t date) { _date = date; }
    void print();
};

#endif  // DATE_H_
