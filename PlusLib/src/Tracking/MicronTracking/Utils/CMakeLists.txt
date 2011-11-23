PROJECT(MicronTrackerInterface)

SET (MicronTrackerInterface_INCLUDE_DIRS 
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

SET (MicronTrackerInterface_INCLUDE_DIRS 
  ${MicronTrackerInterface_INCLUDE_DIRS} 
  ${MICRONTRACKER_INCLUDE_DIR}
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
