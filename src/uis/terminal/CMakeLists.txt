add_library(ui_terminal
    UITerminal.cpp
    terminal.cpp
    UITerminalForm.cpp
    UITerminalList.cpp
    UITerminalMenu.cpp
    UITerminalMessage.cpp
)

target_include_directories(ui_terminal
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
