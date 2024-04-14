message("external start")
add_library(tubekit-http-parser SHARED "./http-parser/http_parser.c")

file(GLOB_RECURSE SOURCE_INIFILE "./tubekit-inifile/*.cpp")
add_library(tubekit-inifile SHARED ${SOURCE_INIFILE})


file(GLOB_RECURSE SOURCE_LOG "./tubekit-log/*.cpp")
add_library(tubekit-log SHARED ${SOURCE_LOG})

file(GLOB_RECURSE SOURCE_TIMER "./tubekit-timer/*.cpp")
add_library(tubekit-timer SHARED ${SOURCE_TIMER})

file(GLOB_RECURSE SOURCE_XML "./tubekit-xml/*.cpp")
add_library(tubekit-xml SHARED ${SOURCE_XML})

file(GLOB_RECURSE SOURCE_BUFFER "./tubekit-buffer/*.cpp")
add_library(tubekit-buffer SHARED ${SOURCE_BUFFER})

file(GLOB_RECURSE SOURCE_LUA "./lua/*.c")
add_library(tubekit-lua SHARED ${SOURCE_LUA})

file(GLOB_RECURSE SOURCE_ZLIB "./zlib/*.c")
add_library(tubekit-zlib SHARED ${SOURCE_ZLIB})

file(GLOB_RECURSE SOURCE_JSON "./tubekit-json/*.cpp")
add_library(tubekit-json SHARED ${SOURCE_JSON})

message("external end")
