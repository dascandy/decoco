cmake_minimum_required(VERSION 3.5)
project(decoco VERSION 0.1.0 LANGUAGES CXX)

#Make sure that custom modules like FindRapidJSON are found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#################################################################################
# Project Settings
#################################################################################

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Put the libaries and binaries that get built into directories at the
    # top of the build tree rather than in hard-to-find leaf
    # directories. This simplifies manual testing and the use of the build
    # tree rather than installed Boost libraries.
    set(MAINFOLDER ${PROJECT_SOURCE_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    # Windows DLLs are "runtime" for CMake. 
    # Output them to "bin" like the Visual Studio projects do.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Must use GNUInstallDirs to install libraries into correct
    # locations on all platforms.
    include(GNUInstallDirs)
    # 
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    # Always use '-fPIC'/'-fPIE' option.
    set( CMAKE_POSITION_INDEPENDENT_CODE ON )
    set( CMAKE_CXX_STANDARD_REQUIRED ON)

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)
    set(CMAKE_VERBOSE_MAKEFILE ON)

    # 
    if (APPLE)
        set(CMAKE_MACOSX_RPATH 1)
        find_package(PkgConfig REQUIRED)
    endif()
    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
	    add_subdirectory(docs)
    else()
	    message(STATUS "Doxygen not found, not building docs")
    endif()
endif()


#################################################################################
# Setup C/C++ compiler options
# Compiler settings - special settings for known compilers
#################################################################################
include(CheckCXXCompilerFlag)
#include(CheckCXXConcepts)
include(CMakePackageConfigHelpers)
include(CheckTypeSize)
include(CMakePrintHelpers)
# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)


#################################################################################
# Source tree
#################################################################################
### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

#FindLibLZMA
find_package(Threads)
find_package(libLZMA REQUIRED)
add_subdirectory(external)
add_subdirectory(src)

#################################################################################
# Test settings
#################################################################################
# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(test)
endif()

#################################################################################
# Packing the stuffs
#################################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)