# Lib estatica pugixml
add_library(pugixml STATIC
    ${CMAKE_SOURCE_DIR}/external/pugixml/pugixml.cpp
)

target_include_directories(pugixml
    PUBLIC
        ${CMAKE_SOURCE_DIR}/external/pugixml
)

# Repo XML
add_library(repo_xml
    XmlDisciplinaRepository.cpp
)

target_include_directories(repo_xml
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/pugixml
)

target_link_libraries(repo_xml
    PUBLIC
        pugixml
)
