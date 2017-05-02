project(librtlib VERSION 0.1 LANGUAGES CXX)
set(module "rtlib")
set(target "${module}")

set(srcs
    src/ReorderUnorganizedTexture.cpp
    src/ReadRawTIFF.cpp
    src/LandmarkDetector.cpp
    src/DeformableRegistration.cpp
    src/AffineLandmarkRegistration.cpp
    src/LandmarkReader.cpp
    src/ImageTransformResampler.cpp
    src/BSplineLandmarkWarping.cpp
)

add_library(${target} ${srcs})
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
        opencv_features2d
        opencv_highgui
        ITKCommon
        ITKTransform
        ITKOptimizers
        VC::core
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
