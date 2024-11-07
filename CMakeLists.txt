cmake_minimum_required(VERSION 3.16)
project(SystemProxyConfig VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(examples)

include(GNUInstallDirs)
install(TARGETS SystemProxyConfig DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})