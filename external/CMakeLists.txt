message("start to build library http-parser")
add_library(http-parser SHARED ./http-parser/http_parser.c)
set(EXTERNAL_LIB ${EXTERNAL_LIB} http-parser PARENT_SCOPE)
message("build library http-parser over")