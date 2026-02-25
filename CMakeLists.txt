cmake_minimum_required(VERSION 3.28.3)
project(MultithreadedServer)

add_executable(server src/server_entry.cpp src/Server.cpp)
add_executable(client src/client_entry.cpp src/Client.cpp)

target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include)

target_link_libraries(server PRIVATE pthread)