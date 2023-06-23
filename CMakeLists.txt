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
file(GLOB_RECURSE SOURCE_INIFILE "${PROJECT_SOURCE_DIR}/src/inifile/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_INIFILE})
file(GLOB_RECURSE SOURCE_LOG "${PROJECT_SOURCE_DIR}/src/log/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_LOG})
file(GLOB_RECURSE SOURCE_TIMER "${PROJECT_SOURCE_DIR}/src/timer/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_TIMER})
file(GLOB_RECURSE SOURCE_XML "${PROJECT_SOURCE_DIR}/src/xml/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_XML})
file(GLOB_RECURSE SOURCE_BUFFER "${PROJECT_SOURCE_DIR}/src/buffer/*.cpp")
list(REMOVE_ITEM SOURCE_FILES ${SOURCE_BUFFER})

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_FILES})

add_subdirectory(${PROJECT_SOURCE_DIR}/src/plugin)

set(EXTERNAL_LIB)
add_subdirectory(${PROJECT_SOURCE_DIR}/external)
set(PROJECT_LIB)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/inifile)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/log)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/timer)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/xml)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/buffer)

message(${EXTERNAL_LIB})
message(${PROJECT_LIB})
target_link_libraries(${PROJECT_NAME} pthread dl ${EXTERNAL_LIB} ${PROJECT_LIB})

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
