add_library(ui_cpp_terminal
    UICppTerminal.cpp
    CppTermHomeScreen.cpp
    CppTermDisciplinaForm.cpp
    CppTermConfirmDialog.cpp
    CppTermMessageDialog.cpp
    CppTermWindowUtils.cpp
)

target_include_directories(ui_cpp_terminal
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
