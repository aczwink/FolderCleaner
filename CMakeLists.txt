cmake_minimum_required(VERSION 3.6)
project(FolderCleaner)
find_package(XP-CMake REQUIRED) #we need advanced cmake modules

set(CMAKE_CXX_STANDARD 20)

XPC_AutoBuildType()
XPC_OptForHost()
XPC_SetCompileDefinitions()

find_package(StdPlusPlus REQUIRED)

add_executable(foldercleaner src/main.cpp)
target_link_libraries(foldercleaner LINK_PUBLIC Std++ Std++Static)