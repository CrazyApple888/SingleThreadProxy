cmake_minimum_required(VERSION 3.16)
project(SingleThreadProxy)

set(CMAKE_CXX_STANDARD 11)

add_executable(SingleThreadProxy main.cpp Logger.cpp Proxy.cpp Cache.cpp http_parser.c CacheEntity.cpp Server.cpp Client.cpp)