#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "MainThread.h"
#include "HttpClient.h"
#include "JsonParser.h"

#define REPEAT   0 //반복       // 0: 반복안함
#define SYMMETRY 1 //대칭       // 1: 끝자리 홀수짝수 대칭
#define SURFING  1 //파도타기   // 1: 끝가지 2개씩 파도타기 10,11 -> 12,13 -> ...

//#define __ 0xff
Maptype Map[] = {
/*
timeout delay, arm1, arm2, stan, body, leg1, leg2, le1l, le2l, ank1, ank2 */
    { 0,    0,    5,    5,    0,    0,    0,    0,    0,    0,    0,    0 }, // 00 준비
    { 6,    0,    5,    5,  100,    0,   45,   45,    0,    0,   50,   50 }, // 01 일어서
    { 0,    1,   95,    5,  100,    0,   45,   45,    0,    0,   50,   50 }, // 02 왼팔 파도
    { 6,    1,   95,   95,  100,    0,   45,   45,    0,    0,   50,   50 }, // 03 오른팔 파도
    { 0,    0,   50,   50,  100,    0,   45,   45,    0,    0,   50,   50 }, // 04 팔 나란히
    { 0,    0,   95,    5,  100,    0,   45,   90,    0,    0,   80,    0 }, // 05 팔다리 엇갈리기
    { 0,    0,    5,   95,  100,    0,   90,   45,    0,    0,    0,   80 }, // 06 팔다리 엇갈리기
    { 7,    0,   95,    5,    0,   70,   45,   90,   20,   20,  100,    0 }, // 07 누우며 엇갈리기
    { 7,    0,    5,   95,    0,   70,   90,   45,   20,   20,    0,  100 }, // 08 누우며 엇갈리기
    { 0,    0,    5,    5,    0,   70,   45,   45,   60,   60,   50,   50 }, // 09 누우며 자세 정렬
    { 8,    1,   95,   95,    0,  100,   10,   10,   90,   90,  100,  100 }, // 10 전신 스트레칭
    { 0,    0,   50,   50,    0,  100,   45,   45,   50,   50,   50,   50 }, // 11 양팔 위로 다리 평행
    { 6,    0,   95,    5,    0,  100,   45,   45,   10,   90,   50,   50 }, // 12 공중부양 다리 쭉쭉이 팔 엇갈리기
    { 0,    0,   95,    5,    0,  100,   45,   45,   10,   90,    0,  100 }, // 13 발목 스트레칭
    { 0,    0,    5,   95,    0,  100,   45,   45,   90,   10,   50,   50 }, // 14 공중부양 다리 쭉쭉이 팔 엇갈리기
    { 0,    0,    5,   95,    0,  100,   45,   45,   90,   10,  100,    0 }, // 15 발목 스트레칭
    { 7,    0,    5,    5,    5,   70,   45,   45,   20,   20,   50,   50 }, // 16 기본 마사지 자세 복귀
    { 0,    0,    5,    5,    5,   80,   68,   68,   40,   40,   50,   50 }, // 17 사이클 자세 준비
    { 0, 0xf1,    5,    5,    5,   80,   68,   68,   40,   40,   50,   50 }, // 18 (S)RoboWalking
    { 8,    0,    5,    5,  100,   70,   45,   45,   20,   20,   50,   50 }, // 19 기본 자세 복귀
    { 7,    0,   95,   95,  100,    0,   33,   33,    0,    0,   50,   50 }, // 20 스탠딩 + 양팔들기
    { 8,    1,    5,    5,    5,    0,   33,   33,    0,    0,   50,   50 }, // 21 양팔 내리기
    { 0,    0,    5,    5,  100,    0,   33,   33,    0,    0,   20,   20 }, // 22 시연모드 스탠딩
    { 0,    0,   50,   50,  100,    0,   33,   33,    0,    0,   20,   20 }, // 23 앞으로 나란히
    { 0,    0,   95,    5,  100,    0,   33,   33,    0,    0,   50,    0 }, // 24 팔 허우적, 발목 까딱
    { 0,    0,    5,   95,  100,    0,   33,   33,    0,    0,    0,   50 }, // 25
    { 0,    0,   95,    5,  100,    0,   33,   33,    0,    0,   50,    0 }, // 26
    { 0,    0,    5,   95,  100,    0,   33,   33,    0,    0,    0,   50 }, // 27
    { 0,    0,    5,    5,  100,    0,   33,   33,    0,    0,   50,   50 }, // 28
    { 8,    0,    5,    5,    0,  100,   45,   45,    0,    0,   50,   50 }, // 29 눕기 CES0
    { 0,    0,    5,   95,    0,  100,   90,   10,   70,   70,   50,   50 }, // 30 찢기 CES1
    { 5,    0,    5,   95,    0,  100,   90,   10,   70,   70,    0,  100 }, // 31
    { 0,    0,    5,   95,    0,  100,   90,   10,   70,   70,   50,   50 }, // 32
    { 0,    0,   95,    5,    0,  100,   10,   90,   70,   70,   50,   50 }, // 33 찢기 CES2
    { 5,    0,   95,    5,    0,  100,   10,   90,   70,   70,  100,    0 }, // 34
    { 0,    0,   50,   50,    0,  100,   10,   90,   70,   70,   50,   50 }, // 35
    { 9,    0,    5,    5,    0,   10,   45,   45,    0,    0,   50,   50 }, // 36 기본 자세 복귀
    { 0,    0,    5,   90,    0,   80,   90,   20,   50,   50,   50,   50 }, // 37 Fin.
};

