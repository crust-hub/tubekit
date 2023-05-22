message("start to build library buffer")
add_library(buffer SHARED buffer.cpp)
set(PROJECT_LIB ${PROJECT_LIB} buffer PARENT_SCOPE)
message("build library buffer over")