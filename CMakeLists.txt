cmake_minimum_required(VERSION 3.10)

project(CommandLineStuff)

set(SOURCES
    # Main
    src/main.cpp
)

add_subdirectory(libs/glm)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src libs)
