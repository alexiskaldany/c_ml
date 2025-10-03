cmake_minimum_required(VERSION 3.10)
project(LinearRegressionServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(server
    src/main.cpp
    src/linear_regression.cpp
    src/http_server.cpp
)

# Include directories
target_include_directories(server PRIVATE include)

# Link pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
