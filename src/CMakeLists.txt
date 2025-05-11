if (NOT "${CMAKE_CURRENT_LIST_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    add_library(system_proxy STATIC
            ${CMAKE_CURRENT_LIST_DIR}/SystemProxy.h
            ${CMAKE_CURRENT_LIST_DIR}/SystemProxy.cpp
    )
    target_include_directories(system_proxy
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    )
else ()
    add_executable(app
            ${CMAKE_CURRENT_LIST_DIR}/main.cpp
            ${CMAKE_CURRENT_LIST_DIR}/SystemProxy.h
            ${CMAKE_CURRENT_LIST_DIR}/SystemProxy.cpp
    )
endif ()