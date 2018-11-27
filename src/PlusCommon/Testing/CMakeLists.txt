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

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileTrim
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=TRIM
    --first-frame-index=0
    --last-frame-index=5
    --source-seq-file=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2.igs.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.igs.mha
    --use-compression
    --verbose=3
    WORKING_DIRECTORY ${PLUS_EXECUTABLE_OUTPUT_PATH}
    )
  SET_TESTS_PROPERTIES(EditSequenceFileTrim PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileTrimCompareToBaselineTest EditSequenceFileTrim
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.igs.mha)

  #--------------------------------------------------------------------------------------------
  IF(VTK_VERSION VERSION_LESS 8.2.0)
    SET(_NRRD_COMPARE_FILE NrrdSample.igs.nrrd)
    SET(_COLOR_NRRD_COMPARE_FILE ColorNrrdSample.igs.nrrd)
  ELSE()
    SET(_NRRD_COMPARE_FILE NrrdSample_vtk9.igs.nrrd)
    SET(_COLOR_NRRD_COMPARE_FILE ColorNrrdSample_vtk9.igs.nrrd)
  ENDIF()

  ADD_TEST(NAME EditSequenceFileReadWriteNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/${_NRRD_COMPARE_FILE}
    --output-seq-file=${_NRRD_COMPARE_FILE}
    --use-compression
    --verbose=3
    )

  SET_TESTS_PROPERTIES(EditSequenceFileReadWriteNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileReadWriteNrrdCompareToBaselineTest EditSequenceFileReadWriteNrrd
    ${_NRRD_COMPARE_FILE})

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileReadWriteColorNrrd
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --source-seq-file=${TestDataDir}/${_COLOR_NRRD_COMPARE_FILE}
    --output-seq-file=${_COLOR_NRRD_COMPARE_FILE}
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileReadWriteColorNrrd PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileReadWriteColorNrrdCompareToBaselineTest EditSequenceFileReadWriteColorNrrd
    ${_COLOR_NRRD_COMPARE_FILE})

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileFillImageRectangle
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=FILL_IMAGE_RECTANGLE
    --rect-origin 52 25
    --rect-size 260 25
    --fill-gray-level=20
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.igs.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.igs.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileFillImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileFillImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileFillImageRectangleCompareToBaselineTest EditSequenceFileFillImageRectangle
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Anonymized.igs.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileCropImageRectangle
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=CROP
    --rect-origin 52 25
    --rect-size 260 25
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.igs.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.igs.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangle PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangle PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileCropImageRectangleCompareToBaselineTest EditSequenceFileCropImageRectangle
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_PatientCropped.igs.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileCropImageRectangleFlipX
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=CROP
    --flipX
    --rect-origin 52 25
    --rect-size 260 25
    --source-seq-file=${TEST_OUTPUT_PATH}/SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed.igs.mha
    --output-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.igs.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangleFlipX PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  SET_TESTS_PROPERTIES(EditSequenceFileCropImageRectangleFlipX PROPERTIES DEPENDS EditSequenceFileTrim)
  ADD_COMPARE_FILES_TEST(EditSequenceFileCropImageRectangleFlipXCompareToBaselineTest EditSequenceFileCropImageRectangleFlipX
    SegmentationTest_BKMedical_RandomStepperMotionData2_Trimmed_Cropped_FlipX.igs.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageData
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.igs.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.igs.mha
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileRemoveImageData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileRemoveImageDataCompareToBaselineTest EditSequenceFileRemoveImageData
    UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUS.igs.mha)

  #--------------------------------------------------------------------------------------------
  ADD_TEST(NAME EditSequenceFileRemoveImageDataCompressed
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=REMOVE_IMAGE_DATA
    --source-seq-file=${TestDataDir}/UsSimulatorOutputSpinePhantom2CurvilinearBaseline.igs.mha
    --output-seq-file=UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.igs.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileRemoveImageDataCompressed PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ADD_COMPARE_FILES_TEST(EditSequenceFileRemoveImageDataCompressedCompareToBaselineTest EditSequenceFileRemoveImageDataCompressed
     UsSimulatorOutputSpinePhantom2CurvilinearBaselineNoUSCompressed.igs.mha)
    
  ADD_TEST(NAME EditSequenceFileMix 
    COMMAND $<TARGET_FILE:EditSequenceFile>
    --operation=MIX
    --source-seq-files ${TestDataDir}/WaterTankBottomTranslationVideoBuffer.igs.mha ${TestDataDir}/WaterTankBottomTranslationTrackerBuffer.igs.mha
    --output-seq-file=WaterTankBottomTranslationTrackedVideo.igs.mha
    --use-compression
    --verbose=3
    )
  SET_TESTS_PROPERTIES(EditSequenceFileMix PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

ENDIF(PLUSBUILD_BUILD_PlusLib_TOOLS)

 