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

add_subdirectory(src)

add_subdirectory(app)
