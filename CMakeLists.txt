cmake_minimum_required(VERSION 3.14)
project(phevctl VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(UNIX)
    add_definitions(-D__unix__)
endif()

# ---------- dependencies via FetchContent ----------
include(FetchContent)

FetchContent_Declare(phevcore
    GIT_REPOSITORY https://github.com/phev-remote/phevcore.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(phevcore)

# argp: part of glibc on Linux, needs a separate library on macOS/BSD
find_library(ARGP_LIB argp)

# ---------- phevctl executable ----------
add_executable(phevctl
    src/main.c
    src/phevargs.c
)

target_include_directories(phevctl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(phevctl PRIVATE phev pthread)
target_compile_definitions(phevctl PRIVATE
    VERSION="${PROJECT_VERSION}"
)
if(ARGP_LIB)
    target_link_libraries(phevctl PRIVATE ${ARGP_LIB})
endif()

# ---------- decode executable ----------
add_executable(decode src/decode.c)

target_include_directories(decode PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(decode PRIVATE phev)

# ---------- tests ----------
option(BUILD_TESTS "Build the test binaries" OFF)

if(BUILD_TESTS)
    FetchContent_Declare(greatest
        GIT_REPOSITORY https://github.com/silentbicycle/greatest.git
        GIT_TAG        v1.5.0
    )
    FetchContent_MakeAvailable(greatest)

    enable_testing()
    add_subdirectory(test)
endif()
