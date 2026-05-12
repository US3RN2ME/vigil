cmake_minimum_required(VERSION 3.28)

option(VIGIL_ENABLE_INSTALL "Create installable target" ON) # TODO
option(VIGIL_BUILD_TESTS "Build tests" ON)

if(VIGIL_BUILD_TESTS)
   list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(
   vigil
   VERSION 0.1.0
   DESCRIPTION "Cross-platform EDR agent"
   LANGUAGES CXX)

configure_file("${CMAKE_SOURCE_DIR}/cmake/version.hpp.in" "${PROJECT_BINARY_DIR}/include/vigil/version.hpp" @ONLY)

add_library(vigil_options INTERFACE)
target_compile_features(vigil_options INTERFACE cxx_std_23)
set_target_properties(vigil_options PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)
target_compile_options(vigil_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /EHsc>
                                               $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(VIGIL_PLATFORM_DIR "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   set(VIGIL_PLATFORM_DIR "windows")
else()
   message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(src)

if(VIGIL_BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)
endif()

add_subdirectory(app)
