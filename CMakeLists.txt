cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project("registration-toolkit" VERSION 1.5.0)

# Modules
include(FetchContent)
include(CMakeDependentOption)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Look for external dependencies.
include(FindDependencies)

# Some helpful constants to be used in subprojects.
include(Constants)

# XXX Hack to get ninja output colorized for all source files.
if (CMAKE_GENERATOR MATCHES "Ninja")
    add_compile_options(-fdiagnostics-color=always)
endif()

# Enable warnings
include(Warnings)

## Libraries ##
add_subdirectory(core)
add_subdirectory(graph)

## Core Apps ##
option(RT_BUILD_APPS "Compile core programs" on)
CMAKE_DEPENDENT_OPTION(RT_INSTALL_APPS "Install core programs" on "RT_BUILD_APPS" off)
if (RT_BUILD_APPS)
    add_subdirectory(apps)
endif()

## Documentation
find_package(Doxygen OPTIONAL_COMPONENTS dot)
CMAKE_DEPENDENT_OPTION(RT_BUILD_DOCS "Build Doxygen documentation" on "DOXYGEN_FOUND" off)
CMAKE_DEPENDENT_OPTION(RT_INSTALL_DOCS "Install documentation" off "RT_BUILD_DOCS" off)
if(RT_BUILD_DOCS)
    add_subdirectory(docs)
endif()

## Tests
option(RT_BUILD_TESTS "Compile unit tests" off)
if(RT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install to system directories
include(Install)
