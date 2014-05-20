cmake_minimum_required(VERSION 2.8)

project(Registration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

add_executable(ApplyTransform src/ApplyTransform.cxx)
add_executable(LandmarkRegistration src/LandmarkRegistration.cxx)

target_link_libraries(ApplyTransform ${ITK_LIBRARIES})
target_link_libraries(LandmarkRegistration ${ITK_LIBRARIES})
