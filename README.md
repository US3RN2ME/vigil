# vigil

`vigil` is a cross-platform EDR agent. It collects process and runtime telemetry, evaluates it against built-in detection rules, and logs alerts for suspicious activity such as server-spawned shells, suspicious execution paths, fileless execution, anonymous RWX mappings, privilege escalation, preload hijacking, and binary replacement.

The project builds a reusable `vigil_core` library and, by default, the `vigil` executable.

## Requirements

Common requirements:

- CMake 3.28 or newer
- A C++23 compiler
- vcpkg manifest mode, or otherwise available CMake packages for:
  - `spdlog`
  - `nlohmann-json`
  - `bext-ut` when `VIGIL_BUILD_TESTS=ON`

Windows requirements:

- Visual Studio 2022 or newer with MSVC C++ tools
- Windows SDK
- System libraries used by the build: `ntdll`, `psapi`, `advapi32`, `iphlpapi`, `ws2_32`

Linux requirements:

- `pkg-config`
- `libbpf`
- `libelf`
- `zlib`
- `clang` with BPF target support
- `bpftool`
- A kernel exposing BTF at `/sys/kernel/btf/vmlinux`
- Permissions/capabilities sufficient to load and run eBPF programs

## Build

Configure and build with CMake:

```sh
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVIGIL_BUILD_TESTS=OFF \
  -DVIGIL_ENABLE_INSTALL=OFF
cmake --build build
```
On Linux, the build generates `src/platform/linux/bpf/vmlinux.h`, compiles `events.bpf.c`, and generates the BPF skeleton with `bpftool`.

## CMake Options

- `VIGIL_BUILD_APP=ON` builds the `vigil` executable.
- `VIGIL_BUILD_TESTS=ON` builds tests and enables the vcpkg `tests` feature.
- `VIGIL_ENABLE_INSTALL=ON` enables install rules for the library, executable, headers, package config files, and default config.

## Usage

The default config is copied from `config/platform/<platform>/config.json` into the application build directory as `config.json`. Run the built executable:

```sh
./build/app/vigil
```

Stop the agent with `Ctrl+C` or the platform's normal console close/termination signal.

For production-like collection, start the agent with elevated privileges: Administrator on Windows for ETW, and root or equivalent capabilities on Linux for eBPF.
