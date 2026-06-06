cmake_minimum_required(VERSION 3.28)

project(
  vigilExamples
  DESCRIPTION "Benign attack simulation examples for vigil"
  LANGUAGES CXX)

if(NOT DEFINED VIGIL_PLATFORM_DIR)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(VIGIL_PLATFORM_DIR "linux")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(VIGIL_PLATFORM_DIR "windows")
  else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
  endif()
endif()

set(EXAMPLE_PLATFORM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${VIGIL_PLATFORM_DIR}")

file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "${EXAMPLE_PLATFORM_DIR}/*.cpp")

if(VIGIL_PLATFORM_DIR STREQUAL "linux")
  list(REMOVE_ITEM EXAMPLE_SOURCES "${EXAMPLE_PLATFORM_DIR}/PreloadProbe.cpp")
endif()

foreach(EXAMPLE_SOURCE IN LISTS EXAMPLE_SOURCES)
  get_filename_component(EXAMPLE_NAME "${EXAMPLE_SOURCE}" NAME_WE)
  set(TARGET "vigil_example_${EXAMPLE_NAME}")

  add_executable(${TARGET} ${EXAMPLE_SOURCE})
  target_include_directories(${TARGET} PRIVATE "${EXAMPLE_PLATFORM_DIR}")

  if(TARGET vigil_options)
    target_link_libraries(${TARGET} PRIVATE vigil_options)
  else()
    target_compile_features(${TARGET} PRIVATE cxx_std_23)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
  endif()

  if(VIGIL_PLATFORM_DIR STREQUAL "windows")
    target_link_libraries(${TARGET} PRIVATE shell32 ws2_32)
  endif()
endforeach()

if(VIGIL_PLATFORM_DIR STREQUAL "linux")
  add_library(vigil_example_PreloadProbe SHARED
              "${EXAMPLE_PLATFORM_DIR}/PreloadProbe.cpp")
  set_target_properties(vigil_example_PreloadProbe
                        PROPERTIES OUTPUT_NAME "vigil_preload_probe")

  if(TARGET vigil_options)
    target_link_libraries(vigil_example_PreloadProbe PRIVATE vigil_options)
  else()
    target_compile_features(vigil_example_PreloadProbe PRIVATE cxx_std_23)
    set_target_properties(vigil_example_PreloadProbe PROPERTIES CXX_EXTENSIONS
                                                                OFF)
  endif()
endif()
