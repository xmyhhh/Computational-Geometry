cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(CppGC VERSION 0.1.0)

add_definitions(-DVK_EXAMPLE_ASSETS_DIR=\"${CMAKE_SOURCE_DIR}/asset/\")
add_definitions(-DVK_EXAMPLE_SHADERS_DIR=\"${CMAKE_SOURCE_DIR}/shader/compile\")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH third/opencv4)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_PATH})

include_directories(src)
include_directories(third/glm)
include_directories(third/tinygltf)

include_directories(third/ktx/include)
include_directories(third/ktx/other_include)
link_directories(third/ktx/lib)

include_directories(third/imgui)
include_directories(third/glfw/include)
link_directories(third/glfw/lib-vc2022)


file(GLOB_RECURSE  SOURCE_FILES 
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE  IMGUI_SOURCE_FILES 
	${PROJECT_SOURCE_DIR}/third/imgui/*.cpp
)

add_executable(MAIN ${SOURCE_FILES} ${IMGUI_SOURCE_FILES})
target_link_libraries(MAIN  ${OpenCV_LIBS} ${Vulkan_LIBRARY} ktx glfw3)


file(GLOB  Opencv_Dll 
	${OpenCV_LIB_PATH}/../bin/*.dll
)

FILE(COPY 
    ${Opencv_Dll} 
    DESTINATION 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
