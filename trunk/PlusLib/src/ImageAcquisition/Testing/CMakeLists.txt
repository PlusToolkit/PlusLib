SET( TestDataDir ${PLUS_DATA_DIR}/TestImages )

#***************************  itkMetaImageSequenceIOTest1  ***************************
ADD_EXECUTABLE(itkMetaImageSequenceIOTest1 itkMetaImageSequenceIOTest1.cxx )
TARGET_LINK_LIBRARIES(itkMetaImageSequenceIOTest1 vtkImageAcquisition )
ADD_TEST(itkMetaImageSequenceIOTest1 
  ${EXECUTABLE_OUTPUT_PATH}/itkMetaImageSequenceIOTest1
  --input-test-data-dir=${TestDataDir}
  --input-img-seq-file-name=MetaImageSequenceIOTest1.mhd
  --output-img-seq-file-name=_MetaImageSequenceIOTest1.mhd
  )

#***************************  itkMetaImageSequenceIOTest2  ***************************
ADD_EXECUTABLE(itkMetaImageSequenceIOTest2 itkMetaImageSequenceIOTest2.cxx )
TARGET_LINK_LIBRARIES(itkMetaImageSequenceIOTest2 vtkImageAcquisition )
ADD_TEST(itkMetaImageSequenceIOTest2 
  ${EXECUTABLE_OUTPUT_PATH}/itkMetaImageSequenceIOTest2
  --input-img-seq-file-names ${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
  )
  
#***************************  itkMetaImageSequenceIOTest3  ***************************
ADD_EXECUTABLE(itkMetaImageSequenceIOTest3 itkMetaImageSequenceIOTest3.cxx )
TARGET_LINK_LIBRARIES(itkMetaImageSequenceIOTest3 vtkImageAcquisition )
ADD_TEST(itkMetaImageSequenceIOTest3
  ${EXECUTABLE_OUTPUT_PATH}/itkMetaImageSequenceIOTest3
  --input-img-seq-file-name=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
  --rendering-off
  )

#***************************  itkMetaImageSequenceIOTest4  ***************************
ADD_EXECUTABLE(itkMetaImageSequenceIOTest4 itkMetaImageSequenceIOTest4.cxx )
TARGET_LINK_LIBRARIES(itkMetaImageSequenceIOTest4 vtkImageAcquisition )
ADD_TEST(itkMetaImageSequenceIOTest4
  ${EXECUTABLE_OUTPUT_PATH}/itkMetaImageSequenceIOTest4
  --output-img-seq-file-name=GeneratedUsSeqMetafile.mhd
  )

#***************************  itkFcsvReaderTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvReaderTest1 itkFcsvReaderTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvReaderTest1 vtkImageAcquisition )
ADD_TEST(itkFcsvReaderTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvReaderTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv 
  )

#***************************  itkFcsvWriterTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvWriterTest1 itkFcsvWriterTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvWriterTest1 vtkImageAcquisition )
ADD_TEST(itkFcsvWriterTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvWriterTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv ${TestDataDir}/FcsvWriterTest1.fcsv
  )

#***************************  vtkSonixVideoSourceTest1.cxx ***************************
IF (PLUS_USE_SONIX_VIDEO)
	ADD_EXECUTABLE(vtkSonixVideoSourceTest1 vtkSonixVideoSourceTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkSonixVideoSourceTest1 vtkImageAcquisition )
	ADD_TEST(vtkSonixVideoSourceTest1 
	${EXECUTABLE_OUTPUT_PATH}/vtkSonixVideoSourceTest1
	 --rendering-off
     --sonix-ip=130.15.7.20
	)
ENDIF (PLUS_USE_SONIX_VIDEO)

#***************************  vtkICCapturingSourceTest1.cxx ***************************
IF (PLUS_USE_ICCAPTURING_VIDEO)
ADD_EXECUTABLE(vtkICCapturingSourceTest1 vtkICCapturingSourceTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkICCapturingSourceTest1 vtkImageAcquisition )
	# ADD_TEST(vtkICCapturingSourceTest1 
	# ${EXECUTABLE_OUTPUT_PATH}/vtkICCapturingSourceTest1
	 # --rendering-off
	# )
ENDIF (PLUS_USE_ICCAPTURING_VIDEO)

#***************************  vtkSonixVolumeReaderTest1.cxx ***************************
IF (PLUS_USE_SONIX_VIDEO)
	ADD_EXECUTABLE(vtkSonixVolumeReaderTest1 vtkSonixVolumeReaderTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkSonixVolumeReaderTest1 vtkImageAcquisition )
	ADD_TEST(vtkSonixVolumeReaderTest1 
	${EXECUTABLE_OUTPUT_PATH}/vtkSonixVolumeReaderTest1
	--input-file-name=${TestDataDir}/UltrasonixVolume.b8
	--input-baseline-name=${TestDataDir}/UltrasonixFrame5Baseline.tiff
	--input-frame-number=5 
	)
ENDIF (PLUS_USE_SONIX_VIDEO)

