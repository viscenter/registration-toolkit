set(module "core")

set(io_srcs
    src/OBJReader.cpp
    src/OBJWriter.cpp
    src/TIFFIO.cpp
    src/LandmarkReader.cpp
    src/LandmarkWriter.cpp
    src/ImageIO.cpp
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

add_library(rt_${module}
    ${srcs}
    ${io_srcs}
    ${type_srcs}
    ${util_srcs}
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
)
add_library("${namespace}${module}" ALIAS "rt_${module}")
target_include_directories(rt_${module}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(rt_${module}
    PUBLIC
        Boost::filesystem
        opencv_core
        ITKCommon
        ITKTransform
        vtkCommonCore
        vtkCommonDataModel
        smgl::smgl
    PRIVATE
        opencv_features2d
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
    target_link_libraries(rt_${module} PRIVATE ITKSmoothing)
endif()
target_compile_features(rt_${module} PUBLIC cxx_std_14)
set_target_properties(rt_${module} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "${module}"
)

install(
    TARGETS "rt_${module}"
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
