cmake_minimum_required(VERSION 2.8)
project(lab0)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set(target "lab0")
add_executable(${target} main.cpp) # add other .cpp
                                       # and .h files here
target_link_libraries(${target} ${OpenCV_LIBS})
