include_guard(GLOBAL)

##
# @brief Resolve a required external tool and cache its path.
#
# @param output_variable Cache variable that should receive the resolved path.
# @param doc_string Cache entry help text.
# @param ARGN List of acceptable executable names to search for.
##
function(_project_require_program output_variable doc_string)
    set(program_names ${ARGN})
    set(resolved_program "${${output_variable}}")

    if(NOT resolved_program)
        find_program(
            resolved_program
            NAMES ${program_names}
            HINTS /usr/local/bin /usr/bin
            PATHS ENV PATH)
    endif()

    if(NOT resolved_program AND UNIX)
        foreach(program_name IN LISTS program_names)
            execute_process(
                COMMAND /bin/sh -lc "command -v ${program_name}"
                OUTPUT_VARIABLE resolved_program
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if(resolved_program)
                break()
            endif()
        endforeach()
    endif()

    if(NOT resolved_program)
        list(
            JOIN
            program_names
            ", "
            joined_program_names)
        message(
            FATAL_ERROR
                "Required tool was not found. Looked for: ${joined_program_names}. Set ${output_variable} explicitly if needed."
        )
    endif()

    set(${output_variable}
        "${resolved_program}"
        CACHE FILEPATH "${doc_string}" FORCE)
endfunction()

##
# @brief Validate optional external tooling requests at configure time.
#
# Tool discovery is centralized here so top-level configuration fails fast and
# repo-owned targets can assume the requested tool exists.
#
# @return None. Emits fatal errors if a requested tool is missing.
##
function(validate_project_optional_tools)
    if(ENABLE_IWYU)
        _project_require_program(
            PROJECT_IWYU_EXECUTABLE
            "Path to the include-what-you-use executable"
            include-what-you-use
            iwyu)
        message(STATUS "Using include-what-you-use: ${PROJECT_IWYU_EXECUTABLE}")
    endif()

    if(ENABLE_CLANG_TIDY)
        _project_require_program(PROJECT_CLANG_TIDY_EXECUTABLE "Path to the clang-tidy executable" clang-tidy)
        message(STATUS "Using clang-tidy: ${PROJECT_CLANG_TIDY_EXECUTABLE}")
    endif()
endfunction()

##
# @brief Attach optional analysis tools to a repo-owned concrete target.
#
# These tools are intentionally not applied to the public interface library so
# downstream consumers are unaffected.
#
# @param target_name Concrete target that should be analyzed.
##
function(enable_project_optional_tools target_name)
    if(ENABLE_IWYU)
        set_property(TARGET ${target_name} PROPERTY CXX_INCLUDE_WHAT_YOU_USE
                                                    "${PROJECT_IWYU_EXECUTABLE};-Xiwyu;--error_always")
    endif()

    if(ENABLE_CLANG_TIDY)
        set_property(TARGET ${target_name}
                     PROPERTY CXX_CLANG_TIDY "${PROJECT_CLANG_TIDY_EXECUTABLE};-extra-arg=-Wno-unknown-warning-option")
    endif()
endfunction()
