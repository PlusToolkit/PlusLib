# Find the ISI API 1.0 libary
# This module defines
#

SET(DAVINCI_SDK_PATH_HINTS 
  ../daVinci
  ../PLTools/daVinci
  ../../PLTools/daVinci
  ${CMAKE_CURRENT_BINARY_DIR}/PLTools/daVinci
)

FIND_PATH(DAVINCI_INCLUDE_DIR
  NAMES "isi_api.h" "isi_api_math.h" "isi_api_types.h"
  PATH_SUFFIXES inc
  PATHS ${DAVINCI_SDK_PATH_HINTS}
  DOC "Include directory, i.e parent directory of the ISI API"
)

FIND_LIBRARY(DAVINCI_LIBRARY
  NAMES isi_api.lib
  PATHS ${DAVINCI_SDK_PATH_HINTS}
  DOC "Lib directory for ISI API"
)

# handle the QUIETLY and REQUIRED arguments and set ISIAPI_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DAVINCI DEFAULT_MSG  DAVINCI_LIBRARY  DAVINCI_INCLUDE_DIR  )

IF(ISIAPI_FOUND)
  SET( DAVINCI_LIBRARY ${DAVINCI_LIBRARY} )
  SET( DAVINCI_INCLUDE_DIR ${DAVINCI_INCLUDE_DIR} )
ENDIF()
