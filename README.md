# phevctl

[![CI](https://github.com/phev-remote/phevctl/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/phev-remote/phevctl/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Command-line tool for controlling a Mitsubishi Outlander PHEV over WiFi.

## Dependencies

All fetched automatically via CMake FetchContent:

- [phevcore](https://github.com/phev-remote/phevcore) (transitively brings cJSON and msg-core)
- [greatest](https://github.com/silentbicycle/greatest) (tests only)

## Build

```bash
cmake --preset dev
cmake --build --preset dev
```

## Test

```bash
ctest --preset dev
```

## Install

```bash
cmake --preset release
cmake --build --preset release
sudo cmake --install build/release
```

## Docker

```bash
docker build -t phevctl .
docker run --rm --network host phevctl <command>
```

## Usage

Connect to the car's WiFi access point, then:

```bash
# Show battery level
phevctl battery

# Turn headlights on/off
phevctl headlights on
phevctl headlights off

# Start/stop air conditioning
phevctl aircon on
phevctl aircon off

# Get help
phevctl --help
```

## License

[MIT](LICENSE)
