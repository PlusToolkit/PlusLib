SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPlusLoggerTest vtkPlusLoggerTest.cxx )
TARGET_LINK_LIBRARIES(vtkPlusLoggerTest vtkPlusCommon )

ADD_TEST(vtkPlusLoggerTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPlusLoggerTest
  --verbose=5
  )

 #--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusCommonTest PlusCommonTest.cxx )
TARGET_LINK_LIBRARIES(PlusCommonTest vtkPlusCommon )

ADD_TEST(PlusCommonTest 
  ${EXECUTABLE_OUTPUT_PATH}/PlusCommonTest
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusMathTest PlusMathTest.cxx )
TARGET_LINK_LIBRARIES(PlusMathTest vtkPlusCommon )

ADD_TEST(PlusMathTest 
  ${EXECUTABLE_OUTPUT_PATH}/PlusMathTest
  --xml-file=${TestDataDir}/PlusMathTestData.xml 
  )
  
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(AccurateTimerTest AccurateTimerTest.cxx )
TARGET_LINK_LIBRARIES(AccurateTimerTest vtkPlusCommon )
GENERATE_HELP_DOC(AccurateTimerTest)

ADD_TEST(AccurateTimerTest 
  ${EXECUTABLE_OUTPUT_PATH}/AccurateTimerTest
  --testTimeSec=10
  --averageIntendedDelaySec=0.005
  --numberOfThreads=3
  --verbose=3
  )
SET_TESTS_PROPERTIES( AccurateTimerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkTransformRepositoryTest vtkTransformRepositoryTest.cxx )
TARGET_LINK_LIBRARIES(vtkTransformRepositoryTest vtkPlusCommon )

ADD_TEST(vtkTransformRepositoryTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkTransformRepositoryTest
  --verbose=3
  )
# This test prints some errors when testing error cases, therefore the output is not
# checked for the presence of ERROR or WARNING string

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(EditSeqMetaFile EditSeqMetaFile.cxx )
TARGET_LINK_LIBRARIES(EditSeqMetaFile vtkPlusCommon )
GENERATE_HELP_DOC(EditSeqMetaFile)

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileTrim
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=TRIM
  --first-frame-index=0
  --last-frame-index=5
  --source-seq-file=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2.mha 
  --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha 
  --use-compression
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileTrim PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )  

ADD_TEST(EditSeqMetaFileTrimCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
   ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileTrimCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileTrim )

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileFillImageRectangle
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=FILL_IMAGE_RECTANGLE
  --rect-origin 52 25
  --rect-size 260 25
  --fill-gray-level=20
  --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha 
  --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha 
  --use-compression
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileFillImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )  
SET_TESTS_PROPERTIES( EditSeqMetaFileFillImageRectangle PROPERTIES DEPENDS EditSeqMetaFileTrim)

ADD_TEST(EditSeqMetaFileFillImageRectangleCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha
   ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileFillImageRectangleCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileFillImageRectangle )

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileCropImageRectangle
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=CROP
  --rect-origin 52 25
  --rect-size 260 25
  --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha 
  --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha 
  --use-compression
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )  
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangle PROPERTIES DEPENDS EditSeqMetaFileTrim)

ADD_TEST(EditSeqMetaFileCropImageRectangleCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha
   ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangleCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileCropImageRectangle )

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileCropImageRectangleFlipX
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=CROP
  --flipX
  --rect-origin 52 25
  --rect-size 260 25
  --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha 
  --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha 
  --use-compression
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangleFlipX PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )  
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangleFlipX PROPERTIES DEPENDS EditSeqMetaFileTrim)

ADD_TEST(EditSeqMetaFileCropImageRectangleFlipXCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha
   ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileCropImageRectangleFlipXCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileCropImageRectangleFlipX )

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileRemoveImageData
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=REMOVE_IMAGE_DATA 
  --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha  
  --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileRemoveImageData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(EditSeqMetaFileRemoveImageDataCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
   ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileRemoveImageDataCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileRemoveImageData )

#--------------------------------------------------------------------------------------------
ADD_TEST(NAME EditSeqMetaFileRemoveImageDataCompressed
  COMMAND $<TARGET_FILE:EditSeqMetaFile>
  --operation=REMOVE_IMAGE_DATA
  --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha  
  --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
  --use-compression
  --verbose=3
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileRemoveImageDataCompressed PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(EditSeqMetaFileRemoveImageDataCompressedCompareToBaselineTest
  ${CMAKE_COMMAND} -E compare_files 
   ${TEST_OUTPUT_PATH}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
   ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
  )
SET_TESTS_PROPERTIES( EditSeqMetaFileRemoveImageDataCompressedCompareToBaselineTest PROPERTIES DEPENDS EditSeqMetaFileRemoveImageDataCompressed )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(ModelRenderer ModelRenderer.cxx )
TARGET_LINK_LIBRARIES(ModelRenderer vtkPlusCommon )

# --------------------------------------------------------------------------
# Install
#

INSTALL(
  TARGETS
    EditSeqMetaFile
    AccurateTimerTest
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
