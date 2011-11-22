# Find the Claron MicronTracker SDK 
# This module defines
# MICRONTRACKER_FOUND - MicronTracker SDK has been found on this system
# MICRONTRACKER_INCLUDE_DIRS - where to find the header files
# MICRONTRACKER_LIBRARIES - libraries to be linked
# MICRONTRACKER_BINARIES - shared libraries to be installed

SET( MicronTracker_PATH_HINTS 
  ../PLTools/Claron/MTC_3.6.1.6_x86_win/MicronTracker
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

FIND_PATH(MicronTracker_WINDIST_BINARY_DIR digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    Dist/Windist
    bin
  PATHS ${MicronTracker_PATH_HINTS} 
  DOC "Path to MicronTracker additional windows shared libraries (digiclops.dll, etc.)"
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )

# handle the QUIETLY and REQUIRED arguments and set MicronTracker_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MicronTracker DEFAULT_MSG 
  MicronTracker_BASE_LIBRARY
  MicronTracker_BASE_INCLUDE_DIR
  MicronTracker_BASE_BINARY_DIR
  MicronTracker_WINDIST_BINARY_DIR
  )

IF(MICRONTRACKER_FOUND)
  SET( MICRONTRACKER_LIBRARIES ${MicronTracker_BASE_LIBRARY} )
  SET( MICRONTRACKER_INCLUDE_DIRS ${MicronTracker_BASE_INCLUDE_DIR} )
  SET( MICRONTRACKER_BINARIES
    ${MicronTracker_BASE_BINARY_DIR}/MTC${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${MicronTracker_WINDIST_BINARY_DIR}/digiclops${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${MicronTracker_WINDIST_BINARY_DIR}/triclops${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
ENDIF()
