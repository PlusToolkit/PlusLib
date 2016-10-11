# --------------------------------------------------------------------------
# SpatialSensorFusion
ADD_EXECUTABLE(SpatialSensorFusion SpatialSensorFusion.cxx)
SET_TARGET_PROPERTIES(SpatialSensorFusion PROPERTIES FOLDER Utilities)
TARGET_LINK_LIBRARIES(SpatialSensorFusion PUBLIC vtkxio vtkPlusCommon)
GENERATE_HELP_DOC(SpatialSensorFusion)

# --------------------------------------------------------------------------
# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
  INSTALL(TARGETS SpatialSensorFusion
    DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
    COMPONENT RuntimeLibraries
    )
ENDIF()

# --------------------------------------------------------------------------
# Testing
IF (BUILD_TESTING)
  ADD_SUBDIRECTORY (Testing)
ENDIF()