#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <chrono>
#include "AbstractThread.h"

class Chair {
public:
    Chair();
    ~Chair();

    // Setters
    void setId(const std::string& id) { _id = id; }
    void setIp(const std::string& ip) { _ip = ip; }
    void setOnline(bool online) { _online = online; }
    void setRun(int run) { _run = run; }
    void setArm1(int arm1) { _arm1 = arm1; }
    void setArm2(int arm2) { _arm2 = arm2; }
    void setStan(int stan) { _stan = stan; }
    void setBody(int body) { _body = body; }
    void setLeg1(int leg1) { _leg1 = leg1; }
    void setLeg2(int leg2) { _leg2 = leg2; }
    void setLe1l(int le1l) { _le1l = le1l; }
    void setLe2l(int le2l) { _le2l = le2l; }
    void setAnk1(int ank1) { _ank1 = ank1; }
    void setAnk2(int ank2) { _ank2 = ank2; }

    // Getters
    const std::string& getId() const { return _id; }
    const std::string& getIp() const { return _ip; }
    bool isOnline() const { return _online; }
    int getRun() const { return _run; }
    int getArm1() const { return _arm1; }
    int getArm2() const { return _arm2; }
    int getStan() const { return _stan; }
    int getBody() const { return _body; }
    int getLeg1() const { return _leg1; }
    int getLeg2() const { return _leg2; }
    int getLe1l() const { return _le1l; }
    int getLe2l() const { return _le2l; }
    int getAnk1() const { return _ank1; }
    int getAnk2() const { return _ank2; }

    void print() const;

private:
    bool _online;
    std::string _id;
    std::string _ip;
    int _run;
    int _arm1;
    int _arm2;
    int _stan;
    int _body;
    int _leg1;
    int _leg2;
    int _le1l;
    int _le2l;
    int _ank1;
    int _ank2;
};

typedef struct {
    int timeout;
    int delay;      // 이 맵을 보내기 전 대기 시간 (초)
    int _arm1;
    int _arm2;
    int _stan;
    int _body;
    int _leg1;
    int _leg2;
    int _le1l;
    int _le2l;
    int _ank1;
    int _ank2;
} Maptype;

extern Maptype Map[];
extern const int MapCount;

enum class MapState {
    IDLE,           // 대기 중
    DELAYING,       // delay 시간 대기 중
    SENDING,        // 명령 전송 중
    WAITING_RUN,    // 모든 Chair가 RUN 상태로 변경되기를 대기
    WAITING_STOP,   // 모든 Chair가 STOP 상태로 변경되기를 대기
    WAITING_ACTION  // 특별 액션(run==2) 완료 대기
};

class MainThread : public AbstractThread {
public:
    MainThread();
    virtual ~MainThread();

    // Chair IP 설정
    void setChairIp(int index, const std::string& ip);
    void setChairCount(int count);

    // Map 관리
    void addMap(const Maptype& map);
    void startMapSequence();
    void stopMapSequence();
    void setStartMapIndex(int index);    // 시작 맵 인덱스 설정

    // 모드 설정
    void setPositionMode(int position);  // 포지션 모드 설정
    void setStopMode(bool stop);         // 정지 모드 설정
    void setRebootMode(bool reboot);     // 재부팅 모드 설정
    void setMonitorMode(bool monitor);   // 모니터 모드 설정

private:
    Chair _dev[10];
    int _currentChairIndex;
    int _chairCount;
    int _updateCounter;

    // 모드 관련
    bool _monitorMode;      // true: 모니터링만, false: 맵 실행
    bool _positionMode;     // true: 포지션 명령 전송
    int _positionValue;     // 포지션 값
    bool _positionSent;     // 포지션 명령 전송 완료 여부
    bool _stopMode;         // true: 정지 명령 전송
    bool _stopSent;         // 정지 명령 전송 완료 여부
    bool _rebootMode;       // true: 재부팅 명령 전송
    bool _rebootSent;       // 재부팅 명령 전송 완료 여부

    // Map 실행 관련
    std::vector<Maptype> _maps;
    size_t _currentMapIndex;
    MapState _mapState;
    time_t _delayStartTime;
    Maptype _lastSentMap;  // 마지막으로 보낸 Map 저장
    std::chrono::steady_clock::time_point _waitRunStartTime;  // WAITING_RUN 시작 시간 (밀리초 정밀도)
    time_t _waitStopStartTime;  // WAITING_STOP 시작 시간 (timeout용)
    bool _chairEverRan[10];  // 각 Chair가 한번이라도 run 상태가 되었는지

protected:
    void worker();
    void preRun();
    void postRun();

private:
    bool updateChairData(int index);
    void printAllChairs();

    // Map 실행 관련 메서드
    bool sendMapToChair(int chairIndex, const Maptype& map);
    bool sendActionToChair(int chairIndex, int action);  // 특별 액션 전송
    void sendActionToAllChairs(int action);               // 모든 Chair에 액션 전송
    bool allChairsRunning();
    bool allChairsStopped();
    bool anyChairRunningAction();  // 하나라도 액션 실행 중인지 확인 (run==2)
    void processMapSequence();

    // 포지션 명령 전송
    bool sendPositionToChair(int chairIndex, int position);
    void sendPositionToAllChairs();

    // 정지 명령 전송
    bool sendStopToChair(int chairIndex);
    void sendStopToAllChairs();

    // 재부팅 명령 전송
    bool sendRebootToChair(int chairIndex);
    void sendRebootToAllChairs();
};

#endif  // MAIN_THREAD_H_
