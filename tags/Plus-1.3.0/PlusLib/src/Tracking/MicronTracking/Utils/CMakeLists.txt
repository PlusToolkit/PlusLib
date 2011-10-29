PROJECT(MICRONTRACKERFILES)

## INCLUDE_REGULAR_EXPRESSION - Set the regular expression used for 
## dependency checking. 
## Usage: INCLUDE_REGULAR_EXPRESSION(regex_match [regex_complain]) 
## Set the regular expressions used in dependency checking. Only files 
## matching regex_match will be traced as dependencies. Only files 
## matching regex_complain will generate warnings if they cannot be 
## found (standard header paths are not searched). The defaults are: 
## regex_match = "^.*$" (match everything) 
## regex_complain = "^$" (match empty string only)

INCLUDE_REGULAR_EXPRESSION("^(microntrackerfiles).*$")

SET (MICRONTRACKERFILES_INCLUDE_DIRS 
  ${MICRONTRACKERFILES_SOURCE_DIR} 
  ${MICRONTRACKERFILES_BINARY_DIR} 
  )

SET(MICRONTRACKERFILES_SRCS
  MicronTrackerInterface.h
  MicronTrackerInterface.cpp
  UtilityFunctions.h
  UtilityFunctions.cpp
  Cameras.h
  Cameras.cpp
  MCamera.h
  MCamera.cpp
  Collection.h
  Collection.cpp
  Facet.h
  Facet.cpp
  Marker.h
  Marker.cpp
  Markers.h
  Markers.cpp
  Persistence.h
  Persistence.cpp
  Vector.h
  Vector.cpp
  Xform3D.h
  Xform3D.cpp
)

IF(WIN32)
 SET( MICRONTRACKERFILES_SRCS ${MICRONTRACKERFILES_SRCS} 
      drive32.c )
ENDIF(WIN32)

IF(UNIX)
 SET( MICRONTRACKERFILES_SRCS ${MICRONTRACKERFILES_SRCS} 
      driveunx-merge.c )
ENDIF(UNIX)

## VC++ .lib file linking
IF (WIN32)
  IF (NOT BORLAND)
    IF (NOT CYGWIN)
      SET(MICRONTRACKERFILES_SRCS ${MICRONTRACKERFILES_SRCS} vtkMicrontrackerfiles.def)
    ENDIF (NOT CYGWIN)
  ENDIF (NOT BORLAND)
ENDIF (WIN32)

SET (MICRONTRACKERFILES_INCLUDE_DIRS 
  ${MICRONTRACKERFILES_INCLUDE_DIRS} 
  ${MTC_INCLUDE_DIR}
  )

INCLUDE_DIRECTORIES( ${MICRONTRACKERFILES_INCLUDE_DIRS} )
SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} ${MICRONTRACKERFILES_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_LIBRARY(vtkMicrontrackerfiles ${MICRONTRACKERFILES_SRCS})
TARGET_LINK_LIBRARIES( vtkMicrontrackerfiles
  ${MTC_LIB_DIR}/MTC${CMAKE_STATIC_LIBRARY_SUFFIX}
)

#INSTALL_TARGETS(/lib/vtk vtkMicrontrackerfiles )
#vtkMicrontracker)


# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkMicrontrackerfiles CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_Microntrackerfiles vtkMicrontrackerfiles "${PlusLib_BINARY_DIR}/src/Tracking/MicronTracking/Utils/vtkMicrontrackerfiles.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE) 
