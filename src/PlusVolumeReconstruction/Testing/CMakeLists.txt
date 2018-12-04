SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

function(VolRecRegressionTest TestName ConfigFileNameFragment InputSeqFile OutNameFragment)
  ADD_TEST(vtkVolumeReconstructorTestRun${TestName}
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_${ConfigFileNameFragment}.xml
    --source-seq-file=${TestDataDir}/${InputSeqFile}.igs.mha
    --output-volume-file=vtkVolumeReconstructorTest${OutNameFragment}volume.mha
    --image-to-reference-transform=ImageToReference
    --importance-mask-file=${TestDataDir}/ImportanceMask.png
    --disable-compression
    )
  SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRun${TestName} PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  # If expected filename is specified in a platform-specific directory then use that
  IF(WIN32)
    SET(PLATFORM "Windows")
  ELSE()
    SET(PLATFORM "Linux")
  ENDIF()
  SET(CommonExpectedVolumeFileName "${TestDataDir}/vtkVolumeReconstructorTest${OutNameFragment}volumeRef.mha")
  SET(PlatformSpecificExpectedVolumeFileName "${TestDataDir}/${PLATFORM}/vtkVolumeReconstructorTest${OutNameFragment}volumeRef.mha")
  if(EXISTS "${PlatformSpecificExpectedVolumeFileName}")
    SET(ExpectedVolumeFileName ${PlatformSpecificExpectedVolumeFileName})
  ELSE()
    SET(ExpectedVolumeFileName ${CommonExpectedVolumeFileName})
  endif()
  
  ADD_TEST(vtkVolumeReconstructorTestCompare${TestName}
    ${CMAKE_COMMAND} -E compare_files
    ${ExpectedVolumeFileName}
    ${TEST_OUTPUT_PATH}/vtkVolumeReconstructorTest${OutNameFragment}volume.mha
    )
  SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompare${TestName} PROPERTIES DEPENDS vtkVolumeReconstructorTestRun${TestName})
endfunction()

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  VolRecRegressionTest(NearLateUChar SonixRP_TRUS_D70mm_NN_LATE SpinePhantomFreehand NNLATE)
  VolRecRegressionTest(NearMeanUChar SpinePhantom_NN_MEAN SpinePhantomFreehand NNMEAN)
  VolRecRegressionTest(NearMaxiFloat SpinePhantom_NN_MAXI SpinePhantomFreehand3FramesFloat NNMAXI)
  VolRecRegressionTest(LinrLateFloat SpinePhantom_LN_LATE SpinePhantomFreehand3FramesFloat LNLATE)
  VolRecRegressionTest(LinrMeanUChar SonixRP_TRUS_D70mm_LN_MEAN SpinePhantomFreehand LNMEAN)
  VolRecRegressionTest(LinrMaxiUChar SpinePhantom_LN_MAXI SpinePhantomFreehand LNMAXI)

  # Importance mask tests
  VolRecRegressionTest(IMLinearFull ImportanceMaskLinearFull ImportanceMaskInput IMLiF)
  VolRecRegressionTest(IMLinearPartial ImportanceMaskLinearPartial ImportanceMaskInput IMLiP)
  VolRecRegressionTest(IMLinearNone ImportanceMaskLinearNone ImportanceMaskInput IMLiN)
  VolRecRegressionTest(IMNearFull ImportanceMaskNNFull ImportanceMaskInput IMNNF)
  VolRecRegressionTest(IMNearPartial ImportanceMaskNNPartial ImportanceMaskInput IMNNP)
  VolRecRegressionTest(IMNearNone ImportanceMaskNNNone ImportanceMaskInput IMNNN)

  ADD_TEST(CreateSliceModelsTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/CreateSliceModels
    --source-seq-file=${TestDataDir}/NwirePhantomFreehand.igs.mha
    --output-model-file=GeneratedSliceModels.mha
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_NwirePhantomFreehand_vtkVolumeReconstructorTest2.xml
    --image-to-reference-transform=ProbeToReference
    )
  SET_TESTS_PROPERTIES( CreateSliceModelsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(DrawClipRegionRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/DrawClipRegion
    --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Ultrasonix_C5-2_NDIPolaris_fCal3.xml
    --source-seq-file=${TestDataDir}/SpinePhantomPartialSurfaceContact.igs.mha
    --output-seq-file=outputSpinePhantomPartialSurfaceContactWithClipRegion.igs.mha
    )
  SET_TESTS_PROPERTIES( DrawClipRegionRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(DrawClipRegionCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files
     ${TEST_OUTPUT_PATH}/outputSpinePhantomPartialSurfaceContactWithClipRegion.igs.mha
     ${TestDataDir}/SpinePhantomPartialSurfaceContactWithClipRegionBaseline.igs.mha
    )
  SET_TESTS_PROPERTIES(DrawClipRegionCompareToBaselineTest PROPERTIES DEPENDS DrawClipRegionRunTest)
ENDIF()