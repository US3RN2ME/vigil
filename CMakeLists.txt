cmake_minimum_required(VERSION 3.20)

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
    set(VIGIL_LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(VIGIL_WINDOWS TRUE)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(common)

if(VIGIL_LINUX)
    add_subdirectory(linux)
elseif(VIGIL_WINDOWS)
    add_subdirectory(windows)
endif()

add_subdirectory(app)
