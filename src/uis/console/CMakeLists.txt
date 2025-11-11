add_library(ui_console
    UIConsole.cpp
)

target_include_directories(ui_console
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
