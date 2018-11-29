PROJECT(MicronTrackerInterface)

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${${PROJECT_NAME}_SOURCE_DIR}
  ${${PROJECT_NAME}_BINARY_DIR}
  ${MicronTracker_INCLUDE_DIR}
  )

SET(${PROJECT_NAME}_SRCS
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

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS
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
ENDIF()

SET(Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${${PROJECT_NAME}_INCLUDE_DIRS} CACHE INTERNAL "")

# Create a static library (to not have too many DLLs and also because
# the driver library interface heavily uses STL containers, which are unsafe
# to use at a DLL interface)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER Utilities)
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
target_link_libraries(${PROJECT_NAME} PUBLIC ${MicronTracker_LIBRARY})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} ${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_${PROJECT_NAME} ${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/Utilities/MicronTrackerInterface/${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE) 

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(${PROJECT_NAME} ${PROJECT_NAME})