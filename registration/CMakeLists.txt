cmake_minimum_required(VERSION 2.8)

project(Registration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(ApplyTransform src/ApplyTransform.cxx)
add_executable(LandmarkRegistration src/LandmarkRegistration.cxx)
add_executable(DeformableRegistration src/DeformableRegistration13.cxx)
add_executable(LandmarkWarping src/LandmarkWarping2.cxx)

target_link_libraries(ApplyTransform ${ITK_LIBRARIES})
target_link_libraries(LandmarkRegistration ${ITK_LIBRARIES})
target_link_libraries(DeformableRegistration ${ITK_LIBRARIES})
target_link_libraries(LandmarkWarping ${ITK_LIBRARIES})
