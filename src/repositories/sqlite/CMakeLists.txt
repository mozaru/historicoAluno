# Biblioteca estatica do sqlite3
# Assumindo que você colocou sqlite3.c e sqlite3.h em external/sqlite3
add_library(sqlite3 STATIC
    ${CMAKE_SOURCE_DIR}/external/sqlite3/sqlite3.c
)

target_include_directories(sqlite3
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/sqlite3
)

# Repositório SQLite
add_library(repo_sqlite
    SQLiteDisciplinaRepository.cpp
)

target_include_directories(repo_sqlite
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/sqlite3
)

target_link_libraries(repo_sqlite
    PUBLIC
        sqlite3
)
