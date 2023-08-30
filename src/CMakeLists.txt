cmake_minimum_required(VERSION 3.5)

project(AsmZCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AsmZCompiler main.cpp
    aboba.z
)

include(GNUInstallDirs)
install(TARGETS AsmZCompiler
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
