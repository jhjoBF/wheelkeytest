#include "HttpClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <cstring>
#include <cstdio>
#include <errno.h>

HttpClient::HttpClient() : _timeout(2) {}

HttpClient::~HttpClient() {}

bool HttpClient::parseUrl(const std::string& url, std::string& host, int& port, std::string& path) {
    // URL 형식: http://192.168.0.22/v1/api/ces/info 또는 192.168.0.22/v1/api/ces/info
    std::string workUrl = url;
    
    // http:// 제거
    size_t httpPos = workUrl.find("http://");
    if (httpPos != std::string::npos) {
        workUrl = workUrl.substr(7);
    }
    
    // 호스트와 경로 분리
    size_t slashPos = workUrl.find("/");
    if (slashPos == std::string::npos) {
        host = workUrl;
        path = "/";
    } else {
        host = workUrl.substr(0, slashPos);
        path = workUrl.substr(slashPos);
    }
    
    // 포트 분리
    size_t colonPos = host.find(":");
    if (colonPos != std::string::npos) {
        port = std::stoi(host.substr(colonPos + 1));
        host = host.substr(0, colonPos);
    } else {
        port = 80;
    }
    
    return true;
}

bool HttpClient::connectToHost(const std::string& host, int port, int& sockfd) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("[ERROR] Socket creation failed\n");
        return false;
    }
    
    // 타임아웃 설정 (더 짧게)
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000; // 0.5초
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    // 논블로킹 모드 설정
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        printf("[ERROR] Invalid address: %s\n", host.c_str());
        close(sockfd);
        return false;
    }
    
    // 논블로킹 connect
    int result = connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result < 0) {
        if (errno != EINPROGRESS) {
            printf("[ERROR] Connection failed to %s:%d\n", host.c_str(), port);
            close(sockfd);
            return false;
        }
        
        // select로 연결 완료 대기
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        if (select(sockfd + 1, NULL, &fdset, NULL, &tv) <= 0) {
            printf("[ERROR] Connection timeout to %s:%d\n", host.c_str(), port);
            close(sockfd);
            return false;
        }
        
        // 연결 상태 확인
        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
            printf("[ERROR] Connection failed to %s:%d\n", host.c_str(), port);
            close(sockfd);
            return false;
        }
    }
    
    // 다시 블로킹 모드로 변경
    fcntl(sockfd, F_SETFL, flags);
    
    return true;
}

bool HttpClient::sendRequest(int sockfd, const std::string& host, const std::string& path) {
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    
    ssize_t sent = send(sockfd, request.c_str(), request.length(), 0);
    if (sent < 0) {
        printf("[ERROR] Send failed\n");
        return false;
    }
    
    return true;
}

bool HttpClient::receiveResponse(int sockfd, std::string& response) {
    char buffer[4096];
    response.clear();
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        
        if (received < 0) {
            // EAGAIN이나 EWOULDBLOCK은 타임아웃으로 간주
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 타임아웃 발생, 받은 데이터가 있으면 성공으로 처리
                if (!response.empty()) {
                    break;
                }
            }
            printf("[ERROR] Receive failed (errno: %d)\n", errno);
            return false;
        }
        
        if (received == 0) {
            // 연결 종료
            break;
        }
        
        response.append(buffer, received);
    }
    
    // HTTP 헤더와 본문 분리
    size_t bodyPos = response.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        response = response.substr(bodyPos + 4);
    }
    
    return !response.empty();
}

bool HttpClient::get(const std::string& url, std::string& response) {
    std::string host, path;
    int port;
    
    if (!parseUrl(url, host, port, path)) {
        printf("[ERROR] URL parse failed: %s\n", url.c_str());
        return false;
    }
    
    int sockfd;
    if (!connectToHost(host, port, sockfd)) {
        return false;
    }
    
    if (!sendRequest(sockfd, host, path)) {
        close(sockfd);
        return false;
    }
    
    if (!receiveResponse(sockfd, response)) {
        close(sockfd);
        return false;
    }
    
    close(sockfd);
    return true;
}
