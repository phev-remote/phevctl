project("Mitstubishi Outlander PHEV Adapter CLI" C)
cmake_minimum_required(VERSION 3.0)

include(CTest)

option(MQTT_PAHO "Include MQTT Paho support (requires msg core to be compiled with Paho support)")

set (CMAKE_C_STANDARD 11)
if (UNIX)
  add_definitions (-D__unix__)
endif()

set(TARGET_GROUP production CACHE STRING "Group to build")

find_library(ARGP_LIB argp)
find_library(PHEV_LIB phev)
find_library(MSG_CORE_LIB msg_core)
find_library(CJSON_LIB cjson)

if(MQTT_PAHO)
    find_library(PAHO_LIB paho-mqtt3c-static REQUIRED)
    set(CMAKE_C_FLAGS "-DMQTT_PAHO=true")
endif()
if(TARGET_GROUP STREQUAL production)
    add_executable(phevctl main.c
        src/phevargs.c
    )
    add_executable(decode decode.c)
    target_include_directories(phevctl PUBLIC include ${ARGP} /usr/local/include)
    target_include_directories(decode PUBLIC include /usr/local/include)
    target_link_libraries(phevctl "${PHEV_LIB}" "${MSG_CORE_LIB}" "${CJSON_LIB}" "${PAHO_LIB}" pthread "${ARGP_LIB}")
    target_link_libraries(decode "${PHEV_LIB}" "${MSG_CORE_LIB}" "${CJSON_LIB}" )
elseif(TARGET_GROUP STREQUAL test)
    add_subdirectory(test)
ENDIF()




