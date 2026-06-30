cmake_minimum_required(VERSION 3.28)

option(VIGIL_ENABLE_INSTALL "Create installable target" ON)
option(VIGIL_BUILD_TESTS "Build tests" ON)
option(VIGIL_BUILD_APP "Build the vigil application" ON)
option(VIGIL_BUILD_DOCS "Build API documentation with Doxygen" ON)
option(VIGIL_BUILD_EXAMPLES "Build attack simulation examples" OFF)

if(VIGIL_BUILD_TESTS)
   list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(
   vigil
   VERSION 0.2.0
   DESCRIPTION "Cross-platform EDR agent"
   LANGUAGES CXX)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(VIGIL_PLATFORM_DIR "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   set(VIGIL_PLATFORM_DIR "windows")
else()
   message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

include(GNUInstallDirs)

configure_file("${CMAKE_SOURCE_DIR}/cmake/Version.hpp.in" "${PROJECT_BINARY_DIR}/include/vigil/Version.hpp" @ONLY)

add_library(vigil_options INTERFACE)
target_compile_features(vigil_options INTERFACE cxx_std_23)
set_target_properties(vigil_options PROPERTIES INTERFACE_CXX_EXTENSIONS OFF)
target_compile_options(vigil_options INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /EHsc>
                                               $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

add_subdirectory(src)

if(VIGIL_BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)
endif()

if(VIGIL_BUILD_APP)
   add_subdirectory(app)
endif()

if(VIGIL_BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

if(VIGIL_BUILD_DOCS)
   find_package(Doxygen QUIET)
   if(DOXYGEN_FOUND)
      set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/docs")
      set(DOXYGEN_OUTPUT_DIR "${PROJECT_BINARY_DIR}/docs")
      configure_file("${CMAKE_SOURCE_DIR}/cmake/Doxyfile.in" "${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)

      add_custom_target(
         docs
         COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_BINARY_DIR}/Doxyfile"
         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
         COMMENT "Generating API documentation with Doxygen"
         VERBATIM)
   else()
      message(STATUS "Doxygen not found; docs target disabled")
   endif()
endif()

if(VIGIL_ENABLE_INSTALL)
   include(CMakePackageConfigHelpers)

   install(TARGETS vigil_options EXPORT VigilTargets)

   install(
      DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      FILES_MATCHING
      PATTERN "*.hpp")

   install(FILES "${PROJECT_BINARY_DIR}/include/vigil/Version.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vigil)

   configure_package_config_file("${CMAKE_SOURCE_DIR}/cmake/VigilConfig.cmake.in" "${PROJECT_BINARY_DIR}/VigilConfig.cmake"
                                 INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vigil)

   write_basic_package_version_file(
      "${PROJECT_BINARY_DIR}/VigilConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY SameMajorVersion)

   install(FILES "${PROJECT_BINARY_DIR}/VigilConfig.cmake" "${PROJECT_BINARY_DIR}/VigilConfigVersion.cmake"
           DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vigil)

   install(
      EXPORT VigilTargets
      FILE VigilTargets.cmake
      NAMESPACE vigil::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vigil)
endif()
