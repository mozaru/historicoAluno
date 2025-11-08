cmake_minimum_required(VERSION 3.16)
project(historico_tui CXX)

# C++ padrão
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coleta recursiva dos fontes em src/
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(historico ${SRC_FILES})

# Diretórios de include:
# Todos os includes são feitos sem diretório (ex: #include "Disciplina.hpp"),
# então precisamos adicionar TODOS os diretórios e subdiretórios de include/
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)

set(HEADER_DIRS "")
foreach(h ${HDRS})
    get_filename_component(dir "${h}" DIRECTORY)
    list(APPEND HEADER_DIRS "${dir}")
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

target_include_directories(historico PRIVATE
    ${HEADER_DIRS}
)

# Warnings
if (MSVC)
    target_compile_options(historico PRIVATE /W4)
else()
    target_compile_options(historico PRIVATE -Wall -Wextra)
endif()

# Otimizações e linkagem estática (focado em MinGW; ajusta conforme necessidade)
if (MINGW)
    target_compile_options(historico PRIVATE
        -Os
        -ffunction-sections
        -fdata-sections
    )

    target_link_options(historico PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
        -Wl,--gc-sections
        -Wl,-s
    )
endif()

# FTXUI (quando for usar, descomentar e ajustar)
# include(FetchContent)
# FetchContent_Declare(
#   ftxui
#   GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
#   GIT_TAG v6.1.9
# )
# FetchContent_MakeAvailable(ftxui)
# target_link_libraries(historico PRIVATE
#   ftxui::screen
#   ftxui::dom
#   ftxui::component
# )

# Windows: definir macro para consoles com suporte VT (se usar FTXUI/cores ANSI)
if (WIN32)
    add_definitions(-DFTXUI_MICROSOFT_CONSOLE)
endif()
