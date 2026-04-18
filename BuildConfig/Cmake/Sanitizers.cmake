include_guard(GLOBAL)
include(CheckCXXSourceCompiles)

##
# @brief Probe whether a sanitizer flag works at both compile and link time.
#
# @param sanitizer_flag Sanitizer compiler/linker flag such as `-fsanitize=address`.
# @param output_variable Parent-scope boolean variable that receives the result.
##
function(_project_check_sanitizer_support sanitizer_flag output_variable)
    if(NOT
       CMAKE_SYSTEM_NAME
       STREQUAL
       "Linux")
        set(${output_variable}
            FALSE
            PARENT_SCOPE)
        return()
    endif()

    if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        set(${output_variable}
            FALSE
            PARENT_SCOPE)
        return()
    endif()

    set(previous_required_flags "${CMAKE_REQUIRED_FLAGS}")
    set(previous_required_link_options "${CMAKE_REQUIRED_LINK_OPTIONS}")
    set(previous_required_quiet "${CMAKE_REQUIRED_QUIET}")

    set(CMAKE_REQUIRED_QUIET TRUE)
    set(CMAKE_REQUIRED_FLAGS "${previous_required_flags} ${sanitizer_flag}")
    set(CMAKE_REQUIRED_LINK_OPTIONS ${previous_required_link_options} "${sanitizer_flag}")

    string(MAKE_C_IDENTIFIER "${sanitizer_flag}" sanitizer_identifier)
    set(test_variable "PROJECT_SUPPORTS_${sanitizer_identifier}")
    check_cxx_source_compiles("int main() { return 0; }" ${test_variable})

    set(CMAKE_REQUIRED_FLAGS "${previous_required_flags}")
    set(CMAKE_REQUIRED_LINK_OPTIONS "${previous_required_link_options}")
    set(CMAKE_REQUIRED_QUIET "${previous_required_quiet}")

    set(${output_variable}
        "${${test_variable}}"
        PARENT_SCOPE)
endfunction()

##
# @brief Collect the enabled sanitizer flags from the current CMake options.
#
# @param output_variable Parent-scope list variable that receives sanitizer flags.
##
function(_project_get_enabled_sanitizer_flags output_variable)
    set(enabled_sanitizer_flags)

    if(ENABLE_ASAN)
        list(APPEND enabled_sanitizer_flags -fsanitize=address)
    endif()

    if(ENABLE_LSAN)
        list(APPEND enabled_sanitizer_flags -fsanitize=leak)
    endif()

    if(ENABLE_UBSAN)
        list(APPEND enabled_sanitizer_flags -fsanitize=undefined)
    endif()

    if(ENABLE_TSAN)
        list(APPEND enabled_sanitizer_flags -fsanitize=thread)
    endif()

    if(ENABLE_MSAN)
        list(APPEND enabled_sanitizer_flags -fsanitize=memory)
    endif()

    set(${output_variable}
        "${enabled_sanitizer_flags}"
        PARENT_SCOPE)
endfunction()

##
# @brief Validate the active sanitizer option set before target generation.
#
# The validation is intentionally eager so unsupported runtimes fail during
# configure instead of surfacing as confusing link failures later.
#
# @return None. Emits fatal errors for unsupported or incompatible requests.
##
function(validate_project_sanitizer_options)
    _project_get_enabled_sanitizer_flags(enabled_sanitizer_flags)

    if(NOT enabled_sanitizer_flags)
        return()
    endif()

    if(NOT
       CMAKE_SYSTEM_NAME
       STREQUAL
       "Linux")
        message(FATAL_ERROR "Sanitizer probing in this repository currently supports Linux only.")
    endif()

    if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        message(FATAL_ERROR "Sanitizers are only supported with GCC or Clang in this repository.")
    endif()

    if(ENABLE_TSAN
       AND (ENABLE_ASAN
            OR ENABLE_LSAN
            OR ENABLE_MSAN))
        message(
            FATAL_ERROR "ThreadSanitizer cannot be combined with AddressSanitizer, LeakSanitizer, or MemorySanitizer.")
    endif()

    if(ENABLE_MSAN
       AND (ENABLE_ASAN
            OR ENABLE_LSAN
            OR ENABLE_TSAN))
        message(
            FATAL_ERROR "MemorySanitizer cannot be combined with AddressSanitizer, LeakSanitizer, or ThreadSanitizer.")
    endif()

    if(ENABLE_MSAN
       AND NOT
           CMAKE_CXX_COMPILER_ID
           MATCHES
           "Clang")
        message(FATAL_ERROR "ENABLE_MSAN is ON, but MemorySanitizer requires Clang on Linux.")
    endif()

    if(ENABLE_ASAN)
        _project_check_sanitizer_support(-fsanitize=address project_supports_asan)
        if(NOT project_supports_asan)
            message(
                FATAL_ERROR
                    "ENABLE_ASAN is ON, but AddressSanitizer is not supported by the active compiler/runtime on this machine."
            )
        endif()
    endif()

    if(ENABLE_LSAN)
        _project_check_sanitizer_support(-fsanitize=leak project_supports_lsan)
        if(NOT project_supports_lsan)
            message(
                FATAL_ERROR
                    "ENABLE_LSAN is ON, but LeakSanitizer is not supported by the active compiler/runtime on this machine."
            )
        endif()
    endif()

    if(ENABLE_UBSAN)
        _project_check_sanitizer_support(-fsanitize=undefined project_supports_ubsan)
        if(NOT project_supports_ubsan)
            message(
                FATAL_ERROR
                    "ENABLE_UBSAN is ON, but UndefinedBehaviorSanitizer is not supported by the active compiler/runtime on this machine."
            )
        endif()
    endif()

    if(ENABLE_TSAN)
        _project_check_sanitizer_support(-fsanitize=thread project_supports_tsan)
        if(NOT project_supports_tsan)
            message(
                FATAL_ERROR
                    "ENABLE_TSAN is ON, but ThreadSanitizer is not supported by the active compiler/runtime on this machine."
            )
        endif()
    endif()

    if(ENABLE_MSAN)
        _project_check_sanitizer_support(-fsanitize=memory project_supports_msan)
        if(NOT project_supports_msan)
            message(
                FATAL_ERROR
                    "ENABLE_MSAN is ON, but MemorySanitizer is not supported by the active compiler/runtime on this machine."
            )
        endif()
    endif()
endfunction()

##
# @brief Apply the enabled sanitizer flags to a concrete target.
#
# Sanitizers stay opt-in and Debug-only so the default developer build remains
# usable even on machines without the sanitizer runtimes installed.
#
# @param target_name Concrete target that should receive sanitizer flags.
##
function(enable_project_sanitizers target_name)
    _project_get_enabled_sanitizer_flags(enabled_sanitizer_flags)

    if(NOT enabled_sanitizer_flags)
        return()
    endif()

    target_compile_options(${target_name} PRIVATE "$<$<CONFIG:Debug>:${enabled_sanitizer_flags}>")

    target_link_options(${target_name} PRIVATE "$<$<CONFIG:Debug>:${enabled_sanitizer_flags}>")
endfunction()
