# Vigil Examples

`examples` contains realistic benign attack simulators for validating `vigil`
against process, command-line, memory, privilege, module, and network telemetry.
The examples are intentionally shaped like common intrusion tradecraft, but they
avoid the parts that would make them malware.

## Safety Boundary

The examples may create suspicious telemetry, copy themselves to suspicious
paths, allocate executable memory, attach to child processes, or request
privilege boundaries. They do not implement persistence, stealth, credential
access, destructive behavior, self-replication, real payload downloads, or data
exfiltration.

Run them in a lab VM while the `vigil` agent is already running. Some scenarios
need root, Administrator, sudo, UAC, file capabilities, or a local listener on a
suspicious port.

## Build From Project Root

```sh
cmake -B build -DVIGIL_BUILD_EXAMPLES=ON
cmake --build build --parallel
```

## Build Examples Only

```sh
cmake -S examples -B build/examples
cmake --build build/examples --parallel
```

Target names are generated from PascalCase file names:

```text
vigil_example_SuspiciousCmdLine
vigil_example_LdPreloadHijackRule
vigil_example_SystemProcessImpersonation
```

On Visual Studio generators, executables are placed under a configuration
directory such as `build/examples/Debug`.

## Linux Scenarios

The Linux examples live in `examples/linux` and model these telemetry shapes:

- `ServerSpawnedShell.cpp`: a process named `nginx` forks `/bin/sh` and runs harmless discovery commands.
- `SuspiciousPath.cpp`: a staged helper runs from `/tmp/.vigil-cache`.
- `ElevatedSuspiciousPath.cpp`: a root helper is launched from `/tmp/.vigil-cache` through sudo.
- `FilelessExecution.cpp`: the program copies itself into a memfd and executes it with `fexecve`.
- `AnonRwxMapping.cpp`: anonymous memory is mapped with read, write, and execute permissions.
- `PrivilegeEscalation.cpp`: a sudo child crosses an elevation boundary.
- `SuspiciousCmdLine.cpp`: the process command line contains downloader and reverse-shell indicators without executing them.
- `SuspiciousPort.cpp`: a beacon-shaped connection is attempted to `127.0.0.1:4444`.
- `DangerousCapabilities.cpp`: run after applying a file capability such as `cap_sys_ptrace+ep`.
- `ContainerEscapeIndicator.cpp`: ptrace-shaped activity that is relevant inside containers.
- `LdPreloadHijackRule.cpp`: relaunches with a benign `LD_PRELOAD` shared object.
- `BinaryReplacedRule.cpp`: replaces the running image on disk like a self-updater.
- `KernelModuleLoadRule.cpp`: guarded `modprobe loop` invocation.
- `PtraceInjectRule.cpp`: attaches to a child with `PTRACE_ATTACH` without modifying memory.

Example runs:

```sh
./build/examples/vigil_example_SuspiciousCmdLine
./build/examples/vigil_example_FilelessExecution
sudo ./build/examples/vigil_example_ElevatedSuspiciousPath
sudo VIGIL_ALLOW_DANGEROUS=1 ./build/examples/vigil_example_KernelModuleLoadRule
```

For `DangerousCapabilities.cpp`:

```sh
sudo setcap cap_sys_ptrace+ep ./build/examples/vigil_example_DangerousCapabilities
./build/examples/vigil_example_DangerousCapabilities
sudo setcap -r ./build/examples/vigil_example_DangerousCapabilities
```

## Windows Scenarios

The Windows examples live in `examples/windows` and model these telemetry shapes:

- `ServerSpawnedShell.cpp`: a `w3wp.exe`-named process launches `cmd.exe`.
- `SuspiciousPath.cpp`: a staged helper runs from `C:/Users/Public/vigil-examples`.
- `ElevatedSuspiciousPath.cpp`: a UAC-elevated helper runs from `C:/Users/Public/vigil-examples`.
- `FilelessExecution.cpp`: PowerShell runs an encoded in-memory marker command.
- `AnonRwxMapping.cpp`: `VirtualAlloc` reserves `PAGE_EXECUTE_READWRITE` memory.
- `PrivilegeEscalation.cpp`: a `runas` elevation boundary is crossed.
- `SuspiciousCmdLine.cpp`: PowerShell uses `-EncodedCommand`.
- `SuspiciousPort.cpp`: a beacon-shaped connection is attempted to `127.0.0.1:4444`.
- `DebugPrivilegeRule.cpp`: the process attempts to enable `SeDebugPrivilege`.
- `SystemProcessImpersonation.cpp`: a copy named `lsass.exe` runs outside System32.
- `LolbinExecution.cpp`: `regsvr32` is launched with remote-scriptlet-shaped arguments against TEST-NET.
- `SuspiciousPowerShell.cpp`: hidden encoded PowerShell execution shape.

Example runs:

```powershell
.\build\examples\Debug\vigil_example_SuspiciousPowerShell.exe
.\build\examples\Debug\vigil_example_LolbinExecution.exe
.\build\examples\Debug\vigil_example_SystemProcessImpersonation.exe
.\build\examples\Debug\vigil_example_DebugPrivilegeRule.exe
```

## Cleanup

The examples intentionally use predictable lab artifact paths:

```sh
rm -rf /tmp/.vigil-cache /tmp/vigil-preload-probe.log
```

```powershell
Remove-Item -Recurse -Force C:\Users\Public\vigil-examples -ErrorAction SilentlyContinue
```
