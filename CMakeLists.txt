cmake_minimum_required(VERSION 3.19)
project(FolderSync)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 1 - project_demo
# 2 - build_general_tool
# 3 - build_this_tool
set(BUILD_CODE 1)
if (BUILD_CODE STREQUAL 1)
    add_executable(FolderSync main.cpp FolderSync.hpp)
elseif(BUILD_CODE STREQUAL 2)
    add_executable(FolderSyncGeneralTool main_general_tool.cpp FolderSync.hpp)
elseif(BUILD_CODE STREQUAL 3)
    add_executable(FolderSyncThisTool main_this_tool.cpp FolderSync.hpp)
else()
    add_executable(FolderSync main.cpp FolderSync.hpp)
    add_executable(FolderSyncGeneralTool main_general_tool.cpp FolderSync.hpp)
    add_executable(FolderSyncThisTool main_this_tool.cpp FolderSync.hpp)
endif ()
