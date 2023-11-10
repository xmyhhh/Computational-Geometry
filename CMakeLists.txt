cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(CppGC VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH third/opencv4)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

add_executable(MAIN src/main.cpp )
target_link_libraries(MAIN  ${OpenCV_LIBS})