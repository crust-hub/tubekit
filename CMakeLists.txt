cmake_minimum_required(VERSION 3.10.0)
project(tubekit)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -Wall -O0")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

message(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src)
message(${PROJECT_SOURCE_DIR}/external/http-parser)

include_directories(${PROJECT_SOURCE_DIR}/external)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB_RECURSE SOURCE_PLUGIN "${PROJECT_SOURCE_DIR}/src/plugin/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_PLUGIN})

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

set(EXTERNAL_LIB http-parser tubekit-inifile tubekit-log tubekit-timer tubekit-xml tubekit-buffer)
target_link_libraries(${PROJECT_NAME} pthread dl ${EXTERNAL_LIB})

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
