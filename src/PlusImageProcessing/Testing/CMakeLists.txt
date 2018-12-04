SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

# -----------------  vtkPlusTransverseProcessEnhancerTest -------------------
ADD_EXECUTABLE(vtkPlusTransverseProcessEnhancerTest vtkPlusTransverseProcessEnhancerTest.cxx )
SET_TARGET_PROPERTIES(vtkPlusTransverseProcessEnhancerTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkPlusTransverseProcessEnhancerTest 
  vtkPlusCommon 
  vtkIGSIOCommon
  vtkPlusDataCollection 
  vtkPlusImageProcessing 
  )

ADD_TEST(vtkPlusTransverseProcessEnhancerTest 
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusTransverseProcessEnhancerTest
  --input-seq-file=${TestDataDir}/PlusTransverseProcessEnhancerTestData.igs.mha
  --output-seq-file=${TestDataDir}/outputPlusTransverseProcessEnhancerTest.igs.mha
  --input-config-file=${ConfigFilesDir}/Testing/PlusTransverseProcessEnhancerTestingParameters.xml
  --save-intermediate-images=false
  )
SET_TESTS_PROPERTIES( vtkPlusTransverseProcessEnhancerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR" )

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  # --------------------------------------------------------------------------
  ADD_TEST(vtkPlusRfToBrightnessConvertRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/RfProcessor
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
    --rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.igs.mha
    --output-img-file=outputUltrasonixCurvilinearBrightnessData.igs.mha 
    --use-compression=false
    --operation=BRIGHTNESS_CONVERT
    )
  SET_TESTS_PROPERTIES( vtkPlusRfToBrightnessConvertRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkPlusRfToBrightnessConvertCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files 
     ${TEST_OUTPUT_PATH}/outputUltrasonixCurvilinearBrightnessData_OutputChannel_ScanConvertOutput.igs.mha
     ${TestDataDir}/UltrasonixCurvilinearBrightnessData.igs.mha
    )
  SET_TESTS_PROPERTIES(vtkPlusRfToBrightnessConvertCompareToBaselineTest PROPERTIES DEPENDS vtkPlusRfToBrightnessConvertRunTest)

  # --------------------------------------------------------------------------
  ADD_TEST(vtkPlusUsScanConvertCurvilinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/RfProcessor
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
    --rf-file=${TestDataDir}/UltrasonixCurvilinearRfData.igs.mha
    --output-img-file=outputUltrasonixCurvilinearScanConvertedData.igs.mha 
    --use-compression=false
    --operation=BRIGHTNESS_SCAN_CONVERT
    )
  SET_TESTS_PROPERTIES( vtkPlusUsScanConvertCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkPlusUsScanConvertCurvilinearCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files 
     ${TEST_OUTPUT_PATH}/outputUltrasonixCurvilinearScanConvertedData_OutputChannel_ScanConvertOutput.igs.mha
     ${TestDataDir}/UltrasonixCurvilinearScanConvertedData.igs.mha
    )
  SET_TESTS_PROPERTIES(vtkPlusUsScanConvertCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkPlusUsScanConvertCurvilinearRunTest)

  # --------------------------------------------------------------------------
  ADD_TEST(vtkPlusUsScanConvertLinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/RfProcessor
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoLinearTest.xml
    --rf-file=${TestDataDir}/UltrasonixLinearRfData.igs.mha
    --output-img-file=outputUltrasonixLinearScanConvertedData.igs.mha 
    --operation=BRIGHTNESS_SCAN_CONVERT
    --use-compression=false
    )
  SET_TESTS_PROPERTIES( vtkPlusUsScanConvertLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkPlusUsScanConvertLinearCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files 
     ${TEST_OUTPUT_PATH}/outputUltrasonixLinearScanConvertedData_OutputChannel_ScanConvertOutput.igs.mha
     ${TestDataDir}/UltrasonixLinearScanConvertedData.igs.mha
    )
  SET_TESTS_PROPERTIES(vtkPlusUsScanConvertLinearCompareToBaselineTest PROPERTIES DEPENDS vtkPlusUsScanConvertLinearRunTest)

  # --------------------------------------------------------------------------
  ADD_TEST(vtkPlusUsScanConvertBkCurvilinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/RfProcessor
    --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_BkProFocus_OpenIGTLinkTracker.xml
    --rf-file=${TestDataDir}/BkCurvilinearRfData.igs.mhd
    --output-img-file=outputBkCurvilinearScanConvertedData.igs.mha 
    --operation=BRIGHTNESS_SCAN_CONVERT
    --use-compression=false
    )
  SET_TESTS_PROPERTIES( vtkPlusUsScanConvertBkCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(vtkPlusUsScanConvertBkCurvilinearCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files 
     ${TEST_OUTPUT_PATH}/outputBkCurvilinearScanConvertedData_OutputChannel_VideoStream.igs.mha
     ${TestDataDir}/BkCurvilinearScanConvertedData.igs.mha
    )
  SET_TESTS_PROPERTIES(vtkPlusUsScanConvertBkCurvilinearCompareToBaselineTest PROPERTIES DEPENDS vtkPlusUsScanConvertBkCurvilinearRunTest)

  # --------------------------------------------------------------------------
  ADD_TEST(DrawScanLinesCurvilinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/DrawScanLines
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoCurvilinearTest.xml
    --source-seq-file=${TestDataDir}/UltrasonixCurvilinearScanConvertedData.igs.mha
    --output-seq-file=UltrasonixCurvilinearScanConvertedDataWithScanlines.igs.mha
    )
  SET_TESTS_PROPERTIES( DrawScanLinesCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  # --------------------------------------------------------------------------
  ADD_TEST(DrawScanLinesLinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/DrawScanLines
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_RfProcessingAlgoLinearTest.xml
    --source-seq-file=${TestDataDir}/UltrasonixLinearScanConvertedData.igs.mha
    --output-seq-file=UltrasonixLinearScanConvertedDataWithScanlines.igs.mha
    )
  SET_TESTS_PROPERTIES( vtkPlusUsScanConvertLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  #---------------------------------------------------------------------------
  ADD_TEST(ExtractScanLinesCurvilinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/ExtractScanLines
    --config-file=${ConfigFilesDir}/Testing/SpineUltrasound-Lumbar-C5_config.xml
    --input-seq-file=${TestDataDir}/SpineUltrasound-Lumbar-C5.igs.mha
    --output-seq-file=SpineUltrasound-Lumbar-C5_ScanLines.igs.mha 
    )
  SET_TESTS_PROPERTIES(ExtractScanLinesCurvilinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

  #---------------------------------------------------------------------------
  ADD_TEST(ExtractScanLinesLinearRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/ExtractScanLines
    --config-file=${ConfigFilesDir}/Testing/BoneUltrasound_L14_config.xml
    --input-seq-file=${TestDataDir}/BoneUltrasound_L14.igs.mha
    --output-seq-file=BoneUltrasound_L14_ScanLines.igs.mha 
    )
  SET_TESTS_PROPERTIES(ExtractScanLinesLinearRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()