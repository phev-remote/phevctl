# Modernization TODO

Tracks the multi-phase modernization of the phevctl project,
following the same approach used for phevcore. Each phase is one PR.

## Phase 1 — Build modernization ✓

- [x] Rewrite `CMakeLists.txt`
  - Bump minimum to 3.14 (3.21 for presets)
  - Fix `cmake_minimum_required` / `project()` order
  - Use `FetchContent` for phevcore (which transitively brings cJSON + msg-core)
  - Fix project name typo ("Mitstubishi" → "Mitsubishi")
  - Remove `TARGET_GROUP` test/production split — tests always buildable
  - Remove `MQTT_PAHO` option (dead code, `msg_mqtt_paho` removed in phevcore Phase 5)
- [x] Add `CMakePresets.json` (dev / release / ci)
- [x] Delete dead files (`.travis.yml`, `.gitmodules`)
- [x] Add GitHub Actions workflow (`.github/workflows/ci.yml`)
- [x] Update Dockerfile to use cmake presets
- [x] Expand `.gitignore` (fix `*.txt` glob, match phevcore pattern)
- [x] Add `.clang-format` (copy from phevcore)
- [x] Update include paths for phevcore restructure (`phev/phev.h`, `msg/msg_utils.h`)

## Phase 2 — Test migration ✓

- [x] Replace Unity with **greatest** (v1.5.0) via FetchContent
- [x] Convert `test_phev_args.c` to greatest style (standalone `main()`, `SUITE` macros)
- [x] Wire dead test functions (`headlights_invalid_operand`, `aircon_invalid_operand`)
- [x] Delete Unity scaffolding (`test_runner.c`, `tests.h`)
- [x] Update `test/CMakeLists.txt` (use `phevctl_add_test()` helper)
- [x] Fix `argp_parse` to use `ARGP_NO_EXIT | ARGP_NO_ERRS` so tests don't abort on error cases
- [x] Verify all 19 tests pass (17 original + 2 previously-dead)

## Phase 3 — Documentation & polish

- [ ] Rewrite `README.md` (concise, CI badge, current build/usage instructions)
- [ ] Merge `BUILD.md` into README or remove
- [ ] Add `AGENTS.md` with phevctl-specific conventions
- [ ] Add `LICENSE` file (MIT — currently only in README text)
- [x] Rename default branch `master` → `main`

## Phase 4 — Code cleanup

- [ ] Move `main.c` and `decode.c` into `src/`
- [x] Evaluate and remove `MQTT_PAHO` conditional compilation — removed (dead code)
- [x] Clean up include paths (use subdirectory-prefixed style)
- [ ] Remove dead/commented-out code in `decode.c`
- [ ] Fix version string inconsistency (`main.c` says 0.0.1, README says 0.1, tag is v0.1)