const int MapCount = sizeof(Map) / sizeof(Maptype);

Chair::Chair() {
    _online = false;
    _run = 0;
    _arm1 = 0;
    _arm2 = 0;
    _stan = 0;
    _body = 0;
    _leg1 = 0;
    _leg2 = 0;
    _le1l = 0;
    _le2l = 0;
    _ank1 = 0;
    _ank2 = 0;
}
Chair::~Chair() {}

void Chair::print() const {
    const char* status = _online ? "ON " : "OFF";
    const char* runStatus = _run ? "RUN" : "STP";

    printf("%-10s %-15s [%3s] [%3s] ARM:%3d/%3d STAN:%3d BODY:%3d LEG:%3d/%3d LE:%3d/%3d ANK:%3d/%3d\n",
           _id.c_str(), _ip.c_str(), status, runStatus,
           _arm1, _arm2, _stan, _body, _leg1, _leg2, _le1l, _le2l, _ank1, _ank2);
}

MainThread::MainThread() : AbstractThread(20) {
    _currentChairIndex = 0;
    _chairCount = 0;
    _updateCounter = 0;
    _currentMapIndex = 0;
    _mapState = MapState::IDLE;
    _delayStartTime = 0;
    _waitRunStartTime = 0;

    // 모드 초기화
    _monitorMode = false;
    _positionMode = false;
    _positionValue = 0;
    _positionSent = false;
    _stopMode = false;
    _stopSent = false;
    _rebootMode = false;
    _rebootSent = false;

    // 마지막 보낸 Map 초기화
    memset(&_lastSentMap, 0, sizeof(_lastSentMap));
    memset(_chairEverRan, 0, sizeof(_chairEverRan));
}
MainThread::~MainThread() {}

void MainThread::setChairIp(int index, const std::string& ip) {
    if (index >= 0 && index < 10) {
        _dev[index].setIp(ip);
    }
}

void MainThread::setChairCount(int count) {
    if (count >= 0 && count <= 10) {
        _chairCount = count;
    }
}

void MainThread::setPositionMode(int position) {
    _positionMode = true;
    _positionValue = position;
    _positionSent = false;
}

void MainThread::setMonitorMode(bool monitor) {
    _monitorMode = monitor;
}

void MainThread::setStopMode(bool stop) {
    _stopMode = stop;
    _stopSent = false;
}

