cmake_minimum_required(VERSION 3.10)
project(FuKBoT)

# Enable warnings
add_compile_options(-Wall -Wextra)

# Find cJSON library
find_library(CJSON_LIB NAMES cjson libcjson PATHS /usr/lib /usr/local/lib /lib/x86_64-linux-gnu)
find_path(CJSON_INCLUDE_DIR cjson/cJSON.h PATHS /usr/include /usr/local/include)

if(CJSON_LIB AND CJSON_INCLUDE_DIR)
    message(STATUS "Found cJSON: ${CJSON_LIB}")
    target_include_directories(FuKBoT PRIVATE ${CJSON_INCLUDE_DIR})
    target_link_libraries(FuKBoT PRIVATE ${CJSON_LIB})
else()
    message(FATAL_ERROR "cJSON library not found. Please install libcjson-dev.")
endif()

# Specify output directories
set_target_properties(FuKBoT PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Add source file and executable
add_executable(FuKBoT fuk3.c)

# Link pthread (only for Linux)
if(NOT WIN32)
    target_link_libraries(FuKBoT PRIVATE pthread)
endif()
