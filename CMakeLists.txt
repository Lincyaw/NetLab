cmake_minimum_required(VERSION 3.17)
project(NetLab)

set(CMAKE_CXX_STANDARD 14)

add_executable(NetLab main.cpp server.cpp server.h client.cpp client.h)