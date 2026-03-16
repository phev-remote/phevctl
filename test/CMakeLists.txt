# ── Helper to define a test executable ──────────────────────────
function(phevctl_add_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE phevargs_lib)
    target_include_directories(${name} PRIVATE
        ${greatest_SOURCE_DIR}
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# ── Static library for code under test ─────────────────────────
add_library(phevargs_lib STATIC
    ../src/phevargs.c
)
target_include_directories(phevargs_lib PUBLIC ../include)

# ── Per-suite test executables (all use greatest) ──────────────
phevctl_add_test(test_phev_args test_phev_args.c)
