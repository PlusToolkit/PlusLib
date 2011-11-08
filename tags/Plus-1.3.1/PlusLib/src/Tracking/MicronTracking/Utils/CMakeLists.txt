PROJECT(MicronTrackerInterface)

SET (MICRONTRACKERFILES_INCLUDE_DIRS 
  ${MicronTrackerInterface_SOURCE_DIR} 
  ${MicronTrackerInterface_BINARY_DIR} 
  )

SET(MicronTrackerInterface_SRCS
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
 SET( MicronTrackerInterface_SRCS ${MicronTrackerInterface_SRCS} 
      drive32.c )
ENDIF(WIN32)

IF(UNIX)
 SET( MicronTrackerInterface_SRCS ${MicronTrackerInterface_SRCS} 
      driveunx-merge.c )
ENDIF(UNIX)

SET (MicronTrackerInterface_INCLUDE_DIRS 
  ${MicronTrackerInterface_INCLUDE_DIRS} 
  ${MTC_INCLUDE_DIR}
  )

INCLUDE_DIRECTORIES( ${MicronTrackerInterface_INCLUDE_DIRS} )
SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} ${MicronTrackerInterface_INCLUDE_DIRS} CACHE INTERNAL "")

# Create a static library (to not have too many DLLs and also because
# the driver library interface havily uses STL containers, which are unsafe
# to use at a DLL interface)
ADD_LIBRARY(MicronTrackerInterface STATIC ${MicronTrackerInterface_SRCS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MicronTrackerInterface CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MicronTrackerInterface MicronTrackerInterface "${PlusLib_BINARY_DIR}/src/Tracking/MicronTracking/Utils/MicronTrackerInterface.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE) 
