SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(RfProcessor RfProcessor.cxx )
TARGET_LINK_LIBRARIES(RfProcessor vtkRfProcessingAlgo vtkCommon )

ADD_TEST(vtkRfToBrightnessConvertRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoTest.xml
	--input-rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.mha
	--output-img-file=outputUltrasonixCurvilinearBrightnessData.mha 
  --operation=BRIGHTNESS_CONVERT
	)
SET_TESTS_PROPERTIES( vtkRfToBrightnessConvertRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkRfToBrightnessConvertCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   outputUltrasonixCurvilinearBrightnessData.mha
   ${TestDataDir}/UltrasonixCurvilinearBrightnessData.mha
  )
SET_TESTS_PROPERTIES(vtkRfToBrightnessConvertCompareToBaselineTest PROPERTIES DEPENDS vtkRfToBrightnessConvertRunTest)


ADD_TEST(vtkUsScanConvertRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoTest.xml
	--input-rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.mha
	--output-img-file=outputUltrasonixCurvilinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
	)
SET_TESTS_PROPERTIES( vtkUsScanConvertRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   outputUltrasonixCurvilinearScanConvertedData.mha
   ${TestDataDir}/UltrasonixCurvilinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertRunTest)

INSTALL(TARGETS RfProcessor
  RUNTIME
  DESTINATION bin
  CONFIGURATIONS Release
  COMPONENT RuntimeExecutables
  )
