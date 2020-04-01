
add_executable(
        genes_project
        ${CMAKE_CURRENT_SOURCE_DIR}/app.cpp
)

target_link_libraries(genes_project PRIVATE LibrariesModule)