void MainThread::setRebootMode(bool reboot) {
    _rebootMode = reboot;
    _rebootSent = false;
}

// 값이 255이면 "__", 아니면 숫자로 포맷팅
static std::string formatValue(int value) {
    if (value == 255) {
        return "__";
    }
    char buf[8];
    sprintf(buf, "%2d", value);
    return std::string(buf);
}

void MainThread::printAllChairs() {
    // 터미널 클리어 (ANSI escape code)
    printf("\033[2J\033[H");

    // 모드에 따른 헤더 표시
    if (_positionMode) {
        printf("========== POSITION MODE (n=%d) ==========\n", _positionValue);
    } else if (_stopMode) {
        printf("========== STOP MODE ==========\n");
    } else if (_monitorMode) {
        printf("========== MONITOR MODE ==========\n");
    } else if (_mapState != MapState::IDLE && !_maps.empty()) {
        // Map 진행 상황 표시
        size_t displayIndex = _currentMapIndex;
        if (_mapState == MapState::WAITING_STOP || _mapState == MapState::WAITING_RUN) {
            // 실행 중일 때는 현재 인덱스 표시
        } else if (displayIndex > 0) {
            displayIndex--; // 이전에 보낸 Map 표시
        }

        printf("========== Map Progress: %zu/%zu ==========\n",
               displayIndex, _maps.size() - 1);
        printf("Current Map: %-29s ARM:%3s/%3s STAN:%3s BODY:%3s LEG:%3s/%3s LE:%3s/%3s ANK:%3s/%3s\n", " ",
               formatValue(_lastSentMap._arm1).c_str(), formatValue(_lastSentMap._arm2).c_str(),
               formatValue(_lastSentMap._stan).c_str(), formatValue(_lastSentMap._body).c_str(),
               formatValue(_lastSentMap._leg1).c_str(), formatValue(_lastSentMap._leg2).c_str(),
               formatValue(_lastSentMap._le1l).c_str(), formatValue(_lastSentMap._le2l).c_str(),
               formatValue(_lastSentMap._ank1).c_str(), formatValue(_lastSentMap._ank2).c_str());
        printf("===========================================\n");
    } else {
        printf("========== CES Chair Status ==========\n");
    }

    for (int i = 0; i < _chairCount; i++) {
        printf("[%d] ", i);
        _dev[i].print();
    }
    printf("======================================\n");
    fflush(stdout);
}

bool MainThread::updateChairData(int index) {
    if (index < 0 || index >= 10) {
        return false;
    }

    std::string ip = _dev[index].getIp();
    if (ip.empty()) {
        return false;
    }

    // HTTP GET 요청 (재시도 없음 - 상태 체크용)
    HttpClient client;
    client.setTimeout(1);  // 1초 타임아웃

    std::string url = "http://" + ip + "/v1/api/ces/info";
    std::string response;

    if (!client.get(url, response)) {
        _dev[index].setOnline(false);
        // 에러 메시지 출력 안 함 (너무 많은 로그 방지)
        return false;
    }

    // JSON 파싱
    JsonParser parser(response);
    if (!parser.parse()) {
        _dev[index].setOnline(false);
        printf("[ERROR] Chair[%d] %s - JSON parse failed\n", index, ip.c_str());
        return false;
    }

    // Chair 데이터 업데이트
    _dev[index].setOnline(true);
    _dev[index].setId(parser.getString("id", ""));
    _dev[index].setRun(parser.getInt("run", 0));
    _dev[index].setArm1(parser.getInt("arm1", 0));
    _dev[index].setArm2(parser.getInt("arm2", 0));
    _dev[index].setStan(parser.getInt("stan", 0));
    _dev[index].setBody(parser.getInt("body", 0));
    _dev[index].setLeg1(parser.getInt("leg1", 0));
    _dev[index].setLeg2(parser.getInt("leg2", 0));
    _dev[index].setLe1l(parser.getInt("le1l", 0));
    _dev[index].setLe2l(parser.getInt("le2l", 0));
    _dev[index].setAnk1(parser.getInt("ank1", 0));
    _dev[index].setAnk2(parser.getInt("ank2", 0));

    return true;
}

