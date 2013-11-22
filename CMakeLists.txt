cmake_minimum_required(VERSION 2.8)

project(test-uv)

add_definitions(-Wall)

add_library(uv++ SHARED
            uv++-loop
            uv++-handle
            uv++-timer
)
target_link_libraries(uv++ uv)

add_executable(test test)
target_link_libraries(test uv++ uv)
