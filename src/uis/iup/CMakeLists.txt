# UI IUP para o projeto Historico Academico

cmake_minimum_required(VERSION 3.16)

set(UI_IUP_SOURCES
    UIIup.cpp
    IupHomeWindow.cpp
    IupHelpers.cpp
    IupDisciplinaFormDialog.cpp
    IupConfirmDialog.cpp
    IupMessageDialog.cpp
)

add_library(ui_iup ${UI_IUP_SOURCES})

# Includes do projeto (interfaces IHistoricoService, ILogger, etc.)
target_include_directories(ui_iup
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# =========================================
# IUP em external/iup
# =========================================
#
# ASSUMINDO a seguinte estrutura:
#   external/iup/
#       include/   -> iup.h, iupcontrols.h, etc.
#       lib/       -> libs compiladas (libiup.a, libiupcontrols.a, ...)
#
# Ajuste os nomes dos arquivos de lib conforme seu ambiente.
# No Windows com MinGW, por exemplo, podem ser:
#   libiup.a, libiupcontrols.a, libiupimglib.a, etc.

set(IUP_ROOT_DIR "${CMAKE_SOURCE_DIR}/external/iup")
set(IUP_INCLUDE_DIR "${IUP_ROOT_DIR}/include")
set(IUP_LIB_DIR     "${IUP_ROOT_DIR}/lib")

if (NOT EXISTS "${IUP_INCLUDE_DIR}/iup.h")
    message(FATAL_ERROR
        "Nao encontrei iup.h em ${IUP_INCLUDE_DIR}\n"
        "Verifique se a IUP foi copiada para external/iup/include"
    )
endif()

# Aqui você DEVE ajustar os nomes das libs conforme o que existir em external/iup/lib
# Exemplos típicos (ajuste se for diferente no seu ambiente):
set(IUP_LIBRARIES
    "${IUP_LIB_DIR}/libiup.a"
    "${IUP_LIB_DIR}/libiupcontrols.a"
    "${IUP_LIB_DIR}/libiupcd.a"
    "${IUP_LIB_DIR}/libcd.a" 
    "${IUP_LIB_DIR}/libfreetype6.a"
    "${IUP_LIB_DIR}/libz.a"
    "${IUP_LIB_DIR}/libiupimglib.a"
)

foreach(libfile IN LISTS IUP_LIBRARIES)
    if (NOT EXISTS "${libfile}")
        message(WARNING "Biblioteca IUP nao encontrada (ajuste o nome): ${libfile}")
    endif()
endforeach()

if (WIN32)
    list(APPEND IUP_LIBRARIES
        comctl32   # InitCommonControlsEx, ImageList_*, MakeDragList, LBItemFromPt
        comdlg32
        gdi32
        user32
        ole32
        oleaut32
        uuid
        advapi32
    )
endif()

target_include_directories(ui_iup
    PUBLIC
        ${IUP_INCLUDE_DIR}
)

target_link_libraries(ui_iup
    PUBLIC
        ${IUP_LIBRARIES}
)

# Warnings consistentes com o restante do projeto
if (MSVC)
    target_compile_options(ui_iup PRIVATE /W4)
else()
    target_compile_options(ui_iup PRIVATE -Wall -Wextra)
endif()
