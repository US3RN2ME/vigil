file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(vigil_common ${COMMON_SOURCES} ${COMMON_HEADERS})

target_link_libraries(vigil_common PRIVATE nlohmann_json::nlohmann_json spdlog::spdlog)

target_include_directories(vigil_common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
