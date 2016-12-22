cmake_minimum_required(VERSION 2.8)
 
PROJECT(manipulate CXX)

# Use modern C++ for everything
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED on)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(Eigen3 REQUIRED)
add_library(eigen3 INTERFACE IMPORTED)
set_target_properties(eigen3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}"
)

find_package(LIBIGL QUIET REQUIRED)
add_library(libigl INTERFACE IMPORTED)
set_target_properties(libigl PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${LIBIGL_INCLUDE_DIRS}"
)

find_package(OpenCV REQUIRED)
 
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(OpenCV REQUIRED)

find_package(VC REQUIRED)

add_executable(manipulate src/main.cpp src/manipulator.cpp src/obj_reader.cpp)
target_include_directories(manipulate
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(manipulate ITKTransform opencv_core opencv_imgproc opencv_highgui ${VTK_LIBRARIES} vtkIOImport VC::core)

add_executable(retexture src/retexture.cpp)
target_link_libraries(retexture Boost::filesystem opencv_core VC::core VC::meshing VC::texturing)