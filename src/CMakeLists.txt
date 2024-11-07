cmake_minimum_required(VERSION 3.16)

add_library(SystemProxyConfig STATIC "SystemProxyConfig.cpp" "../include/ISystemProxyConfig.h" "../include/SystemProxyConfig.h")

target_include_directories(SystemProxyConfig PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

install(TARGETS SystemProxyConfig
        EXPORT SystemProxyConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/SystemProxyConfig/SystemProxyConfig.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SystemProxyConfig)