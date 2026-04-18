include_guard(GLOBAL)

##
# @brief Prevent configuring CMake directly into the source tree.
#
# A dedicated build directory keeps generated files out of the repository and
# avoids collisions with source-controlled paths such as `.clangd` and docs.
#
# @return None. Emits a fatal error if the source and binary directories match.
##
function(prevent_in_source_builds)
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
        message(
            FATAL_ERROR
                "In-source builds are not supported. Configure CMake in a separate build directory such as '_build/debug'."
        )
    endif()
endfunction()
