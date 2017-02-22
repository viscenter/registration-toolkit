# 2D Registration Apps
add_executable(ApplyTransform src/ApplyTransform.cxx)
target_link_libraries(ApplyTransform ${ITK_LIBRARIES})

add_executable(LandmarkRegistration src/LandmarkRegistration.cxx)
target_link_libraries(LandmarkRegistration
    rt::rtlib
    ${ITK_LIBRARIES}
    )

# 3D Registration Apps
add_executable(rt_reorder_texture src/ReorderTexture.cpp)
target_link_libraries(rt_reorder_texture
        rt::rtlib
        opencv_core
        opencv_imgproc
        opencv_highgui
        ${VTK_LIBRARIES}
        VC::core
        VC::meshing
)

add_executable(rt_retexture src/retexture.cpp)
target_link_libraries(rt_retexture
        Boost::filesystem
        opencv_core
        opencv_imgcodecs
        VC::core
        VC::meshing
        VC::texturing
)
