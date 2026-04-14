file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(vigil ${APP_SOURCES})

target_link_libraries(vigil PRIVATE vigil_platform vigil_common)
