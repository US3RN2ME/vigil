cmake_minimum_required(VERSION 3.20)

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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)

if(MSVC)
   add_compile_options(/W4 /permissive- /EHsc)
else()
   add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(VIGIL_PLATFORM_DIR "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   set(VIGIL_PLATFORM_DIR "windows")
else()
   message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(src)

if(VIGIL_BUILD_TESTS)
   add_subdirectory(test)
endif()

add_subdirectory(app)
