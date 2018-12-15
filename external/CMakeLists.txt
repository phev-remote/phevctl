add_library(Unity STATIC
    Unity/src/unity.c
)

target_include_directories(Unity PUBLIC
    Unity/src
)

add_library(msg_core STATIC
    msg_core/src/msg_core.c
    msg_core/src/msg_gcp_mqtt.c
    msg_core/src/msg_mqtt.c
    msg_core/src/msg_pipe_splitter.c
    msg_core/src/msg_pipe.c 
    msg_core/src/msg_tcpip.c 
    msg_core/src/msg_utils.c
)


target_include_directories(msg_core PUBLIC msg_core/include)
