
#set(HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../includes/*.h)

add_executable(
        genes_project
        ${CMAKE_CURRENT_SOURCE_DIR}/app.cpp
#        ${HEADER_FILES}
)

target_link_libraries(genes_project ZLIB::ZLIB LibrariesModule)