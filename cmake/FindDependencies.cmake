########
# Core #
########

# For compiler sanitizers. Taken from:
# https://github.com/arsenm/sanitizers-cmake/blob/master/README.md
find_package(Sanitizers)

### Boost ###
find_package(Boost REQUIRED COMPONENTS filesystem)

### OpenCV ###
find_package(OpenCV 3 REQUIRED)

### ITK ###
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
set(ITKIOTransformLibs 
    ITKIOTransformBase 
    ITKIOTransformHDF5 
    ITKIOTransformInsightLegacy 
    ITKIOTransformMatlab
)

### VTK ###
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# VTK does not mark its headers as system headers with -isystem, which makes
# warnings from those headers show up in builds. This marks them as "system"
# headers.
include_directories(SYSTEM ${VTK_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)
add_library(eigen3 INTERFACE IMPORTED)
set_target_properties(eigen3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${EIGEN3_INCLUDE_DIR}"
)

find_package(VC 2.13 REQUIRED)

############
# Optional #
############

# If this option is set, then use all optional dependencies
option(PROJ_USE_ALL "Use all optional third-party libs" off)
if(PROJ_USE_ALL)
    message(STATUS "All optional third-party libraries enabled. Individual \
preferences will be ignored.")
endif()
