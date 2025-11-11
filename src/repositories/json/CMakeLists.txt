add_library(repo_json
    JsonDisciplinaRepository.cpp
)

target_include_directories(repo_json
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/json
)
