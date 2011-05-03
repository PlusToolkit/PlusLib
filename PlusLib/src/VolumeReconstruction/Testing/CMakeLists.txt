SET( TestDataDir ${PLUS_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUS_DATA_DIR}/ConfigFiles )

#***************************  vtkVolumeReconstructorTest1  ***************************
ADD_EXECUTABLE(vtkVolumeReconstructorTest1 vtkVolumeReconstructorTest1.cxx )
TARGET_LINK_LIBRARIES(vtkVolumeReconstructorTest1 vtkVolumeReconstruction )
ADD_TEST(vtkVolumeReconstructorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkVolumeReconstructorTest1
  --input-config-file-name=${ConfigFilesDir}/GeneratedUSVolumeReconstruction.xml 
  --input-img-seq-file-name=${TestDataDir}/GeneratedUsSeqMetafile.mhd 
  --output-volume-file-name=_GeneratedUsVolume.vtk
  )
