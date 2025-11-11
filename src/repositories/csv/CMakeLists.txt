add_library(repo_csv
    CsvDisciplinaRepository.cpp
)

target_include_directories(repo_csv
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
