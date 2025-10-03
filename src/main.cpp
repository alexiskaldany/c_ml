#include "http_server.h"
#include <iostream>
#include <csignal>

HttpServer* server_ptr = nullptr;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    if (server_ptr) {
        server_ptr->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    int port = 8080;
    
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }
    
    try {
        HttpServer server(port);
        server_ptr = &server;
        
        // Register signal handler
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
