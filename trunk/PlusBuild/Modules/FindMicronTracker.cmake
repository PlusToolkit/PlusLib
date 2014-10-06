# Find the Claron MicronTracker SDK 
# This module defines
# MICRONTRACKER_FOUND - MicronTracker SDK has been found on this system
# MICRONTRACKER_INCLUDE_DIR - where to find the header files
# MICRONTRACKER_LIBRARY - libraries to be linked
# MICRONTRACKER_BINARY_DIR - shared libraries to be installed

OPTION(PLUSBUILD_PREFER_MICRONTRACKER_36 "Plus prefers MicronTracker SDK version MTC_3.6 instead of MTC_3.7" OFF)
MARK_AS_ADVANCED(PLUSBUILD_PREFER_MICRONTRACKER_36)

# If PLUSBUILD_PREFER_MICRONTRACKER_36 is defined: try to find MTC_3.6 first and fall back to MTC_3.7,
# otherwise try to find MTC_3.7 and fall back to MTC_3.6.
IF (PLUSBUILD_PREFER_MICRONTRACKER_36)
  SET( MicronTracker_PATH_HINTS
    ../Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker  
    ../Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    )
ELSE()
  SET( MicronTracker_PATH_HINTS
    ../Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.7.6.8_x86_win/MicronTracker
    ../Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.6.5.4_x86_win/MicronTracker  
    ../Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../../PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ../trunk/PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
    )
ENDIF()

SET( MicronTracker_PATH_HINTS ${MicronTracker_PATH_HINTS}
  "c:/Program Files (x86)/Claron Technology/MicronTracker"
  "c:/Program Files/Claron Technology/MicronTracker"
  )

FIND_PATH(MicronTracker_BASE_INCLUDE_DIR MTC.h 
  PATH_SUFFIXES
    Dist
    inc	
  DOC "MicronTracker include directory (contains MTC.h)"
  PATHS ${MicronTracker_PATH_HINTS} 
  )

FIND_LIBRARY(MicronTracker_BASE_LIBRARY 
  NAMES MTC${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES
    Dist
    lib
  DOC "Path to MicronTracker base library (MTC.lib)"
  PATHS ${MicronTracker_PATH_HINTS} 
  )

FIND_PATH(MicronTracker_BASE_BINARY_DIR MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    Dist
    bin
  PATHS ${MicronTracker_PATH_HINTS} 
  DOC "Path to MicronTracker base shared library (MTC.dll)"
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )

# handle the QUIETLY and REQUIRED arguments and set MicronTracker_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MicronTracker DEFAULT_MSG 
  MicronTracker_BASE_LIBRARY
  MicronTracker_BASE_INCLUDE_DIR
  MicronTracker_BASE_BINARY_DIR
  )

IF(MICRONTRACKER_FOUND)
  SET( MICRONTRACKER_LIBRARY ${MicronTracker_BASE_LIBRARY} )
  SET( MICRONTRACKER_INCLUDE_DIR ${MicronTracker_BASE_INCLUDE_DIR} )
  SET( MICRONTRACKER_BINARY_DIR ${MicronTracker_BASE_BINARY_DIR} )
ENDIF()
