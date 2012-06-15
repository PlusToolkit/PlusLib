# Find the Dalsa Sapera SDK 
# This module defines
# DALSASAPERA_DIR - path to the SDK root
# DALSASAPERA_LIB_DIR - path to the SDK root
# DALSASAPERA_FOUND - true if the SDK is found

SET( DALSASAPERA_DIR_HINTS 
  ../Dalsa/Sapera-7.20.00.1119
  ../PLTools/Dalsa/Sapera-7.20.00.1119
  ../../PLTools/Dalsa/Sapera-7.20.00.1119
  ../PLTools/trunk/Dalsa/Sapera-7.20.00.1119
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Dalsa/Sapera-7.20.00.1119
  )

SET( PLATFORM_SUFFIX "Win32")
IF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )
  SET( PLATFORM_SUFFIX "Win64")
ENDIF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )

FIND_PATH(DALSASAPERA_DIR Include/SapVersion.h 
  PATHS ${DALSASAPERA_DIR_HINTS} 
  )

FIND_PATH(DALSASAPERA_LIB_DIR corapi.lib 
  PATH_SUFFIXES 
    Lib/${PLATFORM_SUFFIX}
    Lib
  PATHS ${DALSASAPERA_DIR_HINTS} 
  )

# handle the QUIETLY and REQUIRED arguments and set DALSASAPERA_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DALSASAPERA  DEFAULT_MSG  DALSASAPERA_DIR DALSASAPERA_LIB_DIR )
