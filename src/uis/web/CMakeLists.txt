cmake_minimum_required(VERSION 3.15)

add_library(ui_web STATIC
    UIWeb.cpp
)

target_include_directories(ui_web PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/json
)

if (WIN32)
    target_link_libraries(ui_web PUBLIC ws2_32)
endif()
