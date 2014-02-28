cmake_minimum_required(VERSION 2.8)

project(Registration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(ApplyTransform src/ApplyTransform.cxx)
add_executable(LandmarkRegistration src/LandmarkRegistration.cxx)

target_link_libraries(ApplyTransform ${ITK_LIBRARIES})
target_link_libraries(LandmarkRegistration ${ITK_LIBRARIES})
