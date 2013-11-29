cmake_minimum_required(VERSION 2.8)

project(test-uv)

add_definitions(-Wall)

add_library(uv++ SHARED
            src/uv++-loop
            src/uv++-handle
            src/uv++-timer
            src/uv++-signal
            src/uv++-address
            src/v++-buffer
            src/v++-socket
            src/v++-stream
            src/v++-tcp
            src/v++-udp
)
target_link_libraries(uv++ uv)

add_executable(test src/test)
target_link_libraries(test uv++)
