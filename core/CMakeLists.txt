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
    src/ImageStitcher.cpp
)

configure_file(src/Version.cpp.in Version.cpp)

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
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(rt_core
    PUBLIC
        ${RT_FS_LIB}
        opencv_core
        opencv_features2d
        ITKCommon
        ITKTransform
        vtkCommonCore
        vtkCommonDataModel
    PRIVATE
        opencv_imgcodecs
        opencv_imgproc
        opencv_stitching
        ITKOptimizers
        ${ITKIOTransformLibs}
        vtkCommonTransforms
        vtkFiltersCore
        vtkFiltersGeneral
        TIFF::TIFF
)
if(TARGET ITKSmoothing)
    target_link_libraries(rt_core PRIVATE ITKSmoothing)
endif()
target_compile_features(rt_core PUBLIC cxx_std_14)
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
