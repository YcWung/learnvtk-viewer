cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

PROJECT (Viewer)

find_package(VTK REQUIRED)
include_directories(${VTK_INCLUDE_DIRS})

add_executable(Viewer viewer.cxx main.cxx viewer.h)
target_link_libraries(Viewer ${VTK_LIBRARIES})
