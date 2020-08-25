cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project("registration-toolkit" VERSION 1.1.0)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Use modern C++ for everything and generate compile_commands database.
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Choose what to build
option(BUILD_APPS     "Compile core programs"    on)
option(BUILD_DOC      "Compile documentation"    off)

# Choose what to install
option(INSTALL_APPS      "Install core programs"    on)
option(INSTALL_DOC       "Install documentation"    off)

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
add_subdirectory(rtlib)

## Core Apps ##
if (BUILD_APPS)
    add_subdirectory(apps)
endif()

## Documentation
if (BUILD_DOC)
    add_subdirectory(doc)
endif()

# Install to system directories
include(Install)