void MainThread::preRun() {}
void MainThread::postRun() {}

void MainThread::worker() {
    // chairCount가 설정되지 않았으면 아무것도 하지 않음
    if (_chairCount == 0) {
        return;
    }
    
    // WAITING_STOP, WAITING_RUN, WAITING_ACTION 상태일 때는 빠른 업데이트 (10ms)
    if (_mapState == MapState::WAITING_STOP || _mapState == MapState::WAITING_RUN || _mapState == MapState::WAITING_ACTION) {
        setSleepTime(10);
    } else {
        setSleepTime(20);
    }
    
    // 포지션 모드: 한 번만 전송
    if (_positionMode && !_positionSent) {
        sendPositionToAllChairs();
        _positionSent = true;
        printf("[INFO] Position %d sent to all chairs. Press Ctrl+C to exit.\n", _positionValue);
    }
    
    // 정지 모드: 한 번만 전송
    if (_stopMode && !_stopSent) {
        sendStopToAllChairs();
        _stopSent = true;
        printf("[INFO] Stop command sent to all chairs. Press Ctrl+C to exit.\n");
    }
    
    // 재부팅 모드: 한 번만 전송
    if (_rebootMode && !_rebootSent) {
        sendRebootToAllChairs();
        _rebootSent = true;
        printf("[INFO] Reboot command sent to all chairs. Press Ctrl+C to exit.\n");
    }

    // 병렬로 모든 Chair 업데이트
    std::vector<std::thread> threads;

    for (int i = 0; i < _chairCount; i++) {
        std::string ip = _dev[i].getIp();
        if (!ip.empty()) {
            threads.push_back(std::thread([this, i]() {
                updateChairData(i);
            }));
        }
    }

    // 모든 스레드가 완료될 때까지 대기
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // 모니터 모드가 아니면 Map 시퀀스 처리
    if (!_monitorMode && !_positionMode && !_stopMode && !_rebootMode) {
        processMapSequence();
    }

    // 모든 업데이트 완료 후 상태 출력
    _updateCounter++;
    printAllChairs();
}

// Map 관리 메서드
void MainThread::addMap(const Maptype& map) {
    _maps.push_back(map);
}

void MainThread::startMapSequence() {
    if (_maps.empty()) {
        printf("[INFO] No maps to execute\n");
        return;
    }
    // _currentMapIndex가 이미 설정되어 있으면 유지 (setStartMapIndex로 설정된 경우)
    // 설정되지 않았으면 0으로 초기화
    if (_currentMapIndex >= (int)_maps.size()) {
        _currentMapIndex = 0;
    }
    _mapState = MapState::SENDING;  // 첫 Map은 바로 전송
    printf("[INFO] Starting map sequence with %zu maps (from index %zu)\n", _maps.size(), _currentMapIndex);
}

void MainThread::stopMapSequence() {
    _mapState = MapState::IDLE;
    printf("[INFO] Map sequence stopped\n");
}

void MainThread::setStartMapIndex(int index) {
    if (index >= 0 && index < (int)_maps.size()) {
        _currentMapIndex = index;
        printf("[INFO] Start map index set to %d\n", index);
    } else {
        printf("[WARNING] Invalid start index %d, using 0\n", index);
        _currentMapIndex = 0;
    }
}

