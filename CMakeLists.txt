cmake_minimum_required(VERSION 3.16)
project(historico_tui CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================
# Seleção de implementação
# =========================================
if (NOT UI_IMPLEMENTATION)
    set(UI_IMPLEMENTATION "console")
endif()

if (NOT REPOSITORY_IMPLEMENTATION)
    set(REPOSITORY_IMPLEMENTATION "memory")
endif()

message(STATUS "UI_IMPLEMENTATION = ${UI_IMPLEMENTATION}")
message(STATUS "REPOSITORY_IMPLEMENTATION = ${REPOSITORY_IMPLEMENTATION}")

# =========================================
# Core: todos os .cpp de src/, exceto uis/ e repositories/
# =========================================
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/src/uis/.*")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/src/repositories/.*")

add_executable(historico
    ${CORE_SOURCES}
)

target_include_directories(historico
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# =========================================
# Repository (cada um no seu subdiretorio)
# =========================================
if (REPOSITORY_IMPLEMENTATION STREQUAL "memory")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/memory)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_MEMORY)
    target_link_libraries(historico PRIVATE repo_memory)

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "bin")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/bin)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_BINARY)
    target_link_libraries(historico PRIVATE repo_bin)

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "fixed")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/fixed)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_FIXED)
    target_link_libraries(historico PRIVATE repo_fixed)

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "csv")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/csv)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_CSV)
    target_link_libraries(historico PRIVATE repo_csv)   

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "sqlite")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/sqlite)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_SQLITE)
    target_link_libraries(historico PRIVATE repo_sqlite)

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "json")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/json)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_JSON)
    target_link_libraries(historico PRIVATE repo_json)    

elseif (REPOSITORY_IMPLEMENTATION STREQUAL "xml")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/repositories/xml)
    target_compile_definitions(historico PRIVATE REPOSITORY_IMPLEMENTATION_XML)
    target_link_libraries(historico PRIVATE repo_xml)    

else()
    message(FATAL_ERROR "REPOSITORY_IMPLEMENTATION invalido: ${REPOSITORY_IMPLEMENTATION}")
endif()

# =========================================
# UI (cada uma no seu subdiretorio)
# =========================================
if (UI_IMPLEMENTATION STREQUAL "console")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/uis/console)
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_CONSOLE)
    target_link_libraries(historico PRIVATE ui_console)

elseif (UI_IMPLEMENTATION STREQUAL "terminal")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/uis/terminal)
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_TERMINAL)
    target_link_libraries(historico PRIVATE ui_terminal)

elseif (UI_IMPLEMENTATION STREQUAL "cpp-terminal")
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/cpp-terminal)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/uis/cpp-terminal)
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_CPP_TERMINAL)
    target_link_libraries(historico PRIVATE
        ui_cpp_terminal
        cpp-terminal::cpp-terminal
    )

elseif (UI_IMPLEMENTATION STREQUAL "ftxui")
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/ftxui)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/uis/ftxui)
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_FTXUI)
    target_link_libraries(historico PRIVATE ui_ftxui)

elseif (UI_IMPLEMENTATION STREQUAL "web")
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/uis/web)
    target_compile_definitions(historico PRIVATE UI_IMPLEMENTATION_WEB)
    target_link_libraries(historico PRIVATE ui_web)
else()
    message(FATAL_ERROR "UI_IMPLEMENTATION invalido: ${UI_IMPLEMENTATION}")
endif()


# =========================================
# HEADERS: adicionar todos os diretórios onde há .hpp/.h
# para suportar includes "Errors.hpp" etc.
# =========================================
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

# Aplica aos alvos relevantes
target_include_directories(historico PRIVATE ${HEADER_DIRS})

# Se os targets existirem, herdando os mesmos includes:
if (TARGET repo_memory)
    target_include_directories(repo_memory PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_bin)
    target_include_directories(repo_bin PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_fixed)
    target_include_directories(repo_fixed PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_csv)
    target_include_directories(repo_csv PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_sqlite)
    target_include_directories(repo_sqlite PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_json)
    target_include_directories(repo_json PUBLIC ${HEADER_DIRS})
endif()

if (TARGET repo_xml)
    target_include_directories(repo_xml PUBLIC ${HEADER_DIRS})
endif()

if (TARGET ui_console)
    target_include_directories(ui_console PUBLIC ${HEADER_DIRS})
endif()

if (TARGET ui_terminal)
    target_include_directories(ui_terminal PUBLIC ${HEADER_DIRS})
endif()

if (TARGET ui_cpp_terminal)
    target_include_directories(ui_cpp_terminal PUBLIC ${HEADER_DIRS})
endif()

if (TARGET ui_ftxui)
    target_include_directories(ui_ftxui PUBLIC ${HEADER_DIRS})
endif()

if (TARGET ui_web)
    target_include_directories(ui_web PUBLIC ${HEADER_DIRS})
endif()


# =========================================
# Warnings
# =========================================
if (MSVC)
    target_compile_options(historico PRIVATE /W4)
else()
    target_compile_options(historico PRIVATE -Wall -Wextra)
endif()

# =========================================
# MinGW: otimização + link estático opcional
# =========================================
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

# =========================================
# Windows: suporte VT/ANSI
# =========================================
if (WIN32)
    target_compile_definitions(historico PRIVATE FTXUI_MICROSOFT_CONSOLE)
endif()
