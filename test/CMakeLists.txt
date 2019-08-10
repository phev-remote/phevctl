find_library(UNITY_LIB Unity)

add_executable(test_runner
    test_runner.c
)

target_link_libraries(test_runner
    adapter
    "${PHEV_LIB}" 
    "${MSG_CORE_LIB}" 
    "${CJSON_LIB}" 
    pthread
    ${UNITY_LIB}
)

add_test(test_adapter test_runner)
