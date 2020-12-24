cmake_minimum_required(VERSION 3.0)
project(lab)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)


find_package(Threads REQUIRED)

add_executable(server server_test.cpp)
target_sources(server PRIVATE lib/client.cpp lib/server.cpp)
target_include_directories(server PUBLIC lib)
target_link_libraries(server PRIVATE Threads::Threads)

add_executable(client client_test.cpp)
target_sources(client PRIVATE lib/client.cpp lib/server.cpp)
target_include_directories(client PUBLIC lib)
target_link_libraries(client PRIVATE Threads::Threads)
