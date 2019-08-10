project("Mitstubishi Outlander PHEV Adapter CLI" C)
cmake_minimum_required(VERSION 3.0)

option(MQTT_PAHO "Include MQTT Paho support (requires msg core to be compiled with Paho support)")

set (CMAKE_C_STANDARD 11)

if(MQTT_PAHO)
    find_library(PAHO_LIB paho-mqtt3c-static REQUIRED)
    set(CMAKE_C_FLAGS "-DMQTT_PAHO=true")
endif()

find_library(PHEV_LIB phev)
find_library(MSG_CORE_LIB msg_core)
find_library(CJSON_LIB cjson)

add_executable(adapter main.c)

target_link_libraries(adapter "${PHEV_LIB}" "${MSG_CORE_LIB}" "${CJSON_LIB}" "${PAHO_LIB}" pthread)
