file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(VIGIL_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/config.json")

configure_file("${CMAKE_SOURCE_DIR}/config/config.json" ${VIGIL_CONFIG_PATH} COPYONLY)

add_executable(vigil ${APP_SOURCES})

target_link_libraries(vigil PRIVATE vigil_core vigil_options)

# TODO: Use proper cmake path for install
target_compile_definitions(vigil PRIVATE VIGIL_CONFIG_PATH="${VIGIL_CONFIG_PATH}")

if(VIGIL_ENABLE_INSTALL)
   include(GNUInstallDirs)
   install(TARGETS vigil RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
   install(FILES "${CMAKE_SOURCE_DIR}/config/config.json" DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/vigil")
endif()
