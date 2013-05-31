PROJECT(vtkMicronTrackerInterface)

SET (vtkMicronTrackerInterface_INCLUDE_DIRS 
  ${vtkMicronTrackerInterface_SOURCE_DIR} 
  ${vtkMicronTrackerInterface_BINARY_DIR} 
  )

SET(vtkMicronTrackerInterface_SRCS
  MicronTrackerInterface.h
  MicronTrackerInterface.cpp
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

SET (vtkMicronTrackerInterface_INCLUDE_DIRS 
  ${vtkMicronTrackerInterface_INCLUDE_DIRS} 
  ${MICRONTRACKER_INCLUDE_DIR}
  )

INCLUDE_DIRECTORIES( ${vtkMicronTrackerInterface_INCLUDE_DIRS} )
SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${vtkMicronTrackerInterface_INCLUDE_DIRS} CACHE INTERNAL "")

# Create a static library (to not have too many DLLs and also because
# the driver library interface havily uses STL containers, which are unsafe
# to use at a DLL interface)
ADD_LIBRARY(vtkMicronTrackerInterface STATIC ${vtkMicronTrackerInterface_SRCS})

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkMicronTrackerInterface CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkMicronTrackerInterface vtkMicronTrackerInterface;"${PlusLib_BINARY_DIR}/src/Utilities/MicronTrackerInterface/vtkMicronTrackerInterface.vcproj" CACHE INTERNAL "" FORCE) 
