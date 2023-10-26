cmake_minimum_required(VERSION 3.10.0)
project(tubekit-ecsactor)

aux_source_directory(. SRCS)

include_directories(.)

set(CMAKE_CXX_FLAGS "-Wall -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

add_executable(main.exe ${SRCS})
target_link_libraries(main.exe uuid)