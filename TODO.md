# Modernization TODO

Tracks the multi-phase modernization of the phevctl project,
following the same approach used for phevcore. Each phase is one PR.

## Phase 1 — Build modernization

- [ ] Rewrite `CMakeLists.txt`
  - Bump minimum to 3.14 (3.21 for presets)
  - Fix `cmake_minimum_required` / `project()` order
  - Use `FetchContent` for phevcore, cJSON, argp
  - Fix project name typo ("Mitstubishi" → "Mitsubishi")
  - Remove `TARGET_GROUP` test/production split — tests always buildable
- [ ] Add `CMakePresets.json` (dev / release / ci)
- [ ] Delete dead files (`.travis.yml`, `.gitmodules`)
- [ ] Add GitHub Actions workflow (`.github/workflows/ci.yml`)
- [ ] Update Dockerfile to use cmake presets
- [ ] Expand `.gitignore` (fix `*.txt` glob, match phevcore pattern)
- [ ] Add `.clang-format` (copy from phevcore)

## Phase 2 — Test migration

- [ ] Replace Unity with **greatest** (v1.5.0) via FetchContent
- [ ] Convert `test_phev_args.c` to greatest style (standalone `main()`, `SUITE` macros)
- [ ] Wire dead test functions (`headlights_invalid_operand`, `aircon_invalid_operand`)
- [ ] Delete Unity scaffolding (`test_runner.c`, `tests.h`)
- [ ] Update `test/CMakeLists.txt` (use `phevctl_add_test()` helper)
- [ ] Verify all tests pass

## Phase 3 — Documentation & polish

- [ ] Rewrite `README.md` (concise, CI badge, current build/usage instructions)
- [ ] Merge `BUILD.md` into README or remove
- [ ] Add `AGENTS.md` with phevctl-specific conventions
- [ ] Add `LICENSE` file (MIT — currently only in README text)
- [ ] Rename default branch `master` → `main`

## Phase 4 — Code cleanup

- [ ] Move `main.c` and `decode.c` into `src/`
- [ ] Evaluate and remove `MQTT_PAHO` conditional compilation if dead code
- [ ] Clean up include paths (use subdirectory-prefixed style)
- [ ] Remove dead/commented-out code in `decode.c`
- [ ] Fix version string inconsistency (`main.c` says 0.0.1, README says 0.1, tag is v0.1)
