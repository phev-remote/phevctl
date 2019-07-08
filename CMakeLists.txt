project("Phev Adapter" C)
cmake_minimum_required(VERSION 3.0)

set (CMAKE_C_STANDARD 11)

find_library(PHEV_LIB phev)
find_library(MSG_CORE_LIB msg_core)
find_library(CJSON_LIB cjson)

include(CTest)

add_executable(adapter main.c)

target_link_libraries(adapter "${PHEV_LIB}" "${MSG_CORE_LIB}" "${CJSON_LIB}")
