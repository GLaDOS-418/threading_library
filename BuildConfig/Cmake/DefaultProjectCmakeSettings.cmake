cmake_minimum_required(VERSION 3.25.0)

include_guard(GLOBAL)

###########################################################################################
# Shared project policy
#
# This file intentionally stays small and readable: it assembles the reusable
# policy modules and applies the global defaults that every target depends on.
###########################################################################################
include("${CMAKE_CURRENT_LIST_DIR}/PreventInSourceBuilds.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ProjectDefaults.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/BuildModes.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/CompilerWarnings.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Hardening.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/OptionalTools.cmake")

prevent_in_source_builds()
apply_default_project_settings()
