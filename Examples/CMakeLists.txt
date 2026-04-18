cmake_minimum_required(VERSION 3.25)

###########################################################################################
# Aggregate example applications
# 
# The custom target gives the top-level Make/CMake workflow a stable handle to
# "build every example" without coupling that workflow to individual app names.
###########################################################################################
add_custom_target(examples)

###########################################################################################
# Add example applications
###########################################################################################
add_subdirectory(SmokeApp)

# NOTE: To add a new example, create a new subdirectory with its own CMakeLists.txt and add
# a new add_subdirectory() call here. The new example's CMakeLists.txt should
# register its executable as a dependency of the "examples" target so it gets built
# when users build the "examples" target.