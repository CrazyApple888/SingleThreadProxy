cmake_minimum_required(VERSION 3.20)
project(SingleThreadProxy)

set(CMAKE_CXX_STANDARD 11)

add_executable(SingleThreadProxy main.cpp Logger.h Logger.cpp Proxy.h Proxy.cpp Cache.cpp Cache.h http_parser.h http_parser.c CacheEntity.cpp CacheEntity.h Handler.h Server.cpp Server.h Client.cpp Client.h)
