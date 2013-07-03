SET (LIBS
     vtkxio
	   vtkPlusCommon
     vtkDataCollection
    )

# ---------------------- SpatialSensorFusion ---------------------
#
ADD_EXECUTABLE(SpatialSensorFusion SpatialSensorFusion.cxx)
TARGET_LINK_LIBRARIES(SpatialSensorFusion ${LIBS} )

# Testing
SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_TEST(SpatialSensorFusionTest
${EXECUTABLE_OUTPUT_PATH}/SpatialSensorFusion
--ahrs-algo=MADGWICK_IMU
--ahrs-algo-gain 1.5 
--initial-gain 1
--initial-repeated-frame-number=1000
--input-seq-file="${TestDataDir}/SpatialSensorFusionTestInput.mha"
--output-seq-file="${TestDataDir}/SpatialSensorFusionTestOutput.mha" 
--baseline-seq-file="${TestDataDir}/SpatialSensorFusionTestBaseline.mha"
--west-axis-index=1
)

SET_TESTS_PROPERTIES( SpatialSensorFusionTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# Install
IF(PLUSAPP_INSTALL_BIN_DIR)
INSTALL(TARGETS SpatialSensorFusion
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)
