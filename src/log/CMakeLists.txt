message("start to build library log")
add_library(log SHARED logger.cpp)
set(PROJECT_LIB ${PROJECT_LIB} log PARENT_SCOPE)
message("build library log over")