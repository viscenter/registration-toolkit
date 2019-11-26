########
# Core #
########

include(FetchContent)

## Envi-tools ##
FetchContent_Declare(
        envi-tools
        GIT_REPOSITORY
        GIT_TAG release-
)

FetchContent_GetProperties(envi-tools)
if(NOT envi-tools_POPULATED)
    #Override default options

    FetchContent_Populate(envi-tools)


# For compiler sanitizers. Taken from:
# https://github.com/arsenm/sanitizers-cmake/blob/master/README.md
find_package(Sanitizers)

### Boost ###
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

### OpenCV ###
find_package(OpenCV 3 QUIET)
if(NOT OpenCV_FOUND)
    find_package(OpenCV 4 QUIET REQUIRED)
endif()

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

### libtiff ###
find_package(TIFF REQUIRED)

############
# Optional #
############
option(RT_USE_VOLCART "Use the Volume Cartographer library" off)
if(RT_USE_VOLCART)
    find_package(VC 2.13 CONFIG)
endif()
