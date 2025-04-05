cmake_minimum_required(VERSION 3.30)
project(SystemProxy
        VERSION 1.0.0
        DESCRIPTION "A project to manage and modify system proxy settings via API"
        HOMEPAGE_URL "https://github.com/cheungxiongwei/SystemProxyConfig"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)