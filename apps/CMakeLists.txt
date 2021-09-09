# Only apps *require* Boost
find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(rt_apply_transform src/ApplyTransform.cpp)
target_link_libraries(rt_apply_transform
    rt::core
    Boost::program_options
)

add_executable(rt_register src/Register.cpp)
target_link_libraries(rt_register
    rt::core
    rt::graph
    Boost::program_options
    smgl::smgl
)

add_executable(rt_reorder_texture src/ReorderTexture.cpp)
target_link_libraries(rt_reorder_texture
    rt::core
    Boost::program_options
)

add_executable(rt_raw_tiff_converter src/RawTIFFConverter.cpp)
target_link_libraries(rt_raw_tiff_converter
    rt::core
)

add_executable(rt_generate_landmarks src/GenerateLandmarks.cpp)
target_link_libraries(rt_generate_landmarks
    rt::core
)

add_executable(rt_plot_landmarks src/PlotLandmarks.cpp)
target_link_libraries(rt_plot_landmarks
    rt::core
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    Boost::program_options
)

add_executable(rt_swap_landmarks src/SwapLandmarks.cpp)
target_link_libraries(rt_swap_landmarks
    rt::core
    Boost::program_options
)

add_executable(rt_segment_disegni src/SegmentDisegni.cpp)
target_link_libraries(rt_segment_disegni
    rt::core
    Boost::program_options
)

add_executable(rt_retexture_mesh src/RetextureMesh.cpp)
target_link_libraries(rt_retexture_mesh
    rt::core
    Boost::program_options
)


if(VC_FOUND)
    add_executable(rt_dewarp src/TextureDewarp.cpp)
    target_link_libraries(rt_dewarp
        rt::core
        VC::texturing
    )
endif()

install(
    TARGETS
        rt_apply_transform
        rt_register
        rt_reorder_texture
        rt_raw_tiff_converter
        rt_generate_landmarks
        rt_plot_landmarks
        rt_swap_landmarks
        rt_segment_disegni
        rt_retexture_mesh
    RUNTIME DESTINATION bin
    COMPONENT Programs
)
