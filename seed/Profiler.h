#ifndef PROFILER_H_
#define PROFILER_H_

#include <chrono>
#include <stdio.h>
#include <string>
#include <vector>

namespace stdch = std::chrono;
using steadyc = stdch::steady_clock;
using steadytp = stdch::steady_clock::time_point;
using millis = stdch::milliseconds;

// T : integr or floating point Type
template<typename T>
class Profiler {
private:
    std::string _title;
    steadytp _lastTp;
    steadytp _firstTp;

    millis _maxTerm;
    millis _minTerm;
    millis _lastTerm;

    T _maxValue;
    T _minValue;
    T _lastValue;
    uint32_t _bandwidth;
    uint32_t _fps;
    int _displayDelay;

    millis _mediumTerm;
    millis _sumOfTerm;

    T _mediumValue;
    double _sumOfSample;
    uint32_t _samplingCount;

    inline steadytp current() {
        return steadyc::now();
    }
public:
    Profiler(const char* title, int id = 0)
        //: _title("unknown")
        : _samplingCount{0}, _sumOfSample{0.0},
          _maxTerm{0}, _minTerm{0xffffff}, _lastTerm{0},
          _bandwidth(0), _fps(0), _displayDelay(3) {
        if (title != nullptr) {
            /* int len = strlen(title);
            if (len > 31) {
                len = 31;
            } */
            _title = title;
            sprintf(&_title[0], "%s:%d", title, id);
            //printf("_title : %s\n", _title.c_str());
        }
    }

    virtual ~Profiler() {}
    void sample(T data) {
        steadytp tp = current();
        if (_samplingCount == 0) { // first sampling point
            _minValue = _maxValue = _lastValue = data;
            _sumOfSample = 0;
            _firstTp = tp;
        } else {
            _lastValue = data;
            if (data > _maxValue) {
                _maxValue = data;
            }
            if (data < _minValue) {
                _minValue = data;
            }
            _lastTerm = stdch::duration_cast<millis>(tp - _lastTp);
            if (_lastTerm > _maxTerm) {
                _maxTerm = _lastTerm;
            }
            if (_lastTerm < _minTerm) {
                _minTerm = _lastTerm;
            }
        }

        _samplingCount++;
        _sumOfSample += static_cast<double>(data);
        _mediumValue = static_cast<T>(_sumOfSample / _samplingCount);
        if (_samplingCount > 1 ) {
            _mediumTerm = stdch::duration_cast<millis>(tp - _firstTp) / (_samplingCount - 1);
        }
        auto diff = tp - _firstTp;
        int sec = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
        if (sec > 3) {
            resultString(sec);
            _samplingCount = 0;
        }
        _lastTp = tp;
    }
    void stop() { _samplingCount = 0; }
    bool isAvailable() { return (_samplingCount > 1); }

    millis maxTerm() { return _maxTerm; }
    millis minTerm() { return _minTerm; }
    millis mediumTerm() { return _mediumTerm; }
    T maxValue() { return _maxValue; }
    T minValue() { return _minValue; }
    T mediumValue() { return _mediumValue; }

    uint32_t samplingCount() { return _samplingCount; }
    uint32_t bandwidth() { return _bandwidth; }
    int getFrameRate() { return _fps; }

    void resultString(int sec) {
    //void resultString(std::string& out) {
        if (sec == 0) return;
        std::string out;
        char sz[128] = {0,};
        sprintf(sz, "Profile[%s] bps %d, ", _title.c_str(), _samplingCount / sec);
        out = sz;
        if (_samplingCount > 1 ) {
            sprintf(sz, "Term (Avg:%dms, Min:%dms, Max:%dms), ",
                    _mediumTerm.count(), _minTerm.count(), _maxTerm.count());
            out += sz;
            sprintf(sz, "Data (Avg:%dkb, Min:%dkb, Max:%dkb)\n",
                    mediumValue() / 1024, minValue() / 1024, maxValue() / 1024);
            _bandwidth = _sumOfSample * 8 / sec / 1024;
            _fps = _samplingCount / sec;
        } else {
            sprintf(sz, "\tSample Count too small\n");
        }
        out += sz;
        _displayDelay++;
        if (_displayDelay > 10) {
            _displayDelay = 0;
            printf("%s", out.c_str());
        }
    }
};

#endif  // PROFILER_H_
