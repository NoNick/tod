project(TOD)
cmake_minimum_required(VERSION 2.8)

set(SRC_DIR ".")

find_package(OpenMP REQUIRED)
find_package(Boost COMPONENTS program_options system chrono random REQUIRED)
include(FindPkgConfig)
pkg_search_module(LIBTORRENT REQUIRED libtorrent-rasterbar)

include_directories(${LIBTORRENT_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
		    ${OpenMP_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -pedantic -g -std=c++11 -Wall -O0")

set(UI_SRC ui/screen.cpp
           ui/progress.cpp
	   ui/label.cpp
	   ui/text_area.cpp)

add_executable(tod main.cpp
                   txt.cpp
                   server.h
                   server.cpp
		   obtainer.h
		   obtainer.cpp
		   helpers.h
		   helpers.cpp
		   remote_interface.h
		   remote_interface.cpp
		   mirror_storage.h
		   mirror_storage.cpp
		   piece_watcher.cpp
		   ${UI_SRC})

add_executable(toc client.cpp
		   remote_interface.h
		   remote_interface.cpp
		   helpers.h
		   helpers.cpp
		   piece_watcher.cpp
		   ${UI_SRC})

target_link_libraries(tod ${Boost_LIBRARIES} ${LIBTORRENT_LIBRARIES} ${OpenMP_LIBRARIES})
target_link_libraries(toc ${Boost_LIBRARIES} ${LIBTORRENT_LIBRARIES} ${OpenMP_LIBRARIES})
