cmake_minimum_required(VERSION 2.8)
 
PROJECT(manipulate)

# Use modern C++ for everything
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED on)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(manipulate ${SRC_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(manipulate ${VTK_LIBRARIES} opencv_core)
endif()
