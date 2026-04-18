include_guard(GLOBAL)

##
# @brief Apply repository build-mode defaults to a concrete target.
#
# These are kept separate from warning policy so optimization/debug tuning is
# not mixed into the warning helper.
#
# @param target_name Concrete target that owns compiled translation units.
##
function(enable_project_build_modes target_name)
    target_compile_options(${target_name} PRIVATE "$<$<CONFIG:Debug>:-Og;-g>" "$<$<CONFIG:Release>:-O3>")
endfunction()
