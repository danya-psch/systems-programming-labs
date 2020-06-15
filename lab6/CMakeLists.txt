cmake_minimum_required(VERSION 3.8)

project(lab6)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -ggdb -O0")

include_directories("include")

file(GLOB SOURCES "src/*.c")

add_executable(${PROJECT_NAME} main.c ${SOURCES})

target_link_libraries(${PROJECT_NAME} pthread)