#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // HTTP GET 요청
    bool get(const std::string& url, std::string& response);
    
    // 타임아웃 설정 (초 단위)
    void setTimeout(int seconds) { _timeout = seconds; }
    
private:
    int _timeout;
    
    bool parseUrl(const std::string& url, std::string& host, int& port, std::string& path);
    bool connectToHost(const std::string& host, int port, int& sockfd);
    bool sendRequest(int sockfd, const std::string& host, const std::string& path);
    bool receiveResponse(int sockfd, std::string& response);
};

#endif  // HTTP_CLIENT_H_
