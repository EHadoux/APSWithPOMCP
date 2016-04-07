cmake_minimum_required(VERSION 3.3)
project(POMCP)

set(BOOST_ROOT "/usr/lib/x86_64-linux-gnu")
find_package(Boost REQUIRED COMPONENTS program_options)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O0 -g")

include_directories(include ${BOOST_INCLUDE_DIRS})

FILE(GLOB SOURCE src/*.cpp)
set(SOURCE_FILES ${SOURCE})
FILE(GLOB HEADER include/*.h)
set(HEADER_FILES ${HEADER})

add_executable(POMCP ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(POMCP ${Boost_LIBRARIES})
