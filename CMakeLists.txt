cmake_minimum_required(VERSION 3.20)
project(Chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all"
        CACHE STRING "Compiler flags in asan build"
        FORCE
        )

find_package(JSON 3.2.0 REQUIRED)

include_directories(other_soft/lib_basics)

add_subdirectory(server)
link_directories(server)

add_subdirectory(client)
link_directories(client)
