message("start to build library xml")
add_library(xml SHARED document.cpp element.cpp)
set(PROJECT_LIB ${PROJECT_LIB} xml PARENT_SCOPE)
message("build library xml over")