include_guard(GLOBAL)

##
# @brief Collect the warning flags for the active compiler family.
#
# GCC and Clang currently share the same warning policy in this repository, so
# the common list is the source of truth and stays sorted lexicographically for
# easier review.
#
# @param output_variable Name of the parent-scope variable that will receive the
#        warning flag list.
##
function(_project_get_warning_options output_variable)
    set(common_warning_flags
        -Wall
        -Wcast-align
        -Wconversion
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Werror
        -Wextra
        -Wextra-semi
        -Wformat=2
        -Wimplicit-fallthrough
        -Wlogical-op
        -Wnoexcept
        -Wnon-virtual-dtor
        -Wnull-dereference
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpedantic
        -Wshadow
        -Wsign-conversion
        -Wstrict-null-sentinel
        -Wsuggest-override
        -Wunused
        -Wuseless-cast)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(selected_warning_flags ${common_warning_flags})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(selected_warning_flags ${common_warning_flags})
    else()
        message(AUTHOR_WARNING "No project warning profile is defined for compiler '${CMAKE_CXX_COMPILER_ID}'.")
        set(selected_warning_flags)
    endif()

    set(${output_variable}
        "${selected_warning_flags}"
        PARENT_SCOPE)
endfunction()

##
# @brief Apply the repository warning profile to a concrete target.
#
# Repo-owned example and test targets get the stricter warning policy. The
# public header-only library target intentionally does not export warning flags
# to downstream consumers.
#
# @param target_name Concrete target that should compile with the warning set.
##
function(enable_project_warnings target_name)
    _project_get_warning_options(project_warning_flags)

    if(NOT project_warning_flags)
        return()
    endif()

    target_compile_options(${target_name} PRIVATE ${project_warning_flags})
endfunction()
