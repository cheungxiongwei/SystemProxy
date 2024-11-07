cmake_minimum_required(VERSION 3.16)

add_executable(app "main.cpp")

target_link_libraries(app PRIVATE SystemProxyConfig)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)