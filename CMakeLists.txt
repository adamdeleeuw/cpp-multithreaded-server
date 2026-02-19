cmake_minimum_required(VERSION 3.28.3)
project(MultithreadedServer)

set(CMAKE_CXX_STANDARD 17)          # Explicitly use C++17  
set(CMAKE_CXX_STANDARD_REQUIRED ON) # build fails if compiler doesn't support C++17

add_executable(server src/server_entry.cpp src/Server.cpp)
add_executable(client src/client_entry.cpp src/Client.cpp)

target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include)

target_link_libraries(server PRIVATE pthread)