set( CMAKE_VERSION 3.25)

cmake_minimum_required(VERSION ${CMAKE_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(DEFINED ENV{PROJECT_NAME})
    set(PROJECT_NAME $ENV{PROJECT_NAME})
else()
    set(PROJECT_NAME "threading_library")
endif()

set( PROJECT_VERSION 0.1 )

project(
  ${PROJECT_NAME}  
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 23)


if( NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()


message(STATUS "Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...\n")


## conan basic setup
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message( STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan...")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Cmake-Conan downloaded succesfully.")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# interface means a header only library
add_library( ${PROJECT_NAME}-lib INTERFACE )

# ADD INCLUDE FOLDERS
target_include_directories( ${PROJECT_NAME}-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )

# ADD EXECUTABLE
add_executable( ${PROJECT_NAME} 
  src/main.cpp
  )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-lib )

set(GCC_WARNINGS
  -Wall
  -Wextra 
  -Wshadow
  -Wnon-virtual-dtor
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Wdouble-promotion
  -Wformat=2

  #-fsanitize=undefined,thread
  -fsanitize=undefined,address

  #compile with gdb debug info
  #-ggdb3
  -g3
  )

add_compile_options( ${GCC_WARNINGS} )

link_libraries("-fsanitize=address")
#link_libraries("-fsanitize=thread")

# catch2 set up
# option(BUILD_TESTS "Enables building the Catch2 unit tests" ON)

#if(BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)
 #endif()

