find_package(unity)

enable_testing()


add_library(phevctl STATIC
    ../src/phevargs.c
)

target_include_directories(phevctl PUBLIC ../include)

add_library(test_runner OBJECT test_runner.c)

target_link_libraries(test_runner
    phevctl
    unity
)
add_executable(test_phev_args test_phev_args.c $<TARGET_OBJECTS:test_runner>)
target_link_libraries(test_phev_args
    phevctl
    unity
)
add_test(NAME phev_args COMMAND test_phev_args)





