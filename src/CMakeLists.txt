file(GLOB CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB CORE_RULES_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/rules/*.cpp")

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(vigil_core ${CORE_SOURCES} ${CORE_RULES_SOURCES})

target_link_libraries(vigil_core PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)

# TODO: Replace with: $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
target_include_directories(vigil_core PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_subdirectory(platform/${VIGIL_PLATFORM_DIR})

