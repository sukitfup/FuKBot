cmake_minimum_required(VERSION 3.10)
project(FuKBot)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(FuKBot src/fuk3.cpp src/other_source_files.cpp)

# Specify output directories
set_target_properties(FuKBot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Include directories
include_directories(include)

# Link libraries
target_link_libraries(FuKBot pthread)
