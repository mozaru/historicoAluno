cmake_minimum_required(VERSION 3.16)
project(historico_tui CXX)

# C++ padrão
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Coleta recursiva dos fontes em src/ + principal.cpp
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
set(SRCS
    ${SRC_FILES}
    ${CMAKE_SOURCE_DIR}/principal.cpp
)

# Dependência FTXUI via FetchContent (oficialmente recomendado)
#include(FetchContent)
#FetchContent_Declare(
#  ftxui
#  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
#  GIT_TAG v6.1.9 # ou a versão estável atual
#)
#FetchContent_MakeAvailable(ftxui) # cria targets ftxui::screen ftxui::dom ftxui::component
# Fonte: README oficial do FTXUI. 

# Executável
add_executable(historico ${SRCS})
if (MINGW)
    # Otimizar pra tamanho e permitir coleta de código morto
    target_compile_options(historico PRIVATE
        -Os
        -ffunction-sections
        -fdata-sections
    )

    # Link estático das runtimes + remover código não usado + strip
    target_link_options(historico PRIVATE
        -static-libgcc
        -static-libstdc++
        -Wl,--gc-sections
        -Wl,-s
    )
endif()


# Diretórios de include
# Coleta todos os headers em headers/ recursivamente
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/headers/*.hpp
    ${CMAKE_SOURCE_DIR}/headers/*.h
)

# Extrai os diretórios únicos desses headers
set(HEADER_DIRS "")
foreach(h ${HDRS})
    get_filename_component(dir ${h} DIRECTORY)
    list(APPEND HEADER_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

# Adiciona todos como include dirs
target_include_directories(historico PRIVATE ${HEADER_DIRS})

# Warnings
if (MSVC)
    target_compile_options(historico PRIVATE /W4)
else()
    target_compile_options(historico PRIVATE -Wall -Wextra)
endif()

if (MINGW)
    target_link_options(historico PRIVATE -static -static-libgcc -static-libstdc++)
endif()


# Link com FTXUI
#target_link_libraries(historico PRIVATE
#    ftxui::screen
#    ftxui::dom
#    ftxui::component
#)

# Em Windows (MinGW/console moderno), garantir uso de ANSI:
if (WIN32)
    # se quiser, você pode habilitar VT no runtime; isso aqui é só uma dica
    add_definitions(-DFTXUI_MICROSOFT_CONSOLE)
endif()

