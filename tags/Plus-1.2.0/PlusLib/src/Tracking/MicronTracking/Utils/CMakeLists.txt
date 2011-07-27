
## PROJECT - Set a name for the entire project. One argument. 
## Usage: PROJECT(projectname [C++ C Java]) Sets the name of the project. 
## This creates the variables projectname_BINARY_DIR and 
## projectname_SOURCE_DIR. Optionally you can specify which languages 
## your project supports. By default all languages are supported. 
## If you do not have a C++ compiler, but want to build a c program 
## with cmake, then use this option.

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

SET (PLUS_INCLUDE_DIRS ${PLUS_INCLUDE_DIRS} ${MICRONTRACKERFILES_SOURCE_DIR} ${MICRONTRACKERFILES_BINARY_DIR} CACHE INTERNAL "")

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
MTC.h
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

SET (PLUS_INCLUDE_DIRS ${PLUS_INCLUDE_DIRS} ${MICRONTRACKERFILES_SRCS}/Dist CACHE INTERNAL "")

INCLUDE_DIRECTORIES( ${PLUS_INCLUDE_DIRS} )
ADD_LIBRARY(vtkMicrontrackerfiles ${MICRONTRACKERFILES_SRCS})
TARGET_LINK_LIBRARIES( vtkMicrontrackerfiles)

INSTALL_TARGETS(/lib/vtk vtkMicrontrackerfiles )
#vtkMicrontracker)
