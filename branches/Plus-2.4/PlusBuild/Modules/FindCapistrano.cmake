# Find the Capistrano iSDK
# This module defines
# CAPISTRANO_FOUND - Capistrano SDK has been found on this system
# CAPISTRANO_INCLUDE_DIR - where to find the header files
# CAPISTRANO_LIBRARY_DIR - libraries to be linked
# CAPISTRANO_BINARY_DIR - common shared libraries to be installed
# CAPISTRANO_WIN32_BINARY_DIR - 32-bit shared libraries to be installed
# CAPISTRANO_WIN64_BINARY_DIR - 64-bit shared libraries to be installed

SET( Capistrano_PATH_HINTS 
  "c:/cSDK2013"
  )

FIND_PATH(Capistrano_INCLUDE_DIR usbprobedll_net.h
  PATH_SUFFIXES
    "include"
  DOC "Capistrano include directory (contains usbProbeDLL_net.h)"
  PATHS ${Capistrano_PATH_HINTS} 
  )

FIND_PATH(Capistrano_WIN32_LIBRARY_DIR USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES
    "lib"
  DOC "Capistrano 32-bit library directory (contains USBprobeDLL.lib)"
  PATHS ${Capistrano_PATH_HINTS} 
  )
  
FIND_PATH(Capistrano_WIN64_LIBRARY_DIR USBprobeDLL${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES
    "lib/x64"
  DOC "Capistrano 64-bit library directory (contains USBprobeDLL.lib)"
  PATHS ${Capistrano_PATH_HINTS} 
  )
  
FIND_PATH(Capistrano_WIN32_BINARY_DIR USBprobe32${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    "bin"
  DOC "Path to Capistrano 32-bit binary directory (contains 32-bit USBprobe.dll)"
  PATHS ${Capistrano_PATH_HINTS}
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )

FIND_PATH(Capistrano_WIN64_BINARY_DIR USBprobe${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES 
    "bin/x64"
  DOC "Path to Capistrano 64-bit binary directory (contains 64-bit USBprobe.dll)"
  PATHS ${Capistrano_PATH_HINTS}
  NO_DEFAULT_PATH # avoid finding installed DLLs in the system folders
  )  

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( Capistrano_BINARY_DIR ${Capistrano_WIN64_BINARY_DIR} )
  set( Capistrano_LIBRARY_DIR ${Capistrano_WIN64_LIBRARY_DIR} )  
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( Capistrano_BINARY_DIR ${Capistrano_WIN32_BINARY_DIR} )
  set( Capistrano_LIBRARY_DIR ${Capistrano_WIN32_LIBRARY_DIR} )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  
# handle the QUIETLY and REQUIRED arguments and set Capistrano_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Capistrano DEFAULT_MSG 
  Capistrano_LIBRARY_DIR
  Capistrano_INCLUDE_DIR
  Capistrano_BINARY_DIR
  )

IF(CAPISTRANO_FOUND)
  SET( CAPISTRANO_LIBRARY_DIR ${Capistrano_LIBRARY_DIR} )
  SET( CAPISTRANO_INCLUDE_DIR ${Capistrano_INCLUDE_DIR} )
  SET( CAPISTRANO_BINARY_DIR ${Capistrano_BINARY_DIR} )
ENDIF()
