cmake_minimum_required(VERSION 3.25)

###########################################################################################
# Create threading library interface target
# 
# The library is header-only, so this target exists to publish include paths
# and transitive link requirements without introducing a compiled archive.
###########################################################################################
add_library(threading_library INTERFACE)
add_library(threading_library::threading_library ALIAS threading_library)

###########################################################################################
# Specify public include directories
###########################################################################################
target_include_directories(
    threading_library
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes"
)
###########################################################################################
# Required packages
###########################################################################################
find_package(Threads REQUIRED)

###########################################################################################
# Add library dependencies
# 
# Even though the library is header-only, consumers still need pthread linkage
# when they instantiate the concurrency primitives.
###########################################################################################
target_link_libraries(
    threading_library
    INTERFACE
    Threads::Threads
)
