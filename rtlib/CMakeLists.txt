project(librtlib VERSION 0.1 LANGUAGES CXX)
set(module "rtlib")
set(target "${module}")

set(io_srcs
    src/OBJReader.cpp
    src/OBJWriter.cpp
    src/ReadRawTIFF.cpp
    src/LandmarkReader.cpp
    src/LandmarkWriter.cpp
)

set(type_srcs
    src/UVMap.cpp
    src/ITK2VTK.cpp
)

set(srcs
    src/ReorderUnorganizedTexture.cpp
    src/LandmarkDetector.cpp
    src/DeformableRegistration.cpp
    src/AffineLandmarkRegistration.cpp
    src/ImageTransformResampler.cpp
    src/BSplineLandmarkWarping.cpp
    src/ImageStitcher.cpp
)

add_library(${target} ${srcs} ${io_srcs} ${type_srcs})
add_library("${namespace}${module}" ALIAS "${target}")
target_include_directories(${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${target}
    PUBLIC
        Boost::filesystem
        opencv_core
        ITKCommon
        ITKTransform
        vtkCommonCore
        vtkCommonDataModel
    PRIVATE
        opencv_calib3d
        opencv_features2d
        opencv_highgui
        opencv_imgproc
        opencv_stitching
        ITKOptimizers
        vtkCommonTransforms
        vtkFiltersGeneral
        TIFF::TIFF
)
set_target_properties(${target} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "${module}"
)

install(
    TARGETS "${target}"
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    INCLUDES DESTINATION "${include_install_dir}"
    RUNTIME DESTINATION "bin"
)
install(
    DIRECTORY "include/${module}"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING REGEX ".*\.(h|hpp)$"
)
