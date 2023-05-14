message("start to build library timer")
add_library(timer SHARED timer.cpp timer_manager.cpp)
set(PROJECT_LIB ${PROJECT_LIB} timer PARENT_SCOPE)
message("build library timer over")