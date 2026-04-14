file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(vigil_common INTERFACE ${COMMON_HEADERS})

target_include_directories(vigil_common INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/..")
