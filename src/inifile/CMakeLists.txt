message("start to build library inifile")
add_library(inifile SHARED inifile.cpp value.cpp)
set(PROJECT_LIB ${PROJECT_LIB} inifile PARENT_SCOPE)
message("build library inifile over")
