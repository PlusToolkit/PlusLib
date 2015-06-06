SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#***************************  VolumeReconstructor  ***************************
ADD_EXECUTABLE(VolumeReconstructor VolumeReconstructor.cxx )
TARGET_LINK_LIBRARIES(VolumeReconstructor vtkVolumeReconstruction )
GENERATE_HELP_DOC(VolumeReconstructor)

# Reconstruct volumes
ADD_TEST(vtkVolumeReconstructorTestRunNearLateUChar
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SonixRP_TRUS_D70mm_NN_LATE.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand.mha
  --output-volume-file=vtkVolumeReconstructorTestNNLATEvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunNearLateUChar PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


ADD_TEST(vtkVolumeReconstructorTestRunNearMeanUChar
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SpinePhantom_NN_MEAN.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand.mha
  --output-volume-file=vtkVolumeReconstructorTestNNMEANvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunNearMeanUChar PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


ADD_TEST(vtkVolumeReconstructorTestRunNearMaxiFloat
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SpinePhantom_NN_MAXI.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand3FramesFloat.mha
  --output-volume-file=vtkVolumeReconstructorTestNNMAXIvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunNearMaxiFloat PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


ADD_TEST(vtkVolumeReconstructorTestRunLinrLateFloat
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SpinePhantom_LN_LATE.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand3FramesFloat.mha
  --output-volume-file=vtkVolumeReconstructorTestLNLATEvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunLinrLateFloat PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


ADD_TEST(vtkVolumeReconstructorTestRunLinrMeanUChar
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SonixRP_TRUS_D70mm_LN_MEAN.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand.mha
  --output-volume-file=vtkVolumeReconstructorTestLNMEANvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunLinrMeanUChar PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


ADD_TEST(vtkVolumeReconstructorTestRunLinrMaxiUChar
  ${EXECUTABLE_OUTPUT_PATH}/VolumeReconstructor
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VolumeReconstructionOnly_SpinePhantom_LN_MAXI.xml
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand.mha
  --output-volume-file=vtkVolumeReconstructorTestLNMAXIvolume.mha
  --image-to-reference-transform=ImageToReference
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTestRunLinrMaxiUChar PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )


# Compare reconstructed volumes with the reference volumes
ADD_TEST(vtkVolumeReconstructorTestCompareNearLateUChar
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestNNLATEvolumeRef.mha
  vtkVolumeReconstructorTestNNLATEvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareNearLateUChar PROPERTIES DEPENDS vtkVolumeReconstructorTestRunNearLateUChar)


ADD_TEST(vtkVolumeReconstructorTestCompareNearMeanUChar
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestNNMEANvolumeRef.mha
  vtkVolumeReconstructorTestNNMEANvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareNearMeanUChar PROPERTIES DEPENDS vtkVolumeReconstructorTestRunNearMeanUChar)


ADD_TEST(vtkVolumeReconstructorTestCompareNearMaxiFloat
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestNNMAXIvolumeRef.mha
  vtkVolumeReconstructorTestNNMAXIvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareNearMaxiFloat PROPERTIES DEPENDS vtkVolumeReconstructorTestRunNearMaxiFloat)


ADD_TEST(vtkVolumeReconstructorTestCompareLinrLateFloat
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestLNLATEvolumeRef.mha
  vtkVolumeReconstructorTestLNLATEvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareLinrLateFloat PROPERTIES DEPENDS vtkVolumeReconstructorTestRunLinrLateFloat)


ADD_TEST(vtkVolumeReconstructorTestCompareLinrMeanUChar
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestLNMEANvolumeRef.mha
  vtkVolumeReconstructorTestLNMEANvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareLinrMeanUChar PROPERTIES DEPENDS vtkVolumeReconstructorTestRunLinrMeanUChar)


ADD_TEST(vtkVolumeReconstructorTestCompareLinrMaxiUChar
  ${CMAKE_COMMAND} -E compare_files 
  ${TestDataDir}/vtkVolumeReconstructorTestLNMAXIvolumeRef.mha
  vtkVolumeReconstructorTestLNMAXIvolume.mha
  )
SET_TESTS_PROPERTIES(vtkVolumeReconstructorTestCompareLinrMaxiUChar PROPERTIES DEPENDS vtkVolumeReconstructorTestRunLinrMaxiUChar)


#***************************  CreateSliceModels  ***************************
# This program helps debugging geometry problems in volume reconstruction.
ADD_EXECUTABLE( CreateSliceModels CreateSliceModels.cxx )
TARGET_LINK_LIBRARIES( CreateSliceModels vtkPlusCommon vtkVolumeReconstruction )
GENERATE_HELP_DOC(CreateSliceModels)

ADD_TEST(CreateSliceModelsTest 
  ${EXECUTABLE_OUTPUT_PATH}/CreateSliceModels
  --source-seq-file=${TestDataDir}/NwirePhantomFreehand.mha
  --output-model-file=GeneratedSliceModels.mha  
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_NwirePhantomFreehand_vtkVolumeReconstructorTest2.xml
  --image-to-reference-transform=ProbeToReference
  )
SET_TESTS_PROPERTIES( CreateSliceModelsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  CompareVolumes  ***************************
# This program compares two volumes and outputs difference statistics
ADD_EXECUTABLE( CompareVolumes CompareVolumes.cxx vtkCompareVolumes.cxx )
TARGET_LINK_LIBRARIES( CompareVolumes vtkPlusCommon )

#***************************  CompareVolumes  ***************************
# This program compares two volumes and outputs difference statistics
ADD_EXECUTABLE( DrawClipRegion DrawClipRegion.cxx )
TARGET_LINK_LIBRARIES( DrawClipRegion vtkPlusCommon vtkVolumeReconstruction)
GENERATE_HELP_DOC(DrawClipRegion)

# --------------------------------------------------------------------------
ADD_TEST(DrawClipRegionRunTest
  ${EXECUTABLE_OUTPUT_PATH}/DrawClipRegion
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

# --------------------------------------------------------------------------
# Install
#

INSTALL(
  TARGETS
    VolumeReconstructor
    CreateSliceModels
    DrawClipRegion
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
