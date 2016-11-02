cmake_minimum_required(VERSION 2.8)
 
PROJECT(manipulate CXX)

# Use modern C++ for everything
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED on)

find_package(OpenCV REQUIRED)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(manipulate ${SRC_FILES})
target_include_directories(manipulate
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(manipulate ITKTransform opencv_core ${VTK_LIBRARIES} vtkIOImport)
