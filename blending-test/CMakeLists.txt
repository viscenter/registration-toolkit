find_package(opencv REQUIRED) 

add_executable(foo ImageBlend.cpp)

target_link_libraries(foo opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc rt::core)
