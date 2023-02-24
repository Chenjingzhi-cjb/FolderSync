cmake_minimum_required(VERSION 3.19)
project(FolderSync)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(FolderSync main.cpp FolderSync.hpp)
