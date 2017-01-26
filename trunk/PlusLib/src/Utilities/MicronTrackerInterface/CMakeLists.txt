PROJECT(vtkPlusMicronTrackerInterface)

SET (vtkPlusMicronTrackerInterface_INCLUDE_DIRS 
  ${vtkPlusMicronTrackerInterface_SOURCE_DIR} 
  ${vtkPlusMicronTrackerInterface_BINARY_DIR} 
  )

SET(vtkPlusMicronTrackerInterface_SRCS
  MicronTrackerInterface.cpp
  MicronTrackerLogger.cpp
  Cameras.cpp
  MCamera.cpp
  Collection.cpp
  Facet.cpp
  Marker.cpp
  Markers.cpp
  MTXPoint.cpp
  Persistence.cpp
  Vector.cpp
  Xform3D.cpp
  XPoints.cpp
)

if (MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(vtkPlusMicronTrackerInterface_HDRS
    MicronTrackerInterface.h
    MicronTrackerLogger.h
    Cameras.h
    MCamera.h
    Collection.h
    Facet.h
    Marker.h
    Markers.h
    MTXPoint.h
    Persistence.h
    Vector.h
    Xform3D.h
    XPoints.h
)
endif()

SET (vtkPlusMicronTrackerInterface_INCLUDE_DIRS 
  ${vtkPlusMicronTrackerInterface_INCLUDE_DIRS} 
  ${MICRONTRACKER_INCLUDE_DIR}
  )

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${vtkPlusMicronTrackerInterface_INCLUDE_DIRS} CACHE INTERNAL "")

# Create a static library (to not have too many DLLs and also because
# the driver library interface heavily uses STL containers, which are unsafe
# to use at a DLL interface)
ADD_LIBRARY(vtkPlusMicronTrackerInterface STATIC ${vtkPlusMicronTrackerInterface_SRCS} ${vtkPlusMicronTrackerInterface_HDRS})
SET_TARGET_PROPERTIES(vtkPlusMicronTrackerInterface PROPERTIES FOLDER Utilities)
target_include_directories(vtkPlusMicronTrackerInterface PUBLIC ${vtkPlusMicronTrackerInterface_INCLUDE_DIRS} )

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkPlusMicronTrackerInterface CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkPlusMicronTrackerInterface vtkPlusMicronTrackerInterface;"${PlusLib_BINARY_DIR}/src/Utilities/MicronTrackerInterface/vtkPlusMicronTrackerInterface.vcxproj" CACHE INTERNAL "" FORCE) 

INSTALL(FILES ${vtkPlusMicronTrackerInterface_HDRS}
  DESTINATION include
  )