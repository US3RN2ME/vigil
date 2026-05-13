find_package(PkgConfig REQUIRED)
pkg_check_modules(libbpf REQUIRED IMPORTED_TARGET libbpf)
pkg_check_modules(libelf REQUIRED IMPORTED_TARGET libelf)
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)

find_program(CLANG clang REQUIRED)
find_program(BPFTOOL bpftool REQUIRED)

set(VMLINUX_H "${CMAKE_CURRENT_SOURCE_DIR}/bpf/vmlinux.h")

add_custom_command(
   OUTPUT ${VMLINUX_H}
   COMMAND /bin/sh -c "${BPFTOOL} btf dump file /sys/kernel/btf/vmlinux format c > ${VMLINUX_H}"
   COMMENT "Generating vmlinux.h from running kernel BTF"
   VERBATIM)

add_custom_target(vigil_vmlinux_h DEPENDS ${VMLINUX_H})

file(GLOB_RECURSE LINUX_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE LINUX_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

target_sources(vigil_core PRIVATE ${LINUX_SOURCES} ${LINUX_HEADERS})

add_dependencies(vigil_core vigil_vmlinux_h)

target_link_libraries(vigil_core PRIVATE PkgConfig::libbpf PkgConfig::libelf PkgConfig::zlib)

set(BPF_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/bpf/events.bpf.c")
set(BPF_OUTPUT "${CMAKE_BINARY_DIR}/bpf/events.bpf.o")
set(BPF_SKELETON "${CMAKE_BINARY_DIR}/bpf/events.skel.h")

add_custom_command(
   OUTPUT ${BPF_OUTPUT}
   COMMAND ${CLANG} -target bpf -O2 -g -fdiagnostics-color=always -I/usr/include/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu -c
           ${BPF_SOURCE} -o ${BPF_OUTPUT}
   DEPENDS ${BPF_SOURCE} ${VMLINUX_H}
   COMMENT "Compiling eBPF: events.bpf.c"
   VERBATIM USES_TERMINAL)

add_custom_command(
   OUTPUT ${BPF_SKELETON}
   COMMAND /bin/sh -c "${BPFTOOL} gen skeleton ${BPF_OUTPUT} > ${BPF_SKELETON}"
   DEPENDS ${BPF_OUTPUT}
   COMMENT "Generating BPF skeleton: events.skel.h"
   VERBATIM)

add_custom_target(vigil_bpf_skeleton ALL DEPENDS ${BPF_SKELETON})
add_dependencies(vigil_core vigil_bpf_skeleton)

target_include_directories(vigil_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_BINARY_DIR}/bpf")
target_compile_definitions(vigil_core PUBLIC VIGIL_PLATFORM_LINUX)
