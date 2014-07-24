# Find the ISI API 1.0 libary
# This module defines
#

SET(IntuitiveDaVinci_SDK_PATH_HINTS 
  ../IntuitiveDaVinci
  ../PLTools/IntuitiveDaVinci
  ../../PLTools/IntuitiveDaVinci
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/IntuitiveDaVinci
)

FIND_PATH(IntuitiveDaVinci_INCLUDE_DIR
  NAMES "isi_api.h" "isi_api_math.h" "isi_api_types.h"
  PATH_SUFFIXES inc
  PATHS ${IntuitiveDaVinci_SDK_PATH_HINTS}
  DOC "Include directory, i.e parent directory of the ISI API"
)

FIND_LIBRARY(IntuitiveDaVinci_LIBRARY
  NAMES isi_api.lib
  PATHS ${IntuitiveDaVinci_SDK_PATH_HINTS}
  DOC "Library file for ISI API"
)

# handle the QUIETLY and REQUIRED arguments and set ISIAPI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IntuitiveDaVinci DEFAULT_MSG  IntuitiveDaVinci_LIBRARY  IntuitiveDaVinci_INCLUDE_DIR  )

IF(ISIAPI_FOUND)
  SET( IntuitiveDaVinci_LIBRARY ${IntuitiveDaVinci_LIBRARY} )
  SET( IntuitiveDaVinci_INCLUDE_DIR ${IntuitiveDaVinci_INCLUDE_DIR} )
ENDIF()
