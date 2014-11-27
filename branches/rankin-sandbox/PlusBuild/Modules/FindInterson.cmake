# Find the Interson iSDK
# This module defines
# INTERSON_FOUND - MicronTracker SDK has been found on this system
# INTERSON_INCLUDE_DIR - where to find the header files
# INTERSON_LIBRARY_DIR - libraries to be linked
# INTERSON_BINARY_DIR - common shared libraries to be installed
# INTERSON_WIN32_BINARY_DIR - 32-bit shared libraries to be installed
# INTERSON_WIN64_BINARY_DIR - 64-bit shared libraries to be installed

SET( Interson_PATH_HINTS 
  ../PLTools/Interson/iSDK2012_4.83.4363
  ../../PLTools/Interson/iSDK2012_4.83.4363
  ../trunk/PLTools/Interson/iSDK2012_4.83.4363
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Interson/iSDK2012_4.83.4363
  "c:/iSDK2012"
  )

FIND_PATH(Interson_INCLUDE_DIR usbProbeDLL_net.h
  PATH_SUFFIXES
    "include"
  DOC "Interson include directory (contains usbProbeDLL_net.h)"
  PATHS ${Interson_PATH_HINTS} 
  )

FIND_PATH(Interson_LIBRARY_DIR USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES
    "lib"
  DOC "Interson library directory (contains USBprobeDLL.lib)"
  PATHS ${Interson_PATH_HINTS} 
  )
  
FIND_PATH(Interson_BINARY_DIR BmodeUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    "lib"
  DOC "Path to Interson common binary directory (contains BmodeUSB.dll)"
  PATHS ${Interson_PATH_HINTS}
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )
  
FIND_PATH(Interson_WIN32_BINARY_DIR USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    "lib/32bit"
  DOC "Path to Interson 32-bit binary directory (contains 32-bit USBprobe.dll)"
  PATHS ${Interson_PATH_HINTS}
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )

  FIND_PATH(Interson_WIN64_BINARY_DIR USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    "lib/64bit"
  DOC "Path to Interson 64-bit binary directory (contains 64-bit USBprobe.dll)"
  PATHS ${Interson_PATH_HINTS}
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )  

# handle the QUIETLY and REQUIRED arguments and set Interson_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Interson DEFAULT_MSG 
  Interson_LIBRARY_DIR
  Interson_INCLUDE_DIR
  Interson_BINARY_DIR
  Interson_WIN32_BINARY_DIR
  Interson_WIN64_BINARY_DIR
  )

IF(INTERSON_FOUND)
  SET( INTERSON_LIBRARY_DIR ${Interson_LIBRARY_DIR} )
  SET( INTERSON_INCLUDE_DIR ${Interson_INCLUDE_DIR} )
  SET( INTERSON_BINARY_DIR ${Interson_BINARY_DIR} )
  SET( INTERSON_WIN32_BINARY_DIR ${Interson_WIN32_BINARY_DIR} )
  SET( INTERSON_WIN64_BINARY_DIR ${Interson_WIN64_BINARY_DIR} )
ENDIF()
