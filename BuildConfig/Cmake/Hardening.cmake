include_guard(GLOBAL)
include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)

##
# @brief Append a compile flag when the active compiler accepts it.
#
# @param output_variable Parent-scope list variable to extend.
# @param candidate_flag Compiler flag to probe and append when supported.
##
function(_project_append_supported_compile_flag output_variable candidate_flag)
    string(MAKE_C_IDENTIFIER "${candidate_flag}" candidate_identifier)
    set(check_variable "PROJECT_HAS_${candidate_identifier}")
    check_cxx_compiler_flag("${candidate_flag}" ${check_variable})

    set(updated_flags "${${output_variable}}")
    if(${check_variable})
        list(APPEND updated_flags "${candidate_flag}")
    endif()

    set(${output_variable}
        "${updated_flags}"
        PARENT_SCOPE)
endfunction()

##
# @brief Append a linker flag when the active toolchain accepts it.
#
# @param output_variable Parent-scope list variable to extend.
# @param candidate_flag Linker flag to probe and append when supported.
##
function(_project_append_supported_link_flag output_variable candidate_flag)
    string(MAKE_C_IDENTIFIER "${candidate_flag}" candidate_identifier)
    set(check_variable "PROJECT_LINKER_HAS_${candidate_identifier}")
    check_linker_flag(CXX "${candidate_flag}" ${check_variable})

    set(updated_flags "${${output_variable}}")
    if(${check_variable})
        list(APPEND updated_flags "${candidate_flag}")
    endif()

    set(${output_variable}
        "${updated_flags}"
        PARENT_SCOPE)
endfunction()

##
# @brief Apply generic Linux/GCC/Clang hardening flags to a concrete target.
#
# Hardening remains target-local so downstream consumers of the header-only
# library are not forced to inherit these flags. The stack protector flag is
# kept explicitly because the repository used it before the CMake refactor.
#
# @param target_name Concrete target that should receive hardening flags.
##
function(enable_project_hardening target_name)
    if(NOT ENABLE_HARDENING)
        return()
    endif()

    if(NOT
       CMAKE_SYSTEM_NAME
       STREQUAL
       "Linux")
        message(AUTHOR_WARNING "Project hardening helpers are currently tuned for Linux only.")
        return()
    endif()

    if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        message(AUTHOR_WARNING "No hardening profile is defined for compiler '${CMAKE_CXX_COMPILER_ID}'.")
        return()
    endif()

    set(hardening_compile_options)
    set(hardening_link_options)

    _project_append_supported_compile_flag(hardening_compile_options -fstack-protector-all)
    _project_append_supported_compile_flag(hardening_compile_options -fcf-protection)
    _project_append_supported_compile_flag(hardening_compile_options -fstack-clash-protection)
    _project_append_supported_link_flag(hardening_link_options -Wl,-z,relro,-z,now)

    if(hardening_compile_options)
        target_compile_options(${target_name} PRIVATE ${hardening_compile_options})
    endif()

    if(hardening_link_options)
        target_link_options(${target_name} PRIVATE ${hardening_link_options})
    endif()

    target_compile_definitions(${target_name} PRIVATE _GLIBCXX_ASSERTIONS "$<$<NOT:$<CONFIG:Debug>>:_FORTIFY_SOURCE=3>")

    target_compile_options(${target_name} PRIVATE "$<$<NOT:$<CONFIG:Debug>>:-U_FORTIFY_SOURCE>")
endfunction()
