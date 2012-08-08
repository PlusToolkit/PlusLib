SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(RfProcessor RfProcessor.cxx )
TARGET_LINK_LIBRARIES(RfProcessor vtkRfProcessingAlgo vtkCommon )

# --------------------------------------------------------------------------
ADD_TEST(vtkRfToBrightnessConvertRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoCurvilinearTest.xml
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

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertCurvilinearRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoCurvilinearTest.xml
	--input-rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.mha
	--output-img-file=outputUltrasonixCurvilinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
	)
SET_TESTS_PROPERTIES( vtkUsScanConvertCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertCurvilinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   outputUltrasonixCurvilinearScanConvertedData.mha
   ${TestDataDir}/UltrasonixCurvilinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertCurvilinearRunTest)

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertLinearRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/Testing/PlusConfiguration_RfProcessingAlgoLinearTest.xml
	--input-rf-file=${TestDataDir}/UltrasonixLinearRfData.mha
	--output-img-file=outputUltrasonixLinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
	)
SET_TESTS_PROPERTIES( vtkUsScanConvertLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertLinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   outputUltrasonixLinearScanConvertedData.mha
   ${TestDataDir}/UltrasonixLinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertLinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertLinearRunTest)

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertBkCurvilinearRunTest
	${EXECUTABLE_OUTPUT_PATH}/RfProcessor
	--input-config-file=${ConfigFilesDir}/PlusConfiguration_BkProFocus_OpenIGTLinkTracker.xml
	--input-rf-file=${TestDataDir}/BkCurvilinearRfData.mhd
	--output-img-file=outputBkCurvilinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
	)
SET_TESTS_PROPERTIES( vtkUsScanConvertBkCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertBkCurvilinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   outputBkCurvilinearScanConvertedData.mha
   ${TestDataDir}/BkCurvilinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertBkCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertBkCurvilinearRunTest)

# --------------------------------------------------------------------------
INSTALL(TARGETS RfProcessor
  RUNTIME
  DESTINATION bin
  CONFIGURATIONS Release
  COMPONENT RuntimeExecutables
  )
