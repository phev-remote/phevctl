project("Phev Adapter" C)
cmake_minimum_required(VERSION 3.0)

set (CMAKE_C_STANDARD 11)

find_library(PAHO_LIB paho-mqtt3c-static)
find_library(PHEV_LIB phev)
find_library(MSG_CORE_LIB msg_core)
find_library(CJSON_LIB cjson)
find_library(CURL_LIB curl)
find_library(SSL_LIB ssl)
find_library(CRYPTO_LIB crypto)
#find_library(pthread)


find_package(CURL)

include(CTest)

add_executable(adapter main.c)

target_link_libraries(adapter "${PHEV_LIB}" "${MSG_CORE_LIB}" "${CJSON_LIB}" "${CURL_LIB}" "${SSL_LIB}" "${CRYPTO_LIB}" "${PAHO_LIB}" pthread)
