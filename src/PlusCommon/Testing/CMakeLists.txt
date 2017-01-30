SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPlusLoggerTest vtkPlusLoggerTest.cxx )
SET_TARGET_PROPERTIES(vtkPlusLoggerTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkPlusLoggerTest vtkPlusCommon )

ADD_TEST(vtkPlusLoggerTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusLoggerTest
  --verbose=5
  )

 #--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusCommonTest PlusCommonTest.cxx )
SET_TARGET_PROPERTIES(PlusCommonTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(PlusCommonTest vtkPlusCommon )

ADD_TEST(PlusCommonTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusCommonTest
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusMathTest PlusMathTest.cxx )
SET_TARGET_PROPERTIES(PlusMathTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(PlusMathTest vtkPlusCommon )

ADD_TEST(PlusMathTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusMathTest
  --xml-file=${TestDataDir}/PlusMathTestData.xml
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(AccurateTimerTest AccurateTimerTest.cxx )
SET_TARGET_PROPERTIES(AccurateTimerTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(AccurateTimerTest vtkPlusCommon )
GENERATE_HELP_DOC(AccurateTimerTest)

ADD_TEST(AccurateTimerTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/AccurateTimerTest
  --testTimeSec=10
  --averageIntendedDelaySec=0.005
  --numberOfThreads=3
  --verbose=3
  )
SET_TESTS_PROPERTIES( AccurateTimerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkTransformRepositoryTest vtkTransformRepositoryTest.cxx )
SET_TARGET_PROPERTIES(vtkTransformRepositoryTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkTransformRepositoryTest vtkPlusCommon )

ADD_TEST(vtkTransformRepositoryTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkTransformRepositoryTest
  --verbose=3
  )
# This test prints some errors when testing error cases, therefore the output is not
# checked for the presence of ERROR or WARNING string

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileTrim
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=TRIM
    --first-frame-index=0
    --last-frame-index=5
    --source-seq-file=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
    --use-compression
    --verbose=3
    WORKING_DIRECTORY ${PLUS_EXECUTABLE_OUTPUT_PATH}
    )
  SET_TESTS_PROPERTIES( EditSequenceFileTrim PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(EditSequenceFileTrimCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
     ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileTrimCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileTrim )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileReadWriteNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/NrrdSample.nrrd
    --output-seq-file=NrrdSample.nrrd
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileReadWriteNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
  ADD_TEST(EditSequenceFileReadWriteNrrdCompareToBaselineTest
     ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/NrrdSample.nrrd
     ${TestDataDir}/NrrdSample.nrrd
    )
  SET_TESTS_PROPERTIES( EditSequenceFileReadWriteNrrdCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileReadWriteNrrd )
 
  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileReadWriteColorNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/ColorNrrdSample.nrrd
    --output-seq-file=ColorNrrdSample.nrrd
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileReadWriteColorNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
  ADD_TEST(EditSequenceFileReadWriteColorNrrdCompareToBaselineTest
     ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/ColorNrrdSample.nrrd
     ${TestDataDir}/ColorNrrdSample.nrrd
    )
  SET_TESTS_PROPERTIES( EditSequenceFileReadWriteColorNrrdCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileReadWriteColorNrrd )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileFillImageRectangle
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=FILL_IMAGE_RECTANGLE
    --rect-origin 52 25
    --rect-size 260 25
    --fill-gray-level=20
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileFillImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  SET_TESTS_PROPERTIES( EditSequenceFileFillImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)

  ADD_TEST(EditSequenceFileFillImageRectangleCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha
     ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileFillImageRectangleCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileFillImageRectangle )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileCropImageRectangle
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=CROP
    --rect-origin 52 25
    --rect-size 260 25
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)

  ADD_TEST(EditSequenceFileCropImageRectangleCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha
     ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangleCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileCropImageRectangle )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileCropImageRectangleFlipX
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=CROP
    --flipX
    --rect-origin 52 25
    --rect-size 260 25
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangleFlipX PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangleFlipX PROPERTIES DEPENDS EditSequenceFileTrim)

  ADD_TEST(EditSequenceFileCropImageRectangleFlipXCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha
     ${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileCropImageRectangleFlipXCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileCropImageRectangleFlipX )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageData
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileRemoveImageData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(EditSequenceFileRemoveImageDataCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
     ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileRemoveImageDataCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileRemoveImageData )

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageDataCompressed
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES( EditSequenceFileRemoveImageDataCompressed PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(EditSequenceFileRemoveImageDataCompressedCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
     ${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
    )
  SET_TESTS_PROPERTIES( EditSequenceFileRemoveImageDataCompressedCompareToBaselineTest PROPERTIES DEPENDS EditSequenceFileRemoveImageDataCompressed )
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(
  TARGETS
    AccurateTimerTest
  DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  COMPONENT RuntimeExecutables
  )
