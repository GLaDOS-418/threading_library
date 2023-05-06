set( CMAKE_VERSION 3.23)

cmake_minimum_required(VERSION ${CMAKE_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set( PROJECT_NAME "threading_library" )
set( PROJECT_VERSION 0.1 )

project(
  ${PROJECT_NAME}  
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 20)


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

conan_add_remote(NAME conancenter 
  URL
  https://center.conan.io
  )


# interface means a header only library
add_library( ${PROJECT_NAME}-lib INTERFACE )

# ADD INCLUDE FOLDERS
target_include_directories( ${PROJECT_NAME}-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )

# ADD EXECUTABLE
add_executable( ${PROJECT_NAME} 
  src/driver.cpp
  )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-lib )

# ADD INTERNAL LIBRARIES
## add_library( ${PROJECT_NAME}-lib <file1.cpp> <file2.cpp> )
## target_include_directories(${PROJECT_NAME}-lib PUBLIC  <file1.h> <file2.h> )
## add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-lib)

# find_package( BENCHMARK REQUIRED)
# target_link_libraries(${PROJECT_NAME} BENCHMARK::BENCHMARK)

set(CLANG_WARNINGS
  -Wall
  -Wextra 
  -Wshadow # warn the user if a variable declaration shadows one from a
  # parent context
  -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
  # non-virtual destructor. This helps catch hard to
  # track down memory errors
  -Wold-style-cast # warn for c-style casts
  -Wcast-align     # warn for potential performance problem casts
  -Wunused         # warn on anything being unused
  -Woverloaded-virtual # warn if you overload (not override) a virtual
  # function
  -Wpedantic   # warn if non-standard C++ is used
  -Wconversion # warn on type conversions that may lose data
  -Wsign-conversion  # warn on sign conversions
  -Wnull-dereference # warn if a null dereference is detected
  -Wdouble-promotion # warn if float is implicit promoted to double
  -Wformat=2 # warn on security issues around functions that format output
  # (ie printf)

  # sanitizers
  #-fsanitize=undefined,thread
  -fsanitize=undefined,address

  #compile with gdb debug info
  -ggdb3
  #-g3
  )

add_compile_options( ${CLANG_WARNINGS} )

link_libraries("-fsanitize=address")
#link_libraries("-fsanitize=thread")

# catch2 set up
# option(BUILD_TESTS "Enables building the Catch2 unit tests" ON)

#if(BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)
   #endif()
