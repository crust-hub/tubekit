cmake_minimum_required(VERSION 3.10.0)
project(tubekit)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++2a -Wall -O2")

#link_directories("/usr/lib")
#FIND_LIBRARY(RUNTIME_LIB http_parser /usr/lib  /usr/local/lib NO_DEFAULT_PATH)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories("${PROJECT_SOURCE}/src")

FILE(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

FILE(GLOB_RECURSE SOURCE_PLUGIN "${PROJECT_SOURCE_DIR}/src/plugin/*.cpp")
LIST(REMOVE_ITEM SOURCE_FILES ${SOURCE_PLUGIN})

ADD_EXECUTABLE(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/src/plugin)

target_link_libraries(${PROJECT_NAME} pthread dl http_parser)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)