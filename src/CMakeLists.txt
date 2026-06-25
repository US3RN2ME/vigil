file(GLOB CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB CORE_RULES_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/rules/*.cpp")

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(vigil_core)

target_sources(vigil_core PRIVATE ${CORE_SOURCES} ${CORE_RULES_SOURCES})

target_link_libraries(
  vigil_core PRIVATE vigil_options nlohmann_json::nlohmann_json spdlog::spdlog)

target_include_directories(
  vigil_core
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_subdirectory(platform/${VIGIL_PLATFORM_DIR})

if(VIGIL_ENABLE_INSTALL)
  install(
    TARGETS vigil_core
    EXPORT vigilTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
