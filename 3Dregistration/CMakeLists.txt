cmake_minimum_required(VERSION 2.8)
 
PROJECT(manipulate)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(manipulate ${SRC_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(manipulate ${VTK_LIBRARIES})
endif()
