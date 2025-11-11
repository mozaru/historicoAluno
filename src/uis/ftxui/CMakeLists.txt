add_library(ui_ftxui
    UIFtxui.cpp
    FtxuiAppState.cpp
    FtxuiHomeScreen.cpp
    FtxuiDisciplinaForm.cpp
    FtxuiConfirmDialog.cpp
    FtxuiMessageDialog.cpp
    FtxuiTableUtils.cpp
)

target_include_directories(ui_ftxui
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ui_ftxui
    PUBLIC
        ftxui::component
        ftxui::dom
        ftxui::screen
)
