cmake_minimum_required(VERSION 2.8)
 
<<<<<<< HEAD
PROJECT(manipulate CXX)
=======
PROJECT(manipulate)
>>>>>>> 0a9802a096811820c073391039fc7503d50071ea

# Use modern C++ for everything
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED on)
<<<<<<< HEAD

find_package(OpenCV REQUIRED)
=======
>>>>>>> 0a9802a096811820c073391039fc7503d50071ea
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(manipulate ${SRC_FILES})
<<<<<<< HEAD
target_include_directories(manipulate
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(manipulate ITKTransform opencv_core ${VTK_LIBRARIES} vtkIOImport)
=======
 
if(VTK_LIBRARIES)
  target_link_libraries(manipulate ${VTK_LIBRARIES} opencv_core)
endif()
>>>>>>> 0a9802a096811820c073391039fc7503d50071ea
