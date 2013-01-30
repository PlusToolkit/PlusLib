cmake_minimum_required(VERSION 2.8)

PROJECT( RANSAC )

#
# Find ITK
#
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot find ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES (
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

LINK_LIBRARIES (
  ITKCommon
  itkvnl
  itkvnl_algo
)

#
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
TARGET_LINK_LIBRARIES(planeEstimation ${LINK_LIBRARIES})


#
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
TARGET_LINK_LIBRARIES(sphereEstimation ${LINK_LIBRARIES})


#
# Optional testing of the plane and sphere estimation code
#
OPTION(BUILD_TESTING "Build the Testing directory." ON)
IF (BUILD_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(Testing)
ENDIF (BUILD_TESTING)
