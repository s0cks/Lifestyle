cmake_minimum_required(VERSION 3.2)
project(Lifestyle)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

set(SOURCE_FILES main.c)
add_executable(Lifestyle ${SOURCE_FILES})
target_link_libraries(Lifestyle hoedown)