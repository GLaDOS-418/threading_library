cmake_minimum_required(VERSION 3.25)

###########################################################################################
# Create smoke example executable
###########################################################################################
add_executable(threading_library_smoke_app)

###########################################################################################
# Add example sources
###########################################################################################
target_sources(
    threading_library_smoke_app
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

###########################################################################################
# Add library to the example executable
###########################################################################################
target_link_libraries(
    threading_library_smoke_app
    PRIVATE
    threading_library::threading_library
)

###########################################################################################
# Apply common compiler options
###########################################################################################
enable_project_warnings(threading_library_smoke_app)
enable_project_sanitizers(threading_library_smoke_app)
enable_project_iwyu(threading_library_smoke_app)

###########################################################################################
# Add the threading_library_smoke_app target as a dependency of the "examples" aggregator
# target so it gets built when users build the "examples" target from the top level using
# "make examples" or "cmake --build . --target examples". This keeps the top-level workflow
# decoupled from individual example names while still giving users a stable handle to "build
# all examples".
###########################################################################################
add_dependencies(examples threading_library_smoke_app)
