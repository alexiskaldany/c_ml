#include "http_server.h"
#include "linear_regression.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

HttpServer::HttpServer(int port) : port_(port), server_fd_(-1), running_(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    
    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to set socket options");
    }
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to bind socket");
    }
    
    if (listen(server_fd_, 10) < 0) {
        close(server_fd_);
        throw std::runtime_error("Failed to listen on socket");
    }
    
    running_ = true;
    std::cout << "Server listening on port " << port_ << std::endl;
    
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (running_) {
                std::cerr << "Failed to accept connection" << std::endl;
            }
            continue;
        }
        
        handleClient(client_socket);
    }
}

void HttpServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

void HttpServer::handleClient(int client_socket) {
    char buffer[4096] = {0};
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }
    
    std::string request(buffer, bytes_read);
    std::string response;
    
    // Parse HTTP request
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;
    
    if (method == "POST" && path == "/linear-regression") {
        // Find the JSON body (after the headers)
        size_t body_start = request.find("\r\n\r\n");
        if (body_start == std::string::npos) {
            body_start = request.find("\n\n");
            if (body_start != std::string::npos) {
                body_start += 2;
            }
        } else {
            body_start += 4;
        }
        
        if (body_start != std::string::npos && body_start < request.length()) {
            std::string body = request.substr(body_start);
            
            try {
                // Simple JSON parsing for array of {x, y} objects
                std::vector<std::pair<double, double>> points;
                
                // Find the "points" array
                size_t points_start = body.find("[");
                size_t points_end = body.find("]");
                
                if (points_start != std::string::npos && points_end != std::string::npos) {
                    std::string points_str = body.substr(points_start + 1, points_end - points_start - 1);
                    
                    // Parse each point object
                    size_t pos = 0;
                    while (pos < points_str.length()) {
                        size_t obj_start = points_str.find("{", pos);
                        if (obj_start == std::string::npos) break;
                        
                        size_t obj_end = points_str.find("}", obj_start);
                        if (obj_end == std::string::npos) break;
                        
                        std::string obj = points_str.substr(obj_start, obj_end - obj_start + 1);
                        
                        // Extract x and y values
                        double x = 0.0, y = 0.0;
                        size_t x_pos = obj.find("\"x\"");
                        size_t y_pos = obj.find("\"y\"");
                        
                        if (x_pos != std::string::npos) {
                            size_t x_colon = obj.find(":", x_pos);
                            if (x_colon != std::string::npos) {
                                size_t x_value_start = x_colon + 1;
                                while (x_value_start < obj.length() && 
                                       (obj[x_value_start] == ' ' || obj[x_value_start] == '\t')) {
                                    x_value_start++;
                                }
                                x = std::stod(obj.substr(x_value_start));
                            }
                        }
                        
                        if (y_pos != std::string::npos) {
                            size_t y_colon = obj.find(":", y_pos);
                            if (y_colon != std::string::npos) {
                                size_t y_value_start = y_colon + 1;
                                while (y_value_start < obj.length() && 
                                       (obj[y_value_start] == ' ' || obj[y_value_start] == '\t')) {
                                    y_value_start++;
                                }
                                y = std::stod(obj.substr(y_value_start));
                            }
                        }
                        
                        points.push_back({x, y});
                        pos = obj_end + 1;
                    }
                }
                
                // Calculate linear regression
                auto result = LinearRegression::calculate(points);
                
                // Create JSON response
                std::ostringstream json;
                json << "{";
                json << "\"slope\": " << result.slope << ",";
                json << "\"intercept\": " << result.intercept << ",";
                json << "\"r_squared\": " << result.r_squared;
                json << "}";
                
                response = createResponse(200, "application/json", json.str());
            } catch (const std::exception& e) {
                std::ostringstream json;
                json << "{\"error\": \"" << e.what() << "\"}";
                response = createResponse(400, "application/json", json.str());
            }
        } else {
            std::string error_body = "{\"error\": \"No body found in request\"}";
            response = createResponse(400, "application/json", error_body);
        }
    } else if (method == "GET" && path == "/") {
        std::string html = R"(<!DOCTYPE html>
<html>
<head>
    <title>Linear Regression Server</title>
</head>
<body>
    <h1>Linear Regression Server</h1>
    <p>POST to /linear-regression with JSON body containing points array</p>
    <p>Example: {"points": [{"x": 1, "y": 2}, {"x": 2, "y": 4}, {"x": 3, "y": 6}]}</p>
</body>
</html>)";
        response = createResponse(200, "text/html", html);
    } else {
        std::string error_body = "{\"error\": \"Not found\"}";
        response = createResponse(404, "application/json", error_body);
    }
    
    write(client_socket, response.c_str(), response.length());
    close(client_socket);
}

std::string HttpServer::parseRequest(const std::string& request) {
    return "";
}

std::string HttpServer::createResponse(int status_code, const std::string& content_type, const std::string& body) {
    std::ostringstream response;
    
    std::string status_text;
    switch (status_code) {
        case 200: status_text = "OK"; break;
        case 400: status_text = "Bad Request"; break;
        case 404: status_text = "Not Found"; break;
        case 500: status_text = "Internal Server Error"; break;
        default: status_text = "Unknown"; break;
    }
    
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;
    
    return response.str();
}
