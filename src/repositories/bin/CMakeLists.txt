add_library(repo_bin
    BinaryDisciplinaRepository.cpp
)

target_include_directories(repo_bin
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
