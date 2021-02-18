# Project name
project(Trains)

# Minimum required cmake version
cmake_minimum_required(VERSION 3.2)

# Compile with c++ 11 support
set(CMAKE_CXX_STANDARD 11)

# Static inclusion of dependencies required for the executables to be independent.
# May be excluded in case of problems with Unix systems.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

# Add source directory
aux_source_directory(src/ SOURCES)

# Create executable for the run configuration
add_executable(${PROJECT_NAME} ${SOURCES})

# target directory to the configuration
target_include_directories(${PROJECT_NAME} PRIVATE include/ _libs/)
