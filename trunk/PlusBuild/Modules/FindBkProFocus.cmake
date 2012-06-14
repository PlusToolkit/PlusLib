# Find the BK Pro Focues ultrasound scanner SDK 
# This module defines
# BKPROFOCUS_DIR - path to the SDK root
# BKPROFOCUS_FOUND - true if the SDK is found

SET( BKPROFOCUS_DIR_HINTS 
  ../BK/ProFocus
  ../PLTools/BK/ProFocus
  ../../PLTools/BK/ProFocus
  ../PLTools/trunk/BK/ProFocus
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/BK/ProFocus
  )

SET( PLATFORM_SUFFIX "win32")
IF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )
  SET( PLATFORM_SUFFIX "win64")
ENDIF ( CMAKE_HOST_WIN32 AND CMAKE_CL_64 )

	
FIND_PATH(BKPROFOCUS_DIR SourceCode/CMakeLists.txt
  PATH_SUFFIXES 
    Grabbie-1.1.0/${PLATFORM_SUFFIX}
	Grabbie-1.1.0/${PLATFORM_SUFFIX}
  PATHS ${BKPROFOCUS_DIR_HINTS} 
  )

# handle the QUIETLY and REQUIRED arguments and set BKPROFOCUS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BKPROFOCUS  DEFAULT_MSG  BKPROFOCUS_DIR )
