file(GLOB_RECURSE WINDOWS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE WINDOWS_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(vigil_platform ${WINDOWS_SOURCES} ${WINDOWS_HEADERS})

target_include_directories(vigil_platform PUBLIC ".")

target_link_libraries(vigil_platform PUBLIC vigil_core ntdll)
target_compile_definitions(vigil_platform PUBLIC VIGIL_PLATFORM_WINDOWS WIN32_LEAN_AND_MEAN)