// Chair에 Map 명령 전송
bool MainThread::sendMapToChair(int chairIndex, const Maptype& map) {
    if (chairIndex < 0 || chairIndex >= _chairCount) {
        return false;
    }

    std::string ip = _dev[chairIndex].getIp();
    if (ip.empty()) {
        return false;
    }

    // URL 생성
    char url1[512];
    char url2[512];
    char* url;
    sprintf(url1, "http://%s/v1/api/ces/set?arm1=%d&arm2=%d&stan=%d&body=%d&leg1=%d&leg2=%d&le1l=%d&le2l=%d&ank1=%d&ank2=%d",
            ip.c_str(), map._arm1, map._arm2, map._stan, map._body,
            map._leg1, map._leg2, map._le1l, map._le2l, map._ank1, map._ank2);
    sprintf(url2, "http://%s/v1/api/ces/set?arm1=%d&arm2=%d&stan=%d&body=%d&leg1=%d&leg2=%d&le1l=%d&le2l=%d&ank1=%d&ank2=%d",
            ip.c_str(), map._arm2, map._arm1, map._stan, map._body,
            map._leg2, map._leg1, map._le2l, map._le1l, map._ank2, map._ank1);

    // IP 주소의 끝자리가 짝수이면 url1, 홀수이면 url2 사용
    size_t lastDotPos = ip.find_last_of('.');
    if (lastDotPos != std::string::npos) {
        int lastOctet = atoi(ip.substr(lastDotPos + 1).c_str());
        if (lastOctet % 2 == 0) {
            url = url1;  // 짝수
        } else {
            url = url2;  // 홀수
        }
    } else {
        url = url1;  // 기본값
    }
#if !SYMMETRY
    url = url1;
#endif

    // HTTP GET 요청 (재시도 로직 포함)
    HttpClient client;
    client.setTimeout(1);
    std::string response;

    const int maxRetries = 5;  // 최대 5회 재시도
    for (int retry = 0; retry < maxRetries; retry++) {
        if (client.get(url, response)) {
            if (retry > 0) {
                printf("[INFO] Chair[%d] %s - Success after %d retries\n", chairIndex, ip.c_str(), retry);
            }
            return true;
        }
        
        // 마지막 시도가 아니면 300ms 대기 후 재시도 (네트워크 안정화)
        if (retry < maxRetries - 1) {
#ifdef _WIN32
            Sleep(300);
            //Sleep(50);//jhjo
#else
            usleep(300000);
#endif
        }
    }
    
    printf("[ERROR] Failed to send map to Chair[%d] %s after %d retries\n", chairIndex, ip.c_str(), maxRetries);
    return false;
}

// Chair에 특별 액션 명령 전송
bool MainThread::sendActionToChair(int chairIndex, int action) {
    if (chairIndex < 0 || chairIndex >= _chairCount) {
        return false;
    }

    std::string ip = _dev[chairIndex].getIp();
    if (ip.empty()) {
        return false;
    }

    // URL 생성
    char url[256];
    sprintf(url, "http://%s/v1/api/ces/runAction", ip.c_str());

    // HTTP GET 요청 (재시도 로직 포함)
    HttpClient client;
    client.setTimeout(1);
    std::string response;

    const int maxRetries = 5;  // 최대 5회 재시도
    for (int retry = 0; retry < maxRetries; retry++) {
        if (client.get(url, response)) {
            if (retry > 0) {
                printf("[INFO] Chair[%d] %s - Action success after %d retries\n", chairIndex, ip.c_str(), retry);
            }
            return true;
        }
        
        // 마지막 시도가 아니면 300ms 대기 후 재시도 (네트워크 안정화)
        if (retry < maxRetries - 1) {
#ifdef _WIN32
            Sleep(300);
            //Sleep(50); //jhjo
#else
            usleep(300000);
#endif
        }
    }
    
    printf("[ERROR] Failed to send action to Chair[%d] %s after %d retries\n", chairIndex, ip.c_str(), maxRetries);
    return false;
}

