include_guard(GLOBAL)

##
# @brief Apply shared project-wide CMake defaults.
#
# These settings are intentionally generic so the module can be reused in other
# Linux-first repositories without carrying project-specific target logic.
#
# @return None.
##
function(apply_default_project_settings)
    set(CMAKE_EXPORT_COMPILE_COMMANDS
        ON
        PARENT_SCOPE)
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG
        TRUE
        PARENT_SCOPE)

    if(NOT DEFINED CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD
            20
            PARENT_SCOPE)
    endif()

    set(CMAKE_CXX_STANDARD_REQUIRED
        ON
        PARENT_SCOPE)
    set(CMAKE_CXX_EXTENSIONS
        OFF
        PARENT_SCOPE)

    if(NOT DEFINED CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE
            Debug
            CACHE STRING "Build type" FORCE)
    endif()

    message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")
endfunction()
