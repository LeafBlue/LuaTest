cmake_minimum_required(VERSION 3.19)
project(LuaTest LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(LuaTest
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lualoader/include)

target_link_libraries(LuaTest
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/lualoader/lib/liblua.a
)


include(GNUInstallDirs)

install(TARGETS LuaTest
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/luafile")
set(DEST_DIR   "${CMAKE_BINARY_DIR}/luafile")

add_custom_command(
    OUTPUT "${DEST_DIR}/.copied"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_DIR}" "${DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${DEST_DIR}/.copied"
    COMMENT "Copying luafile directory to build folder"
    VERBATIM
)

add_custom_target(copy_assets ALL
    DEPENDS "${DEST_DIR}/.copied"
)


qt_generate_deploy_app_script(
    TARGET LuaTest
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