// 모든 Chair에 특별 액션 명령 전송// 모든 Chair에 특별 액션 명령 전송
void MainThread::sendActionToAllChairs(int action) {
    printf("[INFO] Sending action %d to all chairs...\n", action);

    std::vector<std::thread> threads;

    for (int i = 0; i < _chairCount; i++) {
        threads.push_back(std::thread([this, i, action]() {
            sendActionToChair(i, action);
        }));
        
        // 각 Chair 연결 시작 간격을 두어 네트워크 부하 분산 (200ms)
        if (i < _chairCount - 1) {
#ifdef _WIN32
            Sleep(300);
            //Sleep(50); //jhjo
#else
            usleep(200000);
#endif
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    printf("[INFO] Action %d sent to all chairs\n", action);
}

// 모든 Chair가 RUN 상태인지 확인 (개선된 버전)
bool MainThread::allChairsRunning() {
    int onlineCount = 0;
    
    // 각 Chair의 run 상태 확인 및 기록
    for (int i = 0; i < _chairCount; i++) {
        if (_dev[i].isOnline()) {
            onlineCount++;
            if (_dev[i].getRun() != 0) {
                _chairEverRan[i] = true;  // 한번이라도 run 상태가 되면 기록
            }
        }
    }

    // 온라인 Chair가 하나도 없으면 false 반환 (대기)
    if (onlineCount == 0) {
        return false;
    }

    // 모든 온라인 Chair가 run 상태이거나, 한번이라도 run 상태였으면 OK
    for (int i = 0; i < _chairCount; i++) {
        if (_dev[i].isOnline()) {
            // run 중이거나, 이전에 run 상태였던 적이 있으면 OK
            if (_dev[i].getRun() == 0 && !_chairEverRan[i]) {
                return false;  // 아직 run 상태가 안되고 이전에도 안됐음
            }
        }
    }

    return true;
}

// 모든 Chair가 STOP 상태인지 확인
bool MainThread::allChairsStopped() {
    int onlineCount = 0;
    for (int i = 0; i < _chairCount; i++) {
        if (_dev[i].isOnline()) {
            onlineCount++;
            // 한번이라도 run 상태였던 Chair만 체크
            if (_chairEverRan[i] && _dev[i].getRun() != 0) {
                return false;
            }
        }
    }
    // 온라인 Chair가 하나도 없으면 false 반환 (대기)
    if (onlineCount == 0) {
        return false;
    }
    return true;
}

// 하나라도 특별 액션 실행 중인지 확인 (run == 2)
bool MainThread::anyChairRunningAction() {
    for (int i = 0; i < _chairCount; i++) {
        if (_dev[i].isOnline()) {
            if (_dev[i].getRun() == 2) {  // 특별 액션 실행 중
                return true;
            }
        }
    }
    return false;
}

// Map 시퀀스 처리
void MainThread::processMapSequence() {
    if (_mapState == MapState::IDLE || _maps.empty()) {
        return;
    }

    if (_currentMapIndex >= _maps.size()) {
        printf("[INFO] All maps completed!\n");
        _mapState = MapState::IDLE;
        return;
    }

    Maptype& currentMap = _maps[_currentMapIndex];

    switch (_mapState) {
        case MapState::SENDING:
            {
                // 마지막 보낸 Map 저장
                _lastSentMap = currentMap;

                // chairEverRan 초기화
                memset(_chairEverRan, 0, sizeof(_chairEverRan));

#if !SURFING
                currentMap.delay = 0;
#endif
                // delay가 0xf1이면 특별 액션 전송 (action=1)
                if (currentMap.delay == 0xf1) {
                    sendActionToAllChairs(1);
                    printf("[INFO] Special action sent, waiting for completion...\n");
                    _mapState = MapState::WAITING_ACTION;
                    break;
                }

                // delay가 0이면 모든 Chair에 동시 전송
                if (currentMap.delay == 0) {
                    // 모든 Chair에 명령 전송 (병렬, 시간차 두고 시작)
                    std::vector<std::thread> threads;
                    for (int i = 0; i < _chairCount; i++) {
                        threads.push_back(std::thread([this, i, &currentMap]() {
                            sendMapToChair(i, currentMap);
                        }));
                        
                        // 각 Chair 연결 시작 간격 (200ms)
                        if (i < _chairCount - 1) {
                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                            //std::this_thread::sleep_for(std::chrono::milliseconds(50));//jhjo
                        }
                    }

                    for (auto& t : threads) {
                        if (t.joinable()) {
                            t.join();
                        }
                    }
                } else {
                    // delay가 0이 아니면 그룹별로 시간차를 두고 전송
                    // 2개씩 묶어서 그룹화: (0,1), (2,3), (4,5), (6,7), (8,9)
                    int groupCount = (_chairCount + 1) / 2;  // 그룹 개수 계산
                    
                    for (int group = 0; group < groupCount; group++) {
                        std::vector<std::thread> threads;
                        
                        // 각 그룹의 2개 Chair에 명령 전송
                        int chair1 = group * 2;
                        int chair2 = group * 2 + 1;
                        
                        if (chair1 < _chairCount) {
                            threads.push_back(std::thread([this, chair1, &currentMap]() {
                                sendMapToChair(chair1, currentMap);
                            }));
                        }
                        
                        if (chair2 < _chairCount) {
                            threads.push_back(std::thread([this, chair2, &currentMap]() {
                                sendMapToChair(chair2, currentMap);
                            }));
                        }
                        
                        // 스레드 종료 대기
                        for (auto& t : threads) {
                            if (t.joinable()) {
                                t.join();
                            }
                        }
                        
                        printf("[INFO] Group %d (Chair %d,%d) sent\n", group, chair1, chair2);
                        
                        // 마지막 그룹이 아니면 delay 시간만큼 대기
                        if (group < groupCount - 1) {
                            std::this_thread::sleep_for(std::chrono::seconds(currentMap.delay));
                        }
                    }
                }

                printf("[INFO] Waiting for all chairs to start running...\n");
                _waitRunStartTime = time(NULL);
                _mapState = MapState::WAITING_RUN;
            }
            break;

        case MapState::WAITING_RUN:
            {
                time_t elapsed = time(NULL) - _waitRunStartTime;

                // 1초 타임아웃: 1초 안에 run 상태가 안되면 이미 목표 위치에 있다고 간주
                if (elapsed >= 1 || allChairsRunning()) {
                    if (elapsed >= 1) {
                        printf("[INFO] Timeout waiting for run state (some chairs already at target position)\n");
                    } else {
                        printf("[INFO] All chairs are running, waiting for completion...\n");
                    }
                    _waitStopStartTime = time(NULL);  // WAITING_STOP 시작 시간 기록
                    _mapState = MapState::WAITING_STOP;
                }
            }
            break;

        case MapState::WAITING_STOP:
            {
                time_t elapsed = time(NULL) - _waitStopStartTime;
                bool stopped = allChairsStopped();
                bool timedOut = false;

                // timeout이 설정된 경우 체크 (0이 아닌 경우)
                if (_lastSentMap.timeout > 0 && elapsed >= _lastSentMap.timeout) {
                    timedOut = true;
                    printf("[INFO] Timeout (%d sec) reached for map %zu\n", _lastSentMap.timeout, _currentMapIndex + 1);
                }

                if (stopped || timedOut) {
                    if (stopped) {
                        printf("[INFO] All chairs completed map %zu\n", _currentMapIndex + 1);
                    }
                    _currentMapIndex++;

#if REPEAT
                    // 맵이 끝까지 실행되면 처음부터 다시 반복
                    if (_currentMapIndex >= _maps.size()) {
                        _currentMapIndex = 0;
                        printf("[INFO] All maps completed! Restarting from first map...\n");
                    }
#endif

                    _mapState = MapState::SENDING;  // 다음 Map 바로 전송
                }
            }
            break;

        case MapState::WAITING_ACTION:
            // 모든 Chair의 특별 액션 완료 대기 (run == 2 → run == 0)
            if (!anyChairRunningAction()) {
                printf("[INFO] All chairs completed special action\n");
                _currentMapIndex++;

#if REPEAT
                // 맵이 끝까지 실행되면 처음부터 다시 반복
                if (_currentMapIndex >= _maps.size()) {
                    _currentMapIndex = 0;
                    printf("[INFO] All maps completed! Restarting from first map...\n");
                }
#endif

                _mapState = MapState::SENDING;  // 다음 Map 바로 전송
            }
            break;

        default:
            break;
    }
}

// 포지션 명령을 Chair에 전송
bool MainThread::sendPositionToChair(int chairIndex, int position) {
    if (chairIndex < 0 || chairIndex >= _chairCount) {
        return false;
    }

    std::string ip = _dev[chairIndex].getIp();
    if (ip.empty()) {
        return false;
    }

    // URL 생성
    char url[256];
    sprintf(url, "http://%s/v1/api/ces/pos?n=%d", ip.c_str(), position);

    // HTTP GET 요청
    HttpClient client;
    client.setTimeout(2);
    std::string response;

    if (!client.get(url, response)) {
        printf("[ERROR] Failed to send position to Chair[%d] %s\n", chairIndex, ip.c_str());
        return false;
    }

    printf("[INFO] Position %d sent to Chair[%d] %s\n", position, chairIndex, ip.c_str());
    return true;
}

// 모든 Chair에 포지션 명령 전송
void MainThread::sendPositionToAllChairs() {
    printf("[INFO] Sending position %d to all chairs...\n", _positionValue);

    std::vector<std::thread> threads;

    for (int i = 0; i < _chairCount; i++) {
        threads.push_back(std::thread([this, i]() {
            sendPositionToChair(i, _positionValue);
        }));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    printf("[INFO] Position command sent to all chairs\n");
}

// 정지 명령을 Chair에 전송
bool MainThread::sendStopToChair(int chairIndex) {
    if (chairIndex < 0 || chairIndex >= _chairCount) {
        return false;
    }

    std::string ip = _dev[chairIndex].getIp();
    if (ip.empty()) {
        return false;
    }

    // URL 생성 (stop 명령)
    char url[256];
    sprintf(url, "http://%s/v1/api/ces/stop", ip.c_str());

    // HTTP GET 요청
    HttpClient client;
    client.setTimeout(2);
    std::string response;

    if (!client.get(url, response)) {
        printf("[ERROR] Failed to send stop to Chair[%d] %s\n", chairIndex, ip.c_str());
        return false;
    }

    printf("[INFO] Stop command sent to Chair[%d] %s\n", chairIndex, ip.c_str());
    return true;
}

// 모든 Chair에 정지 명령 전송
void MainThread::sendStopToAllChairs() {
    printf("[INFO] Sending stop command to all chairs...\n");

    std::vector<std::thread> threads;

    for (int i = 0; i < _chairCount; i++) {
        threads.push_back(std::thread([this, i]() {
            sendStopToChair(i);
        }));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    printf("[INFO] Stop command sent to all chairs\n");
}

// 재부팅 명령을 Chair에 전송
bool MainThread::sendRebootToChair(int chairIndex) {
    if (chairIndex < 0 || chairIndex >= _chairCount) {
        return false;
    }

    std::string ip = _dev[chairIndex].getIp();
    if (ip.empty()) {
        return false;
    }

    // URL 생성 (reboot 명령)
    char url[256];
    sprintf(url, "http://%s/v1/api/ces/reboot", ip.c_str());

    // HTTP GET 요청
    HttpClient client;
    client.setTimeout(2);
    std::string response;

    if (!client.get(url, response)) {
        printf("[ERROR] Failed to send reboot to Chair[%d] %s\n", chairIndex, ip.c_str());
        return false;
    }

    printf("[INFO] Reboot command sent to Chair[%d] %s\n", chairIndex, ip.c_str());
    return true;
}

// 모든 Chair에 재부팅 명령 전송
void MainThread::sendRebootToAllChairs() {
    printf("[INFO] Sending reboot command to all chairs...\n");

    std::vector<std::thread> threads;

    for (int i = 0; i < _chairCount; i++) {
        threads.push_back(std::thread([this, i]() {
            sendRebootToChair(i);
        }));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    printf("[INFO] Reboot command sent to all chairs\n");
}
