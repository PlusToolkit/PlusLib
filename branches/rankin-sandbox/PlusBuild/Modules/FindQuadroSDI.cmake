# - Find Nvidia Quadro SDI SDK
# Find the QuadroSDI headers and libraries. 
#
#  QuadroSDI_INCLUDE_DIRS - where to find nvapi.h
#  QuadroSDI_LIBRARY_DIR  - List of libraries when using nvapi.
#  QuadroSDI_FOUND        - True if nvapi found.
#  QuadroSDI              - CMake target containing necessary properties
#
# Original Author:
# 2016 Russ Taylor working through ReliaSolve for Sensics.
#
# Modification by:
# 2016/08/16 Adam Rankin, Robarts Research Institute
#
# Copyright Sensics, Inc. 2016.
# Distributed under the Boost Software License, Version 1.0.
# (See http://www.boost.org/LICENSE_1_0.txt)

# Look for the header file.
FIND_PATH(QuadroSDI_INCLUDE_DIR NAMES nvapi.h PATH_SUFFIXES include win/include PATHS "${QuadroSDI_ROOT_DIR}" 
  HINTS "C:/Program Files (x86)/NVIDIA Corporation/NVIDIA Quadro SDI Video SDK for OpenGL/" # Include default install path as a hint
  )

IF(QuadroSDI_INCLUDE_DIR)
  # Prioritize finding the lib near the header to hopefully ensure they match.
  GET_FILENAME_COMPONENT(_ROOT_DIR ${QuadroSDI_INCLUDE_DIR} DIRECTORY)

  IF( (MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64") OR NOT MSVC)
    FIND_PATH(nvapi_LIBRARY NAMES nvapi64.lib PATH_SUFFIXES lib64 PATHS "${_ROOT_DIR}")
  ELSE()
    FIND_PATH(nvapi_LIBRARY NAMES nvapi.lib PATH_SUFFIXES lib32 PATHS "${_ROOT_DIR}")
  ENDIF()

  IF(nvapi_LIBRARY)
    SET(QuadroSDI_LIBRARY_DIR ${nvapi_LIBRARY} CACHE PATH "Library directory for Quadro SDI SDK")
    unset(nvapi_LIBRARY CACHE)
  ENDIF()
ENDIF()

# Handle the QUIETLY and REQUIRED arguments and set QuadroSDI_FOUND to TRUE IF
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QuadroSDI REQUIRED_VARS QuadroSDI_LIBRARY_DIR QuadroSDI_INCLUDE_DIR HANDLE_COMPONENTS)

IF(QuadroSDI_FOUND)
  IF(NOT QuadroSDI_ROOT_DIR)
    GET_FILENAME_COMPONENT(QuadroSDI_ROOT_DIR ${QuadroSDI_INCLUDE_DIR} DIRECTORY)
  ENDIF()

  MESSAGE(STATUS "Using Quadro SDI SDK: ${QuadroSDI_ROOT_DIR}.")
  SET(QuadroSDI_INCLUDE_DIRS ${QuadroSDI_INCLUDE_DIR} CACHE PATHS "Include directories for Quadro SDI SDK")
  unset(QuadroSDI_INCLUDE_DIR CACHE)

  SET(QuadroSDI_ROOT_DIR "${QuadroSDI_ROOT_DIR}" CACHE PATH "Path to search for Quadro SDI SDK.")
  MARK_AS_ADVANCED(QuadroSDI_ROOT_DIR)
  MARK_AS_ADVANCED(QuadroSDI_LIBRARY_DIR QuadroSDI_INCLUDE_DIR QuadroSDI_INCLUDE_DIRS)

  ADD_LIBRARY(QuadroSDI INTERFACE IMPORTED)
  SET_PROPERTY(TARGET QuadroSDI PROPERTY INTERFACE_INCLUDE_DIRECTORIES 
    ${QuadroSDI_INCLUDE_DIRS}
    )
  IF( (MSVC AND ${CMAKE_GENERATOR} MATCHES "Win64") OR NOT MSVC)
    SET_PROPERTY(TARGET QuadroSDI PROPERTY INTERFACE_LINK_LIBRARIES 
      ${QuadroSDI_LIBRARY_DIR}/nvapi64${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/ANCapi${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/NvCpl${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/cutil64${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
  ELSE()
    SET_PROPERTY(TARGET QuadroSDI PROPERTY INTERFACE_LINK_LIBRARIES 
      ${QuadroSDI_LIBRARY_DIR}/nvapi${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/ANCapi${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/NvCpl${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${QuadroSDI_LIBRARY_DIR}/cutil32${CMAKE_STATIC_LIBRARY_SUFFIX}
      )
  ENDIF()

ELSE()
  unset(QuadroSDI_INCLUDE_DIR CACHE)
  unset(QuadroSDI_LIBRARY_DIR CACHE)
  SET(QuadroSDI_ROOT_DIR "QuadroSDI_ROOT_DIR-NOTFOUND" CACHE PATH "Path to search for Quadro SDI SDK.")
ENDIF()