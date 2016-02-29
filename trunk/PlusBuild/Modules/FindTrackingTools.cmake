# Find the OptiTrack Tracking Tools SDK
# http://www.optitrack.com/downloads/tracking-tools.html
# This module defines:
# TRACKINGTOOLS_FOUND
# TRACKINGTOOLS_INCLUDE_DIR
# TRACKINGTOOLS_BINARY_DIR
# TRACKINGTOOLS_64_BIT_LIBRARY
# TRACKINGTOOLS_32_BIT_LIBRARY

SET(TRACKINGTOOLS_PATH_HINTS 
  "C:/Program Files/OptiTrack/Tracking Tools"
  ../OptiTrack/TrackingTools-2.5.3
  ../PLTools/OptiTrack/TrackingTools-2.5.3
  ../../PLTools/OptiTrack/TrackingTools-2.5.3
  ../trunk/PLTools/OptiTrack/TrackingTools-2.5.3
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/OptiTrack/TrackingTools-2.5.3
)

FIND_PATH(TRACKINGTOOLS_INCLUDE_DIR
  NAMES "NPTrackingTools.h"
  PATH_SUFFIXES "inc"
  PATHS ${TRACKINGTOOLS_PATH_HINTS}
  DOC "Include directory of the OptiTrack Tracking Tools SDK"
)

FIND_PATH(TRACKINGTOOLS_BINARY_DIR
  NAMES "NPTrackingToolsx64.dll" "NPTrackingTools.dll"
  PATH_SUFFIXES "lib"
  PATHS ${TRACKINGTOOLS_PATH_HINTS}
  DOC "Directory containing the DLLs of the OptiTrack Tracking Tools SDK"
  NO_DEFAULT_PATH # Avoids finding installed DLLs in the system folders
)

# 64-bit
FIND_LIBRARY(TRACKINGTOOLS_64_BIT_LIBRARY
  NAMES NPTrackingToolsx64.lib
  PATH_SUFFIXES "lib"
  PATHS ${TRACKINGTOOLS_PATH_HINTS}
  DOC "64-bit library file for the OptiTrack Tracking Tools SDK"
)

# 32-bit
FIND_LIBRARY(TRACKINGTOOLS_32_BIT_LIBRARY
  NAMES "NPTrackingTools.lib"
  PATH_SUFFIXES "lib"
  PATHS ${TRACKINGTOOLS_PATH_HINTS}
  DOC "32-bit library file for the OptiTrack Tracking Tools SDK"
)

# handle the QUIETLY and REQUIRED arguments and set TRACKINGTOOLS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TRACKINGTOOLS DEFAULT_MSG TRACKINGTOOLS_INCLUDE_DIR TRACKINGTOOLS_BINARY_DIR TRACKINGTOOLS_64_BIT_LIBRARY TRACKINGTOOLS_32_BIT_LIBRARY)

SET(TRACKINGTOOLS_FOUND ${TRACKINGTOOLS_FOUND}) 
IF(TRACKINGTOOLS_FOUND)
  SET( TRACKINGTOOLS_INCLUDE_DIR ${TRACKINGTOOLS_INCLUDE_DIR} )
  SET( TRACKINGTOOLS_BINARY_DIR ${TRACKINGTOOLS_BINARY_DIR} )
  SET( TRACKINGTOOLS_64_BIT_LIBRARY ${TRACKINGTOOLS_64_BIT_LIBRARY} )
  SET( TRACKINGTOOLS_32_BIT_LIBRARY ${TRACKINGTOOLS_32_BIT_LIBRARY} )
ENDIF()
