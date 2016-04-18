SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_EXECUTABLE(RfProcessor RfProcessor.cxx )
TARGET_LINK_LIBRARIES(RfProcessor vtkImageProcessingAlgo ${VTK_LIBRARIES} )
GENERATE_HELP_DOC(RfProcessor)

# --------------------------------------------------------------------------
ADD_TEST(vtkRfToBrightnessConvertRunTest
  ${EXECUTABLE_OUTPUT_PATH}/RfProcessor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
  --rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.mha
  --output-img-file=outputUltrasonixCurvilinearBrightnessData.mha 
  --use-compression=false
  --operation=BRIGHTNESS_CONVERT
  )
SET_TESTS_PROPERTIES( vtkRfToBrightnessConvertRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkRfToBrightnessConvertCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/outputUltrasonixCurvilinearBrightnessData_OutputChannel_ScanConvertOutput.mha
   ${TestDataDir}/UltrasonixCurvilinearBrightnessData.mha
  )
SET_TESTS_PROPERTIES(vtkRfToBrightnessConvertCompareToBaselineTest PROPERTIES DEPENDS vtkRfToBrightnessConvertRunTest)

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertCurvilinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/RfProcessor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
  --rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.mha
  --output-img-file=outputUltrasonixCurvilinearScanConvertedData.mha 
  --use-compression=false
  --operation=BRIGHTNESS_SCAN_CONVERT
  )
SET_TESTS_PROPERTIES( vtkUsScanConvertCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertCurvilinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/outputUltrasonixCurvilinearScanConvertedData_OutputChannel_ScanConvertOutput.mha
   ${TestDataDir}/UltrasonixCurvilinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertCurvilinearRunTest)

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertLinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/RfProcessor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoLinearTest.xml
  --rf-file=${TestDataDir}/UltrasonixLinearRfData.mha
  --output-img-file=outputUltrasonixLinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
  --use-compression=false
  )
SET_TESTS_PROPERTIES( vtkUsScanConvertLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertLinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/outputUltrasonixLinearScanConvertedData_OutputChannel_ScanConvertOutput.mha
   ${TestDataDir}/UltrasonixLinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertLinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertLinearRunTest)

# --------------------------------------------------------------------------
ADD_TEST(vtkUsScanConvertBkCurvilinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/RfProcessor
  --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_BkProFocus_OpenIGTLinkTracker.xml
  --rf-file=${TestDataDir}/BkCurvilinearRfData.mhd
  --output-img-file=outputBkCurvilinearScanConvertedData.mha 
  --operation=BRIGHTNESS_SCAN_CONVERT
  --use-compression=false
  )
SET_TESTS_PROPERTIES( vtkUsScanConvertBkCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUsScanConvertBkCurvilinearCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/outputBkCurvilinearScanConvertedData_OutputChannel_VideoStream.mha
   ${TestDataDir}/BkCurvilinearScanConvertedData.mha
  )
SET_TESTS_PROPERTIES(vtkUsScanConvertBkCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertBkCurvilinearRunTest)

#---------------------------------------------------------------------------
ADD_EXECUTABLE(DrawScanLines DrawScanLines.cxx )
TARGET_LINK_LIBRARIES(DrawScanLines vtkImageProcessingAlgo )
GENERATE_HELP_DOC(DrawScanLines)

# --------------------------------------------------------------------------
ADD_TEST(DrawScanLinesCurvilinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/DrawScanLines
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
  --source-seq-file=${TestDataDir}/UltrasonixCurvilinearScanConvertedData.mha
  --output-seq-file=UltrasonixCurvilinearScanConvertedDataWithScanlines.mha
  )
SET_TESTS_PROPERTIES( DrawScanLinesCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#ADD_TEST(vtkUsScanConvertCurvilinearCompareToBaselineTest
#  ${CMAKE_COMMAND} -E compare_files 
#   ${TEST_OUTPUT_PATH}/outputUltrasonixCurvilinearScanConvertedData_OutputChannel_ScanConvertOutput.mha
#   ${TestDataDir}/UltrasonixCurvilinearScanConvertedData.mha
#  )
#SET_TESTS_PROPERTIES(vtkUsScanConvertCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertCurvilinearRunTest)

# --------------------------------------------------------------------------
ADD_TEST(DrawScanLinesLinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/DrawScanLines
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoLinearTest.xml
  --source-seq-file=${TestDataDir}/UltrasonixLinearScanConvertedData.mha
  --output-seq-file=UltrasonixLinearScanConvertedDataWithScanlines.mha
  )
SET_TESTS_PROPERTIES( vtkUsScanConvertLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#ADD_TEST(vtkUsScanConvertLinearCompareToBaselineTest
#  ${CMAKE_COMMAND} -E compare_files 
#   ${TEST_OUTPUT_PATH}/outputUltrasonixLinearScanConvertedData_OutputChannel_ScanConvertOutput.mha   
#  )
#SET_TESTS_PROPERTIES(vtkUsScanConvertLinearCompareToBaselineTest PROPERTIES DEPENDS vtkUsScanConvertLinearRunTest)

#---------------------------------------------------------------------------
ADD_EXECUTABLE(ExtractScanLines ExtractScanLines.cxx)
TARGET_LINK_LIBRARIES(ExtractScanLines vtkImageProcessingAlgo)
GENERATE_HELP_DOC(ExtractScanLines)

#---------------------------------------------------------------------------
ADD_TEST(ExtractScanLinesCurvilinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/ExtractScanLines
  --config-file=${ConfigFilesDir}/Testing/SpineUltrasound-Lumbar-C5_config.xml
  --input-seq-file=${TestDataDir}/SpineUltrasound-Lumbar-C5.mha
  --output-seq-file=SpineUltrasound-Lumbar-C5_ScanLines.mha 
  )
SET_TESTS_PROPERTIES(ExtractScanLinesCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#---------------------------------------------------------------------------
ADD_TEST(ExtractScanLinesLinearRunTest
  ${EXECUTABLE_OUTPUT_PATH}/ExtractScanLines
  --config-file=${ConfigFilesDir}/Testing/BoneUltrasound_L14_config.xml
  --input-seq-file=${TestDataDir}/BoneUltrasound_L14.mha
  --output-seq-file=BoneUltrasound_L14_ScanLines.mha 
  )
SET_TESTS_PROPERTIES(ExtractScanLinesLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#---------------------------------------------------------------------------
ADD_EXECUTABLE(ScanConvert ScanConvert.cxx)
TARGET_LINK_LIBRARIES(ScanConvert vtkImageProcessingAlgo)
GENERATE_HELP_DOC(ScanConvert)

#---------------------------------------------------------------------------
IF (PLUS_USE_INTEL_MKL)
  ADD_EXECUTABLE(EnhanceBone EnhanceBone.cxx )
  TARGET_LINK_LIBRARIES(EnhanceBone vtkImageProcessingAlgo )
  GENERATE_HELP_DOC(EnhanceBone)
ENDIF (PLUS_USE_INTEL_MKL)

# --------------------------------------------------------------------------
SET (INSTALL_TARGETS
  RfProcessor
  DrawScanLines
  ExtractScanLines
  ScanConvert
  )
IF (PLUS_USE_INTEL_MKL)
  SET (INSTALL_TARGETS ${INSTALL_TARGETS} EnhanceBone )
ENDIF (PLUS_USE_INTEL_MKL)

INSTALL(
  TARGETS ${INSTALL_TARGETS}
  RUNTIME
  DESTINATION bin
  CONFIGURATIONS Release
  COMPONENT RuntimeExecutables
  )
