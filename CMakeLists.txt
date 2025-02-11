cmake_minimum_required(VERSION 3.10)
project(FuKBoT)

# Set the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Enable warnings
add_compile_options(-Wall -Wextra)

# Find cJSON headers and library
find_path(CJSON_INCLUDE_DIR cjson/cJSON.h PATHS /usr/include /usr/local/include)
find_library(CJSON_LIB NAMES cjson libcjson PATHS /lib/x86_64-linux-gnu /usr/lib /usr/local/lib)

if(NOT CJSON_LIB OR NOT CJSON_INCLUDE_DIR)
    message(FATAL_ERROR "cJSON library not found. Please install libcjson-dev.")
else()
    message(STATUS "Found cJSON: ${CJSON_LIB} at ${CJSON_INCLUDE_DIR}")
endif()

# Ensure we only add the executable once
if(NOT TARGET FuKBoT)
    if(WIN32)
        add_executable(FuKBoT fuk3.cpp)
    else()
        add_executable(FuKBoT fuk3.c)
    endif()
endif()

# Include cJSON headers properly
target_include_directories(FuKBoT PRIVATE ${CJSON_INCLUDE_DIR})
target_link_libraries(FuKBoT PRIVATE ${CJSON_LIB})

# Link pthread (only for Linux)
if(NOT WIN32)
    target_link_libraries(FuKBoT PRIVATE pthread)
endif()
