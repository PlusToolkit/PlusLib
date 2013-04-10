SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkGnuplotExecuterTest vtkGnuplotExecuterTest.cxx )
TARGET_LINK_LIBRARIES(vtkGnuplotExecuterTest vtkCommon vtkPlusCommon )

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
ADD_EXECUTABLE(vtkAccurateTimerTest vtkAccurateTimerTest.cxx )
TARGET_LINK_LIBRARIES(vtkAccurateTimerTest vtkPlusCommon )

ADD_TEST(vtkAccurateTimerTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkAccurateTimerTest
  --testTimeSec=10
  --averageIntendedDelaySec=0.005
  --numberOfThreads=3
  --verbose=3
  )
SET_TESTS_PROPERTIES( vtkAccurateTimerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

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

#--------------------------------------------------------------------------------------------
ADD_TEST(EditSeqMetaFileTrim
  ${EXECUTABLE_OUTPUT_PATH}/EditSeqMetaFile
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
ADD_TEST(EditSeqMetaFileFillImageRectangle
  ${EXECUTABLE_OUTPUT_PATH}/EditSeqMetaFile
  --operation=FILL_IMAGE_RECTANGLE
  --fill-rect-origin 52 25
  --fill-rect-size 260 25
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

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS EditSeqMetaFile
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
