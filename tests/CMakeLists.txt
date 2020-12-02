## Google Test ##
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
    CMAKE_CACHE_ARGS
        -DINSTALL_GTEST:BOOL=OFF
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    set(INSTALL_GTEST OFF CACHE BOOL OFF FORCE)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

## Build the tests ##
set(tests
    src/TestITKOCVBridge.cpp
    src/TestString.cpp
    src/TestUVMapIO.cpp
)

foreach(src ${tests})
    get_filename_component(filename ${src} NAME_WE)
    set(testname core_${filename})
    add_executable(${testname} ${src})
    target_link_libraries(${testname}
        rt::core
        gtest_main
        gmock_main
    )
    add_test(
        NAME ${testname}
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${testname}
    )
endforeach()