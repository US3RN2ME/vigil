find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBBPF REQUIRED libbpf)

find_library(LIBELF_LIBRARY REQUIRED NAMES elf)
find_library(LIBZ_LIBRARY REQUIRED NAMES z)

find_program(CLANG clang REQUIRED)
find_program(BPFTOOL bpftool REQUIRED)

set(VMLINUX_H "${CMAKE_CURRENT_SOURCE_DIR}/bpf/vmlinux.h")

add_custom_command(
   OUTPUT ${VMLINUX_H}
   COMMAND /bin/sh -c "${BPFTOOL} btf dump file /sys/kernel/btf/vmlinux format c > ${VMLINUX_H}"
   COMMENT "Generating vmlinux.h from running kernel BTF"
   VERBATIM)

add_custom_target(vmlinux_h DEPENDS ${VMLINUX_H})

file(GLOB_RECURSE LINUX_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE LINUX_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(vigil_platform ${LINUX_SOURCES} ${LINUX_HEADERS})

add_dependencies(vigil_platform vmlinux_h)

target_include_directories(vigil_platform PRIVATE ${LIBBPF_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vigil_platform PRIVATE vigil_core ${LIBBPF_LIBRARIES} ${LIBELF_LIBRARY} ${LIBZ_LIBRARY})

target_compile_definitions(vigil_platform PRIVATE VIGIL_PLATFORM_LINUX)

file(GLOB BPF_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/bpf/*.bpf.c")

foreach(BPF_SOURCE IN LISTS BPF_SOURCES)
   get_filename_component(BPF_NAME "${BPF_SOURCE}" NAME_WE)
   set(BPF_OUTPUT "${CMAKE_BINARY_DIR}/bpf/${BPF_NAME}.bpf.o")

   add_custom_command(
      OUTPUT ${BPF_OUTPUT}
      COMMAND ${CLANG} -target bpf -O2 -g -I/usr/include/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu -c ${BPF_SOURCE} -o ${BPF_OUTPUT}
      DEPENDS ${BPF_SOURCE}
      COMMENT "Compiling eBPF: ${BPF_NAME}.bpf.c")

   list(APPEND BPF_OBJECTS ${BPF_OUTPUT})

   string(TOUPPER "${BPF_NAME}" BPF_NAME_UPPER)

   target_compile_definitions(vigil_platform PRIVATE VIGIL_${BPF_NAME_UPPER}_BPF_OBJECT="${BPF_OUTPUT}")
endforeach()

if(BPF_OBJECTS)
   add_custom_target(bpf_programs ALL DEPENDS ${BPF_OBJECTS})
   add_dependencies(bpf_programs vmlinux_h)
   add_dependencies(vigil_platform bpf_programs)
endif()
