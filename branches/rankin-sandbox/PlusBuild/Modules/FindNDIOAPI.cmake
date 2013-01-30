# Find the NDI oapi 3.0.0.66 libraries
# This module defines
# NDIOAPI_LIBRARY - libraries to link to in order to use NDI oapi
# NDIOAPI_FOUND, if false, do not try to link 
# NDIOAPI_BINARY_DIR, where to find the shared libraries 
# NDIOAPI_INCLUDE_DIR, where to find the headers
#

SET( NDIOAPI_PATH_HINTS 
    ../NDI/Oapi-3.0.0.66
    ../PLTools/NDI/Oapi-3.0.0.66
    ../../PLTools/NDI/Oapi-3.0.0.66
    ../trunk/PLTools/NDI/Oapi-3.0.0.66    
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/NDI/Oapi-3.0.0.66
    )

FIND_PATH(NDIOAPI_INCLUDE_DIR ndopto.h 
  PATH_SUFFIXES inc
  PATHS ${NDIOAPI_PATH_HINTS} 
  )

FIND_LIBRARY(NDIOAPI_LIBRARY 
  NAMES oapi
  PATH_SUFFIXES lib
  PATHS ${NDIOAPI_PATH_HINTS} 
  )

FIND_PATH(NDIOAPI_BINARY_DIR oapi${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES bin
  PATHS ${NDIOAPI_PATH_HINTS} 
  )

# handle the QUIETLY and REQUIRED arguments and set NDIOAPI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NDIOAPI  DEFAULT_MSG  NDIOAPI_LIBRARY  NDIOAPI_INCLUDE_DIR  NDIOAPI_BINARY_DIR)

IF(NDIOAPI_FOUND)
  SET( NDIOAPI_LIBRARIES ${NDIOAPI_LIBRARY} )
  SET( NDIOAPI_INCLUDE_DIRS ${NDIOAPI_INCLUDE_DIR} )
  SET( NDIOAPI_BINARY_DIRS ${NDIOAPI_BINARY_DIR} )
ENDIF()
