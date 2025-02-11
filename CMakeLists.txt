cmake_minimum_required(VERSION 3.10)
project(FuKBoT)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
# Add the executable
add_executable(FuKBoT fuk3.cpp)
else()
add_executable(FuKBoT fuk3.c)
endif()


# Specify output directories
set_target_properties(FuKBoT PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(FuKBoT PRIVATE ws2_32 Threads::Threads cJSON::cJSON)
else()
    target_link_libraries(FuKBoT PRIVATE pthread cJSON)
endif()
