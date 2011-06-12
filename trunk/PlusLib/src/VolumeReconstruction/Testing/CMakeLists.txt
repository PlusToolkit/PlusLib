SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#***************************  vtkVolumeReconstructorTest1  ***************************
ADD_EXECUTABLE(vtkVolumeReconstructorTest1 vtkVolumeReconstructorTest1.cxx )
TARGET_LINK_LIBRARIES(vtkVolumeReconstructorTest1 vtkVolumeReconstruction )
ADD_TEST(vtkVolumeReconstructorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkVolumeReconstructorTest1
  --input-config-file-name=${ConfigFilesDir}/VolRec_SonixRP_TRUS_D70mm.xml 
  --input-img-seq-file-name=${TestDataDir}/USTC_Ulterius_ProbeTranslationData.mha
  --output-volume-file-name=_GeneratedUsVolume.vtk
  )
SET_TESTS_PROPERTIES( vtkVolumeReconstructorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
