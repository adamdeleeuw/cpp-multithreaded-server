cmake_minimum_required(VERSION 3.28.3)
project(MultithreadedServer)

add_executable(server utils/server_entry.cpp src/Server.cpp)
add_executable(client utils/client_entry.cpp src/Client.cpp)

target_include_directories(server PRIVATE include)
target_include_directories(client PRIVATE include)

target_link_libraries(server PRIVATE pthread)