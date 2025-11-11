add_library(repo_fixed
    FixedDisciplinaRepository.cpp
)

target_include_directories(repo_fixed
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
