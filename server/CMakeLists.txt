cmake_minimum_required(VERSION 3.20)

include_directories(handlers)
include_directories(structs)

add_library(server_handlers STATIC handlers/replies.cpp)
add_library(server STATIC structs/Server.cpp)
add_library(server_storages STATIC structs/storages.cpp)
target_include_directories(server_handlers PUBLIC handlers)
target_include_directories(server PUBLIC structs)

target_link_libraries(server_handlers contrib_json)
target_link_libraries(server server_handlers)
target_link_libraries(server server_storages)

add_executable(server_starter server_starter.cpp)

target_link_libraries(server_starter server)
target_compile_options(server_starter PRIVATE -std=c++17)
target_compile_definitions(server_starter PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)
target_link_libraries(server_starter pthread)
