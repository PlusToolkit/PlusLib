SET (LIBS
     vtkxio
	 vtkPlusCommon
    )

# ---------------------- DiagDataCollection ---------------------
#
ADD_EXECUTABLE(SpatialSensorFusion SpatialSensorFusion.cxx)
TARGET_LINK_LIBRARIES(SpatialSensorFusion ${LIBS} )

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
INSTALL(TARGETS SpatialSensorFusion
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)
