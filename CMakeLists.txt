# Author: petter.strandmark@gmail.com (Petter Strandmark)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)
PROJECT(MCTS C CXX)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Default locations to search for on various platforms.
LIST(APPEND SEARCH_LIBS /usr/lib)
LIST(APPEND SEARCH_LIBS /usr/local/lib)
LIST(APPEND SEARCH_LIBS /usr/local/homebrew/lib) # Mac OS X
LIST(APPEND SEARCH_LIBS /opt/local/lib)

LIST(APPEND SEARCH_HEADERS /usr/include)
LIST(APPEND SEARCH_HEADERS /usr/local/include)
LIST(APPEND SEARCH_HEADERS /usr/local/homebrew/include) # Mac OS X
LIST(APPEND SEARCH_HEADERS /opt/local/include)

ENABLE_TESTING()

# Change the default build type from Debug to Release, while still
# supporting overriding the build type.
#
# The CACHE STRING logic here and elsewhere is needed to force CMake
# to pay attention to the value of these variables.
IF (NOT CMAKE_BUILD_TYPE)
  MESSAGE("-- No build type specified; defaulting to CMAKE_BUILD_TYPE=Release.")
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

# C++11 support.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
	check_cxx_compiler_flag(--std=c++0x SUPPORTS_STD_CXX0X)
	if(SUPPORTS_STD_CXX11)
		message("-- Enabling C++11 support with --std=c++11.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
	elseif(SUPPORTS_STD_CXX0X)
		message("-- Enabling C++11 support with --std=c++0x.")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
	else()
		message(ERROR "Compiler does not support C++11.")
	endif()
endif()

SET (MY_LIBRARY_DEPENDENCIES)


#
# gcc settings.
#
IF (CMAKE_COMPILER_IS_GNUCXX)
  # No warnings for C. The Meschach library contains really old code.
  # -fPIC for building a shared library.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fpic")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

#
# Clang settings
#
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fpic")
ENDIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

#
# MSVC settings
#
IF (MSVC)
  # Disable deprecation warning for standard functions.
  ADD_DEFINITIONS("/wd4996")
  # To get rid of annoying min and max macros if windows.h
  # is included.
  ADD_DEFINITIONS("-DNOMINMAX=1")
  ADD_DEFINITIONS("-D_VARIADIC_MAX=6")
ENDIF (MSVC)

#
# Include directories
#
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(third-party/Catch)
IF (${USE_CINDER})
	MESSAGE("-- Will be using Cinder.")
	INCLUDE_DIRECTORIES(${CINDER_INCLUDE})
ENDIF()
FILE(GLOB MCTS_HEADERS ${CMAKE_SOURCE_DIR}/*.h)

ADD_SUBDIRECTORY(games)
#ADD_SUBDIRECTORY(tests)
