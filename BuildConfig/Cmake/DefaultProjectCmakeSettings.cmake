cmake_minimum_required(VERSION 3.25.0)

include_guard(GLOBAL)

###########################################################################################
# Configurations
#
# Keep project-wide defaults here so target CMakeLists stay focused on target
# intent rather than repeating build-system policy.
###########################################################################################
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")

if(ENABLE_IWYU)
    set(PROJECT_IWYU_EXECUTABLE "${PROJECT_IWYU_EXECUTABLE}" CACHE FILEPATH "Path to include-what-you-use executable")

    if(NOT PROJECT_IWYU_EXECUTABLE)
        find_program(PROJECT_IWYU_EXECUTABLE NAMES include-what-you-use iwyu)
    endif()

    if(NOT PROJECT_IWYU_EXECUTABLE)
        message(
            FATAL_ERROR
            "ENABLE_IWYU is ON, but include-what-you-use was not found in PATH. "
            "Set PROJECT_IWYU_EXECUTABLE explicitly if your shell config provides it "
            "only in interactive shells."
        )
    endif()

    message(STATUS "Using include-what-you-use: ${PROJECT_IWYU_EXECUTABLE}")
endif()

###########################################################################################
# Define common functions for targets
###########################################################################################
function(enable_project_warnings target_name)
    # INTERFACE is intentional here. Some call sites use real executables and
    # some may use interface targets, and the project wants one helper that can
    # apply consistently to both.
    target_compile_options(
        ${target_name}
        INTERFACE
        -fstack-protector-all
        -Wall
        -Wextra
        -Werror
        -Wpedantic
        "$<$<COMPILE_LANGUAGE:CXX>:-Wconversion>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wduplicated-branches>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wduplicated-cond>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wextra-semi>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wimplicit-fallthrough>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wlogical-op>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wnoexcept>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wnull-dereference>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wshadow>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wstrict-null-sentinel>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wunused>"
        "$<$<COMPILE_LANGUAGE:CXX>:-Wuseless-cast>"
        "$<$<CONFIG:Debug>:-Og>"
        "$<$<CONFIG:Debug>:-g>"
        "$<$<CONFIG:Release>:-O3>"
    )
endfunction()

function(enable_project_sanitizers target_name)
    set(sanitizer_flags)

    if(ENABLE_ASAN)
        list(APPEND sanitizer_flags -fsanitize=address)
    endif()

    if(ENABLE_UBSAN)
        list(APPEND sanitizer_flags -fsanitize=undefined)
    endif()

    if(NOT sanitizer_flags)
        return()
    endif()

    # Sanitizers stay opt-in and Debug-scoped so normal developer builds do not
    # depend on a specific runtime being installed.
    target_compile_options(
        ${target_name}
        INTERFACE
        "$<$<CONFIG:Debug>:${sanitizer_flags}>"
    )

    target_link_options(
        ${target_name}
        INTERFACE
        "$<$<CONFIG:Debug>:${sanitizer_flags}>"
    )
endfunction()

function(enable_project_iwyu target_name)
    if(NOT ENABLE_IWYU)
        return()
    endif()

    # IWYU is most useful on repo-owned concrete targets. Applying it as a
    # target property keeps consumer builds unaffected while still analyzing the
    # translation units that include this header-only library.
    set_property(
        TARGET ${target_name}
        PROPERTY CXX_INCLUDE_WHAT_YOU_USE
        "${PROJECT_IWYU_EXECUTABLE};-Xiwyu;--error_always"
    )
endfunction()
