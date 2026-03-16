# AGENTS.md — phevctl

Guidance for AI agents and contributors working on this codebase.

## Purpose

phevctl is a command-line tool for controlling a Mitsubishi Outlander PHEV
over WiFi. It communicates with the car's onboard WiFi module using the
protocol implemented in [phevcore](https://github.com/phev-remote/phevcore).

## Project layout

```
.
├── src/
│   ├── main.c              # Main executable (phevctl)
│   ├── decode.c            # Decode utility executable
│   └── phevargs.c          # Argument parsing (argp)
├── include/
│   └── phevargs.h          # Public header for phevargs
├── test/
│   ├── CMakeLists.txt       # Test targets (phevctl_add_test helper)
│   └── test_phev_args.c     # Argument parsing tests
├── CMakeLists.txt           # Root build (FetchContent for phevcore + greatest)
├── CMakePresets.json         # dev / release / ci presets
├── Dockerfile               # Preset-based container build
└── .github/workflows/ci.yml # GitHub Actions CI
```

## Dependencies

All managed via CMake FetchContent — no manual installation required.

| Dependency | Fetched by | Purpose |
|---|---|---|
| phevcore | phevctl | PHEV protocol library |
| cJSON | phevcore (transitive) | JSON parsing |
| msg-core | phevcore (transitive) | Message transport |
| greatest | phevctl (test only) | Test framework |

## Build and test

```bash
# Configure + build
cmake --preset dev
cmake --build --preset dev

# Run all tests (phevctl + phevcore)
ctest --preset dev

# Release build
cmake --preset release
cmake --build --preset release
```

The `dev` preset enables `-Wall -Wextra -Werror`, debug symbols, and
`BUILD_TESTS=ON`. The `ci` preset matches `dev` for reproducible CI runs.

## Test suites

| Suite | File | Tests | Notes |
|---|---|---|---|
| test_phev_args | test/test_phev_args.c | 19 | Argument parsing |

phevcore tests also run since it is fetched with `BUILD_TESTS=ON`.
The `test_phev_pipe` suite (from phevcore) takes ~7 seconds due to
connection retry timeouts — this is expected.

## CI

GitHub Actions (`.github/workflows/ci.yml`) runs on push/PR to `main`:
configure, build, and test using the `ci` preset.

## Language and conventions

- **C11** standard
- **Formatting**: `.clang-format` in repo root (matches phevcore)
- **Include style**:
  - System/library headers: angle brackets (`<phev/phev.h>`, `<cjson/cJSON.h>`)
  - Project headers: double quotes (`"phevargs.h"`)
- **Naming**: `snake_case` for functions, variables, files
- **Types**: Use fixed-width types (`uint8_t`, `uint16_t`) for protocol data
- **Error handling**: Return error codes; don't call `exit()` from library code
  (argp uses `ARGP_NO_EXIT` flag for testability)

## Testing conventions

- Framework: [greatest](https://github.com/silentbicycle/greatest) v1.5.0
- Pattern: `SUITE` grouping with standalone `main()` using `GREATEST_MAIN_BEGIN/END`
- Each test file is a standalone executable
- Test helper in `test/CMakeLists.txt`: `phevctl_add_test(name sources... [LIBS libs...])`
- The `phevargs_lib` static library target exists solely for testing phevargs
  without linking the full executable

## Known quirks

- The `test_phev_pipe` suite (phevcore) has ~7s timeout for connection retries
- argp normally calls `exit()` on parse errors; we pass
  `ARGP_NO_EXIT | ARGP_NO_ERRS` to `argp_parse` so tests can verify error cases
  without killing the test process

## Editing guidance

- Run `cmake --build --preset dev && ctest --preset dev` after any change
- Keep phevcore include paths as `<phev/...>` and `<msg/...>` (not bare headers)
- When adding new source files, update `CMakeLists.txt` target sources
- When adding new tests, use the `phevctl_add_test()` helper in `test/CMakeLists.txt`
