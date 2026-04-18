cmake_minimum_required(VERSION 3.25)

project(threading_library VERSION 0.1.0 LANGUAGES CXX)

###########################################################################################
# Include common CMake configuration
###########################################################################################
include("${CMAKE_CURRENT_SOURCE_DIR}/BuildConfig/Cmake/DefaultProjectCmakeSettings.cmake")

###########################################################################################
# Project options
###########################################################################################
option(BUILD_EXAMPLES "Build example executables" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_ASAN "Enable AddressSanitizer in Debug builds" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer in Debug builds" OFF)
option(ENABLE_IWYU "Run include-what-you-use on repo-owned compile targets" OFF)

###########################################################################################
# Add library
###########################################################################################
add_subdirectory(Library)

###########################################################################################
# Add examples
###########################################################################################
if(BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

###########################################################################################
# Add tests
###########################################################################################
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()
