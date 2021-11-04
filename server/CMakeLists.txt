cmake_minimum_required(VERSION 3.20)

include_directories(handlers)
include_directories(structs)

add_library(HANDLERS STATIC handlers/replies.cpp)
add_library(SERVER STATIC structs/server.cpp)
add_library(USER_TABLE STATIC structs/storages.cpp)
target_include_directories(HANDLERS PUBLIC handlers)
target_include_directories(SERVER PUBLIC structs)

target_link_libraries(HANDLERS contrib_json)
target_link_libraries(SERVER HANDLERS)
target_link_libraries(SERVER USER_TABLE)

add_executable(Server server_starter.cpp)

target_link_libraries(Server SERVER)
target_compile_options(Server PRIVATE -std=c++17)
target_compile_definitions(Server PRIVATE BOOST_ERROR_CODE_HEADER_ONLY)
target_link_libraries(Server pthread)
