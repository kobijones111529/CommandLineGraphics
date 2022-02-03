cmake_minimum_required(VERSION 3.10)

project(CommandLineStuff)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCES
    # Main
    src/main.cpp
)

add_subdirectory(libs/glm)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src libs)
