SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
function(ADD_COMPARE_FILES_TEST TestName DependsOnTestName TestFileName)

  # If a platform-specific reference file is found then use that
  IF(WIN32)
    SET(PLATFORM "Windows")
  ELSE()
    SET(PLATFORM "Linux")
  ENDIF()
  SET(CommonFilePath "${TestDataDir}/${TestFileName}")
  SET(PlatformSpecificFilePath "${TestDataDir}/${PLATFORM}/${TestFileName}")
  if(EXISTS "${PlatformSpecificFilePath}")
    SET(FoundReferenceFilePath ${PlatformSpecificFilePath})
  ELSE()
    SET(FoundReferenceFilePath ${CommonFilePath})
  endif()

  ADD_TEST(${TestName} ${CMAKE_COMMAND} -E compare_files "${TEST_OUTPUT_PATH}/${TestFileName}" "${FoundReferenceFilePath}")
  SET_TESTS_PROPERTIES(${TestName} PROPERTIES DEPENDS ${DependsOnTestName})

endfunction()

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
SET_TESTS_PROPERTIES(AccurateTimerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

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
  SET_TESTS_PROPERTIES(EditSequenceFileTrim PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileTrimCompareToBaselineTest EditSequenceFileTrim
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileReadWriteNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/NrrdSample.nrrd
    --output-seq-file=NrrdSample.nrrd
    --use-compression
    --verbose=3
    )

  SET_TESTS_PROPERTIES(EditSequenceFileReadWriteNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  IF(VTK_VERSION_MAJOR LESS 9)
    SET(_NRRD_COMPARE_FILE NrrdSample.nrrd)
  ELSE()
    SET(_NRRD_COMPARE_FILE NrrdSample_vtk9.nrrd)
  ENDIF()
  ADD_COMPARE_FILES_TEST(EditSequenceFileReadWriteNrrdCompareToBaselineTest EditSequenceFileReadWriteNrrd
    ${_NRRD_COMPARE_FILE})

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileReadWriteColorNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/ColorNrrdSample.nrrd
    --output-seq-file=ColorNrrdSample.nrrd
    --use-compression
    --verbose=3
    )
  IF(VTK_VERSION_MAJOR LESS 9)
    SET(_NRRD_COMPARE_FILE ColorNrrdSample.nrrd)
  ELSE()
    SET(_NRRD_COMPARE_FILE ColorNrrdSample_vtk9.nrrd)
  ENDIF()
  SET_TESTS_PROPERTIES(EditSequenceFileReadWriteColorNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileReadWriteColorNrrdCompareToBaselineTest EditSequenceFileReadWriteColorNrrd
    ${_NRRD_COMPARE_FILE})

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
  SET_TESTS_PROPERTIES(EditSequenceFileFillImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileFillImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileFillImageRectangleCompareToBaselineTest EditSequenceFileFillImageRectangle
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.mha)

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
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileCropImageRectangleCompareToBaselineTest EditSequenceFileCropImageRectangle
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.mha)

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
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangleFlipX PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangleFlipX PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileCropImageRectangleFlipXCompareToBaselineTest EditSequenceFileCropImageRectangleFlipX
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageData
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileRemoveImageData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileRemoveImageDataCompareToBaselineTest EditSequenceFileRemoveImageData
    UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageDataCompressed
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileRemoveImageDataCompressed PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileRemoveImageDataCompressedCompareToBaselineTest EditSequenceFileRemoveImageDataCompressed
     UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.mha)
    
  ADD_TEST(NAME EditSequenceFileMix 
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=MIX
    --source-seq-files ${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha ${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.mha
    --output-seq-file=WaterTankBottomTranslationTrackedVideo.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileMix PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

ENDIF(PLUSBUILD_BUILD_PlusLib_TOOLS)

# --------------------------------------------------------------------------
# Install
#
INSTALL(
  TARGETS
    AccurateTimerTest
  DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  COMPONENT RuntimeExecutables
  )
 