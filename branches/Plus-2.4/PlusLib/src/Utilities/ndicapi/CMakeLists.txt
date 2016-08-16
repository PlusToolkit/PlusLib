PROJECT(VTKNDICAPI)
INCLUDE_REGULAR_EXPRESSION("^(ndicapi|polaris).*$")

SET (VTKNDICAPI_INCLUDE_DIRS
  ${VTKNDICAPI_SOURCE_DIR}
  ${VTKNDICAPI_BINARY_DIR}
  )

SET(common_SRCS
  ndicapi.c
  ndicapi_math.c
  ndicapi_serial.c
  ndicapi_thread.c
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(common_HDRS
    ndicapi_math.h
    ndicapi_thread.h
    ndicapi_thread.h
    ndicapi.h
    polaris.h
  )
ENDIF()

IF (WIN32)
  IF (NOT BORLAND)
    IF (NOT CYGWIN)
      SET(common_SRCS ${common_SRCS} vtkndicapi.def)
    ENDIF (NOT CYGWIN)
  ENDIF (NOT BORLAND)
ENDIF (WIN32)

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${VTKNDICAPI_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_LIBRARY(vtkndicapi STATIC ${common_SRCS} ${common_HDRS})
TARGET_LINK_LIBRARIES( vtkndicapi )
IF(NOT WIN32)
  SET_TARGET_PROPERTIES( vtkndicapi PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF()
target_include_directories(vtkndicapi PUBLIC ${VTKNDICAPI_INCLUDE_DIRS} )

# --------------------------------------------------------------------------
# Set external MS project
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkndicapi CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkndicapi vtkndicapi;"${PlusLib_BINARY_DIR}/src/Utilities/ndicapi/vtkndicapi.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib vtkndicapi)
