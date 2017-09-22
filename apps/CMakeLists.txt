# 2D Registration Apps
add_executable(ApplyTransform src/ApplyTransform.cxx)
target_link_libraries(ApplyTransform ${ITK_LIBRARIES})

add_executable(rt_register2d src/Register2D.cpp)
target_link_libraries(rt_register2d
    rt::rtlib
    ${ITK_LIBRARIES}
    Boost::filesystem
    Boost::program_options
)

# 3D Registration Apps
add_executable(rt_reorder_texture src/ReorderTexture.cpp)
target_link_libraries(rt_reorder_texture
    rt::rtlib
    opencv_core
    opencv_imgproc
    opencv_highgui
    ${VTK_LIBRARIES}
)

add_executable(rt_raw_tiff_converter src/RawTIFFConverter.cpp)
target_link_libraries(rt_raw_tiff_converter
    rt::rtlib
)

add_executable(rt_register3d src/Register3D.cpp)
target_link_libraries(rt_register3d
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    ${ITKIOTransformLibs}
)

add_executable(rt_brisk_match src/BriskMatch.cpp)
target_link_libraries(rt_brisk_match
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    opencv_features2d
    opencv_highgui
)
