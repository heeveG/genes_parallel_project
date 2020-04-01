#Search for csv file
file(GLOB CSV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data/*.csv)

# configure the header
set(DIR_STRING ${CSV_DIR})
configure_file(
        ${CMAKE_SOURCE_DIR}/includes/csv_file.h.in
        ${CMAKE_SOURCE_DIR}/includes/csv_file.h
        @ONLY
)

# Includes the new header
include_directories(${CMAKE_CURRENT_BINARY_DIR})