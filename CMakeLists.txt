cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(CppGC VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH third/opencv4)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

file(GLOB_RECURSE  SOURCE_FILES 
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(MAIN ${SOURCE_FILES})
target_link_libraries(MAIN  ${OpenCV_LIBS})