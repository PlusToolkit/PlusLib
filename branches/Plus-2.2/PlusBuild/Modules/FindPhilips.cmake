# Find the Philips ie33 1.0.0 libraries
# This module defines
# Philips_FOUND, if false, do not try to link 
# Philips_BINARY_DIR, where to find the shared libraries 
# Philips_INCLUDE_DIR, where to find the headers
#

IF ( NOT "${Philips_VERSION}" STREQUAL "${PLUS_Philips_MAJOR_VERSION}.${PLUS_Philips_MINOR_VERSION}.${PLUS_Philips_PATCH_VERSION}" )
  SET(Philips_VERSION ${PLUS_Philips_MAJOR_VERSION}.${PLUS_Philips_MINOR_VERSION}.${PLUS_Philips_PATCH_VERSION} CACHE INTERNAL "Actual Philips library version used." )
  UNSET(Philips_DIR CACHE)
ENDIF()

SET( Philips_PATH_HINTS 
    ../Philips/ie33-${Philips_VERSION}
    ../PLTools/Philips/ie33-${Philips_VERSION}
    ../../PLTools/Philips/ie33-${Philips_VERSION}
    ../trunk/PLTools/Philips/ie33-${Philips_VERSION} 
    ${CMAKE_CURRENT_BINARY_DIR}/PLTools/Philips/ie33-${Philips_VERSION}
    )

FIND_PATH(Philips_INCLUDE_DIR StreamMgr.h
  PATH_SUFFIXES inc
  PATHS ${Philips_PATH_HINTS} 
  )

FIND_PATH(Philips_LIBRARY_DIR PipelineComDll${CMAKE_STATIC_LIBRARY_SUFFIX}
  PATH_SUFFIXES lib
  PATHS ${Philips_PATH_HINTS} 
  )

FIND_PATH(Philips_BINARY_DIR PipelineComDll${CMAKE_SHARED_LIBRARY_SUFFIX}
  PATH_SUFFIXES bin
  PATHS ${Philips_PATH_HINTS} 
  )

# handle the QUIETLY and REQUIRED arguments and set PHILIPS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Philips  DEFAULT_MSG  Philips_BINARY_DIR  Philips_INCLUDE_DIR Philips_LIBRARY_DIR)

IF( PHILIPS_FOUND )
	SET( Philips_LIBRARY_DIRS ${Philips_LIBRARY_DIR} )
	SET( Philips_INCLUDE_DIRS ${Philips_INCLUDE_DIR} )
    SET( Philips_BINARY_DIRS ${Philips_BINARY_DIR} )
ENDIF()