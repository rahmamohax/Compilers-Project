cmake_minimum_required(VERSION 3.10)

# Set project name and language
project(CompilerProject CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/SOURCE/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SOURCE/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SOURCE/test_scanner.cpp
)

# Add header files
set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/HEADERS/scanner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HEADERS/parser.h
)

# Add the executable
add_executable(compiler_test ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(compiler_test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/SOURCE
)

# Enable warnings
if(MSVC)
    target_compile_options(compiler_test PRIVATE /W4)
else()
    target_compile_options(compiler_test PRIVATE -Wall -Wextra)
endif() 