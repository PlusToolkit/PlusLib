PROJECT( RANSAC )

SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "" FORCE) 

# --------------------------------------------------------------------------
# Find ITK
#
FIND_PACKAGE(ITK REQUIRED NO_MODULE)
INCLUDE(${ITK_USE_FILE})

SET (RANSAC_INCLUDE_DIRS
  ${RANSAC_SOURCE_DIR}
  ${RANSAC_SOURCE_DIR}/Common
)

SET(RANSAC_HDRS
  RANSAC.h
  RANSAC.txx
  ParametersEstimator.h
  ParametersEstimator.txx
  )

SET(EXAMPLES_DIRECTORY
  ${RANSAC_SOURCE_DIR}/Examples
  )

# --------------------------------------------------------------------------
# First example program showing the use of RANSAC for plane fitting
#
SET(PLANE_ESTIMATION_HDRS
  ${RANSAC_HDRS}
  PlaneParametersEstimator.h
  PlaneParametersEstimator.txx
  )

SET(PLANE_ESTIMATION_SRCS
  ${EXAMPLES_DIRECTORY}/planeEstimation.cxx
  )

ADD_EXECUTABLE(planeEstimation ${PLANE_ESTIMATION_HDRS} ${PLANE_ESTIMATION_SRCS})
SET_TARGET_PROPERTIES(planeEstimation PROPERTIES FOLDER Utilities)
FOREACH(p IN LISTS RANSAC_INCLUDE_DIRS)
  target_include_directories(planeEstimation PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(planeEstimation PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(planeEstimation PUBLIC 
  ITKCommon
  itkvnl
  itkvnl_algo
  )

# --------------------------------------------------------------------------
# Second example program showing the use of RANSAC for sphere fitting
#
SET(SPHERE_ESTIMATION_HDRS
  ${RANSAC_HDRS}
  SphereParametersEstimator.h
  SphereParametersEstimator.txx
  )

SET(SPHERE_ESTIMATION_SRCS
  ${EXAMPLES_DIRECTORY}/sphereEstimation.cxx
)

ADD_EXECUTABLE(sphereEstimation ${SPHERE_ESTIMATION_HDRS} ${SPHERE_ESTIMATION_SRCS})
SET_TARGET_PROPERTIES(sphereEstimation PROPERTIES FOLDER Utilities)
FOREACH(p IN LISTS RANSAC_INCLUDE_DIRS)
  target_include_directories(sphereEstimation PUBLIC $<BUILD_INTERFACE:${p}>
    )
ENDFOREACH()
target_include_directories(sphereEstimation PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}>)
TARGET_LINK_LIBRARIES(sphereEstimation PUBLIC 
  ITKCommon
  itkvnl
  itkvnl_algo
  )

INSTALL(TARGETS sphereEstimation planeEstimation EXPORT PlusLib DESTINATION ${PLUSLIB_BINARY_INSTALL} COMPONENT RuntimeExecutables)

# --------------------------------------------------------------------------
# Install
#
INSTALL(FILES ${RANSAC_HDRS} DESTINATION "${PLUSLIB_INCLUDE_INSTALL}" COMPONENT Development)

# --------------------------------------------------------------------------
# Testing
#
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()