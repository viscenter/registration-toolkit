cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project("registration-toolkit" VERSION 1.1.0)

# Modules
include(FetchContent)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Use modern C++ for everything and generate compile_commands database.
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Choose what to build
option(RT_BUILD_APPS     "Compile core programs"    on)
option(RT_BUILD_DOC      "Compile documentation"    off)
option(RT_BUILD_TESTS    "Compile unit tests"       off)

# Choose what to install
option(RT_INSTALL_APPS      "Install core programs"    on)
option(RT_INSTALL_DOC       "Install documentation"    off)

# Look for external dependencies.
include(FindDependencies)

# Some helpful constants to be used in subprojects.
include(Constants)

# Enable warnings
include(Warnings)

## Libraries ##
add_subdirectory(rtlib)

## Core Apps ##
if (RT_BUILD_APPS)
    add_subdirectory(apps)
endif()

## Documentation
if (RT_BUILD_DOC)
    add_subdirectory(doc)
endif()

## Tests
if(RT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install to system directories
include(Install)
