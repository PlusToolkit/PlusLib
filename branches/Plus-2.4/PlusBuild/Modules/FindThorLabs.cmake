# Find the ThorLabs CCS (Compact Spectrometer) VISA SDK
# This module defines
# THORLABS_FOUND - ThorLabs SDK has been found on this system
# THORLABS_INCLUDE_DIR - where to find the header files
# ThorLabs_LIBRARY_DIR - where to find the library files

SET( ThorLabs_PATH_HINTS
  ../PLTools/ThorLabs/VISA
  ../../PLTools/ThorLabs/VISA
  ../trunk/PLTools/ThorLabs/VISA
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/ThorLabs/VISA
  "C:/Program Files (x86)/IVI Foundation/VISA"
  "C:/Program Files/IVI Foundation/VISA"
  )

FIND_PATH( ThorLabs_INCLUDE_DIR visa.h
  PATH_SUFFIXES
    "WinNT/include"
  DOC "ThorLabs CCS VISA include directory (contains visa.h)"
  PATHS ${ThorLabs_PATH_HINTS}
  )
IF(NOT ThorLabs_INCLUDE_DIR)
  MESSAGE( SEND_ERROR "Please specify the ThorLabs CCS VISA include directory in the variable ThorLabs_INCLUDE_DIR, or turn off PLUS_USE_THORLABS_VIDEO (this include directory contains visa.h).")
ENDIF()

FIND_PATH(ThorLabs_LIBRARY_DIR TLCCS_32${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES "WinNT/lib/msc"
  DOC "ThorLabs CCS VISA library directory (contains TLCCS_32.lib)"
  PATHS ${ThorLabs_PATH_HINTS} 
  )
IF(NOT ThorLabs_LIBRARY_DIR)
  MESSAGE( SEND_ERROR "Please specify the ThorLabs CCS VISA include directory in the variable ThorLabs_LIBRARY_DIR, or turn off PLUS_USE_THORLABS_VIDEO (this include directory contains TLCCS_32.lib).")
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set ThorLabs_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ThorLabs DEFAULT_MSG
  ThorLabs_INCLUDE_DIR
  ThorLabs_LIBRARY_DIR
  )

IF(THORLABS_FOUND)  
  SET( THORLABS_INCLUDE_DIR ${ThorLabs_INCLUDE_DIR} )
  SET( THORLABS_LIBRARY_DIR ${ThorLabs_LIBRARY_DIR} )
ENDIF()
