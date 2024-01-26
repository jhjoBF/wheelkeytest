#ifndef DATETIME_H_
#define DATETIME_H_

#include "IBuffering.h"
#include "Date.h"
#include "Time.h"
#include "IComparable.h"

class DateTime : public IBuffering, public IComparable<DateTime> {
public:
    DateTime();
    DateTime(uint32_t date, uint32_t time);
    DateTime(time_t t);
    DateTime(const DateTime& t);
    DateTime(const systemc::time_point& tp);
    DateTime(int year, int month, int day, int hour, int minute, int second);

private:
    Date _date;
    Time _time;

public:
    uint32_t date() const { return _date.date(); }
    uint32_t time() const { return _time.time(); }

    int year() const { return _date.year(); }
    int month() const { return _date.month(); }
    int day() const { return _date.day(); }
    int hour() const { return _time.hour(); }
    int minute() const { return _time.minute(); }
    int second() const { return _time.second(); }

    static DateTime current();

    void setDateTime(uint32_t date, uint32_t time) { _date.setDate(date); _time.setTime(time); }
    void print();

    DateTime& operator=(const DateTime& datetime);
    virtual bool operator==(const DateTime& datetime) const;
    virtual bool operator<(const DateTime& datetime) const;
    virtual bool operator>(const DateTime& datetime) const;

public:  // IBuffering
    void writeToBuffer(BufferRw* rw) const;
    void readFromBuffer(BufferRw* rw);
    uint32_t length() const;
};

#endif  // DATETIME_H_
