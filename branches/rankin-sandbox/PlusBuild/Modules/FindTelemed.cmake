# Find the Telemed SDK
# This module defines
# TELEMED_FOUND - MicronTracker SDK has been found on this system
# TELEMED_INCLUDE_DIR - where to find the header files


# Find Telemed SDK include directory

SET( Telemed_PATH_HINTS
  ../PLTools/Telemed/USGFW2-3.25
  ../../PLTools/Telemed/USGFW2-3.25
  ../PLTools/trunk/Telemed/USGFW2-3.25
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Telemed/USGFW2-3.25
  "C:/Program Files/Telemed SDK"
  "C:/Telemed SDK"
  "C:/Users/$ENV{USERNAME}/Documents"
  )

FIND_PATH( Telemed_INCLUDE_DIR usgfw.h
  PATH_SUFFIXES
    "include/USGFWSDK/include"
  DOC "Telemed include directory (contains usgfw.h)"
  PATHS ${Telemed_PATH_HINTS}
  )
IF(NOT Telemed_INCLUDE_DIR)
  MESSAGE( SEND_ERROR "Please specify the Telemed SDK include directory in the variable Telemed_INCLUDE_DIR, or turn off PLUS_USE_TELEMED_VIDEO. \nFYI, this directory should contain among others files : usgfw.h and multfreq.h")
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set Telemed_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Telemed DEFAULT_MSG
  Telemed_INCLUDE_DIR
  )

IF(TELEMED_FOUND)
  SET( TELEMED_INCLUDE_DIR ${Telemed_INCLUDE_DIR} )
ENDIF()
