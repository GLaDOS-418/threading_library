cmake_minimum_required(VERSION 3.25)

###########################################################################################
# Find test dependencies
###########################################################################################
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

###########################################################################################
# Create unit test executable
###########################################################################################
add_executable(threading_library_tests)

###########################################################################################
# Add library dependencies
###########################################################################################
target_link_libraries(
    threading_library_tests
    PRIVATE
    GTest::gtest_main
    threading_library::threading_library
)

###########################################################################################
# Apply common compiler options
###########################################################################################
enable_project_build_modes(threading_library_tests)
enable_project_warnings(threading_library_tests)
enable_project_hardening(threading_library_tests)
enable_project_sanitizers(threading_library_tests)
enable_project_optional_tools(threading_library_tests)

###########################################################################################
# Add test sources
###########################################################################################
add_subdirectory(Src)

###########################################################################################
# Discover unit tests
###########################################################################################
gtest_discover_tests(threading_library_tests)
