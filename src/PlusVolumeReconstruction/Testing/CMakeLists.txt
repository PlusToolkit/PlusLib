SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

function(RegressionTestMCI ImageName Ext) #optional: Algo Axis Label
  foreach(f ${ARGN}) #create unique test name
    set(optArgs "${optArgs}_${f}")
  endforeach()
  set(outImage "${ITK_TEST_OUTPUT_DIR}/${ImageName}${optArgs}.${Ext}")
  set(imageNameOpt "${ImageName}${optArgs}")
  if(${MCI_TestWithRLEImage})
    itk_add_test( NAME rleMCI_${ImageName}${optArgs}
      COMMAND MorphologicalContourInterpolationTestDriver
      --compare DATA{Baseline/${imageNameOpt}.${Ext}} ${outImage}
      itkMorphologicalContourInterpolationTestWithRLEImage
        DATA{Input/${ImageName}.${Ext}} ${outImage} ${ARGN})
  else(${MCI_TestWithRLEImage})
    itk_add_test( NAME itkMCI_${ImageName}${optArgs}
      COMMAND MorphologicalContourInterpolationTestDriver
      --compare DATA{Baseline/${imageNameOpt}.${Ext}} ${outImage}
      itkMorphologicalContourInterpolationTest
        DATA{Input/${ImageName}.${Ext}} ${outImage} ${ARGN})
  endif(${MCI_TestWithRLEImage})
endfunction()

function(RegressionTest TestName Config Seq Out)
  ADD_TEST(vtkVolumeReconstructorTestRun${TestName}
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_${Config}.xml
    --source-seq-file=${TestDataDir}/${Seq}.mha
    --output-volume-file=vtkVolumeReconstructorTest${Out}volume.mha
    --image-to-reference-transform=ImageToReference
    )
  SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRun${TestName} PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  
  ADD_TEST(vtkVolumeReconstructorTestCompare${TestName}
    ${CMAKE_COMMAND} -E compare_files 
    ${TestDataDir}/vtkVolumeReconstructorTest${Out}volumeRef.mha
    vtkVolumeReconstructorTest${Out}volume.mha
    )
  SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompare${TestName} PROPERTIES DEPENDS vtkVolumeReconstructorTestRun${TestName})
endfunction()

IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  #RegressionTest(TestSpecificName ConfigFileNameFragment InputSeqFile OutNameFragment)
  RegressionTest(NearLateUChar SonixRP_TRUS_D70mm_NN_LATE SpinePhantomFreehand NNLATE)
  RegressionTest(NearMeanUChar SpinePhantom_NN_MEAN SpinePhantomFreehand NNMEAN)
  RegressionTest(NearMaxiFloat SpinePhantom_NN_MAXI SpinePhantomFreehand3FramesFloat NNMAXI)
  RegressionTest(LinrLateFloat SpinePhantom_LN_LATE SpinePhantomFreehand3FramesFloat LNLATE)
  RegressionTest(LinrMeanUChar SonixRP_TRUS_D70mm_LN_MEAN SpinePhantomFreehand LNMEAN)
  RegressionTest(LinrMaxiUChar SpinePhantom_LN_MAXI SpinePhantomFreehand LNMAXI)


  ADD_TEST(CreateSliceModelsTest 
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/CreateSliceModels
    --source-seq-file=${TestDataDir}/NwirePhantomFreehand.mha
    --output-model-file=GeneratedSliceModels.mha  
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_NwirePhantomFreehand_vtkVolumeReconstructorTest2.xml
    --image-to-reference-transform=ProbeToReference
    )
  SET_TESTS_PROPERTIES( CreateSliceModelsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(DrawClipRegionRunTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/DrawClipRegion
    --config-file=${ConfigFilesDir}/PlusDeviceSet_fCal_Ultrasonix_C5-2_NDIPolaris_fCal3.xml
    --source-seq-file=${TestDataDir}/SpinePhantomPartialSurfaceContact.mha
    --output-seq-file=outputSpinePhantomPartialSurfaceContactWithClipRegion.mha
    )
  SET_TESTS_PROPERTIES( DrawClipRegionRunTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

  ADD_TEST(DrawClipRegionCompareToBaselineTest
    ${CMAKE_COMMAND} -E compare_files 
     ${TEST_OUTPUT_PATH}/outputSpinePhantomPartialSurfaceContactWithClipRegion.mha
     ${TestDataDir}/SpinePhantomPartialSurfaceContactWithClipRegionBaseline.mha
    )
  SET_TESTS_PROPERTIES(DrawClipRegionCompareToBaselineTest PROPERTIES DEPENDS DrawClipRegionRunTest)
ENDIF()