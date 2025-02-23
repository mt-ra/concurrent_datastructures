cmake_minimum_required(VERSION 3.10)
project(datastructures)

include_directories(include)
include_directories(/usr/local/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")

set(SOURCES
    main.cpp
)

add_executable(datastructures ${SOURCES})