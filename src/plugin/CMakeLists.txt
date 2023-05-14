message('cmake plugin')

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/plugin/)
add_library(echoplugin SHARED ${PROJECT_SOURCE_DIR}/src/plugin/echo_plugin.cpp  ${PROJECT_SOURCE_DIR}/src/engine/plugin.cpp)

