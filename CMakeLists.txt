project(TOD)
cmake_minimum_required(VERSION 2.8)

set(LIBTOR_DIR "./libtorrent")
set(SRC_DIR ".")

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${SRC_DIR} ${LIBTOR_DIR}
                    ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -g -std=c++11 -Wall -O2")

add_executable(tod main.cpp
                   txt.cpp
                   server.h
                   server.cpp)

target_link_libraries(tod ${Boost_LIBRARIES})
