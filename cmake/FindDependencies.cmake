########
# Core #
########

## Filesystem ##
find_package(Filesystem)
option(RT_USE_BOOSTFS "Use Boost as the filesystem library" "NOT Filesystem_FOUND")
if(RT_USE_BOOSTFS)
    add_compile_definitions(RT_USE_BOOSTFS)
    find_package(Boost 1.58 REQUIRED COMPONENTS system filesystem)
    set(RT_FS_LIB Boost::filesystem)
else()
    set(RT_FS_LIB std::filesystem)
endif()
message(STATUS "Using filesystem library: ${RT_FS_LIB}")

### OpenCV ###
find_package(OpenCV 4 QUIET REQUIRED)

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
if(VTK_VERSION_MAJOR VERSION_LESS 8.9)
  include(${VTK_USE_FILE})
endif()

### libtiff ###
find_package(TIFF REQUIRED)

### smeagol ###
include(BuildSmeagol)

### bvh ###
include(Buildbvh)

############
# Optional #
############
option(RT_USE_VOLCART "Use the Volume Cartographer library" off)
if(RT_USE_VOLCART)
    find_package(VC 2.13 CONFIG)
endif()
