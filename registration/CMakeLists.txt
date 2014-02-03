cmake_minimum_required(VERSION 2.8)

project(Registration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# add_executable(Registration Registration.cxx)
add_executable(DeformableRegistration DeformableRegistration13.cxx)
add_executable(LandmarkWarping LandmarkWarping2.cxx)

# target_link_libraries(Registration ${ITK_LIBRARIES})
target_link_libraries(DeformableRegistration ${ITK_LIBRARIES})
target_link_libraries(LandmarkWarping ${ITK_LIBRARIES})
