project("Phev Register" C)
cmake_minimum_required(VERSION 3.0)

set (CMAKE_C_STANDARD 11)

add_library(phev_register STATIC
    src/phev_register.c
    src/phev_pipe.c
    src/phev_core.c
)
target_link_libraries(phev_register msg_core)

include(CTest)

add_subdirectory(external) 

target_include_directories(msg_core PUBLIC msg_core/include)

add_subdirectory(test)

target_include_directories(phev_register PUBLIC include)

add_executable(app main.c)
if(WIN32)
    target_link_libraries(app LINK_PUBLIC
        msg_core
        phev_register
        mswsock
        advapi32 
        ws2_32

    )
else()
    target_link_libraries (app LINK_PUBLIC 
        msg_core
        phev_register 
    )
endif()


