set(io_srcs
    src/OBJReader.cpp
    src/OBJWriter.cpp
    src/TIFFIO.cpp
    src/LandmarkIO.cpp
    src/ImageIO.cpp
    src/UVMapIO.cpp
)

set(type_srcs
    src/UVMap.cpp
    src/ITK2VTK.cpp
    src/Transforms.cpp
)

set(util_srcs
    src/ImageConversion.cpp
)

set(srcs
    src/ReorderUnorganizedTexture.cpp
    src/LandmarkDetector.cpp
    src/DeformableRegistration.cpp
    src/AffineLandmarkRegistration.cpp
    src/ImageTransformResampler.cpp
    src/BSplineLandmarkWarping.cpp
    src/DisegniSegmenter.cpp
)

configure_file(src/Version.cpp.in Version.cpp)

# VTK libraries
set(core_vtk_public CommonCore CommonDataModel)
set(core_vtk_private CommonTransforms FiltersCore FiltersGeneral)
if(VTK_VERSION_MAJOR VERSION_GREATER_EQUAL 9)
  list(TRANSFORM core_vtk_public PREPEND "VTK::")
  list(TRANSFORM core_vtk_private PREPEND "VTK::")
else()
  list(TRANSFORM core_vtk_public PREPEND "vtk")
  list(TRANSFORM core_vtk_private PREPEND "vtk")
endif()

add_library(rt_core
    ${srcs}
    ${io_srcs}
    ${type_srcs}
    ${util_srcs}
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
)
add_library("${namespace}core" ALIAS "rt_core")
target_include_directories(rt_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${bvh_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(rt_core
    PUBLIC
        ${RT_FS_LIB}
        opencv_core
        ITKCommon
        ITKTransform
        ${core_vtk_public}
    PRIVATE
        opencv_features2d
        opencv_imgcodecs
        opencv_imgproc
        opencv_stitching
        ITKOptimizers
        ${ITKIOTransformLibs}
        ${core_vtk_private}
        TIFF::TIFF
)
if(TARGET ITKSmoothing)
    target_link_libraries(rt_core PRIVATE ITKSmoothing)
endif()
target_compile_features(rt_core PUBLIC cxx_std_17)
set_target_properties(rt_core PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "core"
)

install(
    TARGETS "rt_core"
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    INCLUDES DESTINATION "${include_install_dir}"
    RUNTIME DESTINATION "bin"
)
install(
    DIRECTORY "include/rt/"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING REGEX ".*\.(h|hpp)$"
)
