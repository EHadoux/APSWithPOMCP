cmake_minimum_required(VERSION 3.3)
project(POMCP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

FILE(GLOB SOURCE src/*.cpp)
set(SOURCE_FILES ${SOURCE})
FILE(GLOB HEADER include/*.h)
set(HEADER_FILES ${HEADER})

add_executable(POMCP ${HEADER_FILES} ${SOURCE_FILES})
