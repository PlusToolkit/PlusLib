# --------------------------------------------------------------------------
# SpatialSensorFusion
ADD_EXECUTABLE(SpatialSensorFusion SpatialSensorFusion.cxx)
TARGET_LINK_LIBRARIES(SpatialSensorFusion PUBLIC vtkxio vtkPlusCommon)
GENERATE_HELP_DOC(SpatialSensorFusion)

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS SpatialSensorFusion
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF(PLUSAPP_INSTALL_BIN_DIR)

# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()