cmake_minimum_required(VERSION 3.22)

set(SML_SLN  "001-glx-widnow-sln")
project(${SML_SLN})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)

add_subdirectory(001-glx-widnow)