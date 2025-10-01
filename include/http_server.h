#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <functional>

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();
    
    void start();
    void stop();
    
private:
    int port_;
    int server_fd_;
    bool running_;
    
    void handleClient(int client_socket);
    std::string parseRequest(const std::string& request);
    std::string createResponse(int status_code, const std::string& content_type, const std::string& body);
};

#endif // HTTP_SERVER_H
