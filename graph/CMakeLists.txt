set(srcs
    src/DeformableRegistration.cpp
    src/ImageIO.cpp
    src/LandmarkIO.cpp
    src/LandmarkRegistration.cpp
    src/Transforms.cpp
)

add_library(rt_graph ${srcs})
add_library("${namespace}graph" ALIAS rt_graph)
target_include_directories(rt_graph
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(rt_graph
    PUBLIC
        rt::core
        ${RT_FS_LIB}
        smgl::smgl
        opencv_core
)
target_compile_features(rt_graph PUBLIC cxx_std_14)
set_target_properties(rt_graph PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "graph"
)

install(
    TARGETS "rt_graph"
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    INCLUDES DESTINATION "${include_install_dir}"
    RUNTIME DESTINATION "bin"
)
install(
    DIRECTORY "include/rt/graph"
    DESTINATION "${include_install_dir}"
)
