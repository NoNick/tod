project(TOD)
cmake_minimum_required(VERSION 2.8)

set(SRC_DIR ".")

find_package(Boost COMPONENTS program_options system chrono random REQUIRED)
include(FindPkgConfig)
pkg_search_module(LIBTORRENT REQUIRED libtorrent-rasterbar)
pkg_search_module(TBB REQUIRED tbb)

include_directories(${LIBTORRENT_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
		    ${OpenMP_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TBB_CXX_FLAGS} -pedantic -g -std=c++11 -Wall -O0")

set(UI_SRC ui/screen.cpp
           ui/progress.cpp
	   ui/label.cpp
	   ui/text_area.cpp)

add_executable(tod main.cpp
                   txt.cpp
                   server.cpp
		   obtainer.cpp
		   helpers.cpp
		   remote_interface.cpp
		   mirror_storage.cpp
		   piece_watcher.cpp
		   swimming_iov.cpp
		   ${UI_SRC})

add_executable(toc client.cpp
		   remote_interface.cpp
		   helpers.cpp
		   piece_watcher.cpp
		   swimming_iov.cpp
		   ${UI_SRC})

target_link_libraries(tod ${Boost_LIBRARIES} ${LIBTORRENT_LIBRARIES} ${TBB_LIBRARIES})
target_link_libraries(toc ${Boost_LIBRARIES} ${LIBTORRENT_LIBRARIES})
