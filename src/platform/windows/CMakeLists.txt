file(GLOB_RECURSE WINDOWS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE WINDOWS_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

target_sources(vigil_core PRIVATE ${WINDOWS_SOURCES} ${WINDOWS_HEADERS})

target_include_directories(vigil_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vigil_core PUBLIC ntdll psapi advapi32 iphlpapi ws2_32)

target_compile_definitions(vigil_core PUBLIC VIGIL_PLATFORM_WINDOWS WIN32_LEAN_AND_MEAN)
