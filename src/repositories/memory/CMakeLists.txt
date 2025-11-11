add_library(repo_memory
    MemoryDisciplinaRepository.cpp
)

target_include_directories(repo_memory
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
