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

find_package(Eigen3)
add_library(eigen3 INTERFACE IMPORTED)
set_target_properties(eigen3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}"
)
find_package(VC 2.13)

if(VC_FOUND)
    add_executable(rt_dewarp src/TextureDewarp.cpp)
    target_link_libraries(rt_dewarp
        rt::rtlib
        VC::core
        VC::texturing
        opencv_core
        opencv_imgproc
        opencv_highgui
        ${VTK_LIBRARIES}
    )
endif()