file(GLOB CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB CORE_RULES_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/rules/*.cpp")

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(vigil_core ${CORE_SOURCES} ${CORE_RULES_SOURCES})

target_link_libraries(vigil_core PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)

target_include_directories(vigil_core PUBLIC "${CMAKE_SOURCE_DIR}/include")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
   add_subdirectory(platform/linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
   add_subdirectory(platform/windows)
else()
   message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
