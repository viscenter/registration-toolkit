# 2D Registration Apps
add_executable(rt_apply_transform src/ApplyTransform.cpp)
target_link_libraries(rt_apply_transform
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    Boost::filesystem
    Boost::program_options
    ${ITKIOTransformLibs}
)

add_executable(rt_register2d src/Register2D.cpp)
target_link_libraries(rt_register2d
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    Boost::filesystem
    Boost::program_options
    ${ITKIOTransformLibs}
)

# 3D Registration Apps
add_executable(rt_reorder_texture src/ReorderTexture.cpp)
target_link_libraries(rt_reorder_texture
    rt::rtlib
    Boost::filesystem
    Boost::program_options
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
    Boost::program_options
    ${ITKIOTransformLibs}
)

add_executable(rt_generate_landmarks src/GenerateLandmarks.cpp)
target_link_libraries(rt_generate_landmarks
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    opencv_features2d
    opencv_highgui
)

add_executable(rt_plot_landmarks src/PlotLandmarks.cpp)
target_link_libraries(rt_plot_landmarks
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Boost::filesystem
    Boost::program_options
)

add_executable(rt_swap_landmarks src/SwapLandmarks.cpp)
target_link_libraries(rt_swap_landmarks
    rt::rtlib
    Boost::filesystem
    Boost::program_options
)

add_executable(rt_segment_disegni src/SegmentDisegni.cpp)
target_link_libraries(rt_segment_disegni
    rt::rtlib
    opencv_core
    opencv_imgcodecs
    opencv_highgui
    Boost::filesystem
    Boost::program_options
)

add_executable(rt_retexture_mesh src/RetextureMesh.cpp)
target_link_libraries(rt_retexture_mesh
    rt::rtlib
    Boost::filesystem
    Boost::program_options
)


if(VC_FOUND)
    add_executable(rt_dewarp src/TextureDewarp.cpp)
    target_link_libraries(rt_dewarp
        rt::rtlib
        VC::texturing
        Boost::filesystem
    )
endif()

install(
    TARGETS
        rt_apply_transform
        rt_register2d
        rt_reorder_texture
        rt_raw_tiff_converter
        rt_register3d
        rt_generate_landmarks
        rt_plot_landmarks
        rt_swap_landmarks
        rt_segment_disegni
        rt_retexture_mesh
    RUNTIME DESTINATION bin
    COMPONENT Programs
)
