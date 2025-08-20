cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable for your main program
file(GLOB SOURCES "src/*.cpp")

add_executable(main ${SOURCES})

# Include headers
target_include_directories(main PUBLIC includes)