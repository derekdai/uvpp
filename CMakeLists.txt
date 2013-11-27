cmake_minimum_required(VERSION 2.8)

project(test-uv)

add_definitions(-Wall)

include_directories(src)

add_library(uv++ SHARED
            src/uv++-loop
            src/uv++-handle
            src/uv++-timer
            src/uv++-signal
            src/uv++-address
            src/uv++-buffer
            src/uv++-socket
            src/uv++-stream
            src/uv++-tcp
            src/uv++-udp)
target_link_libraries(uv++ uv)

add_executable(test examples/test)
target_link_libraries(test uv++)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(device
               device
               daemon)
target_link_libraries(device uv++)

add_executable(registry
               registry
               daemon)
target_link_libraries(registry uv++)
