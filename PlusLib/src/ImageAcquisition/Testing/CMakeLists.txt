SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )

#***************************  vtkMetaImageSequenceIOTest  ***************************
ADD_EXECUTABLE(vtkMetaImageSequenceIOTest vtkMetaImageSequenceIOTest.cxx )
TARGET_LINK_LIBRARIES(vtkMetaImageSequenceIOTest vtkImageAcquisition )
ADD_TEST(vtkMetaImageSequenceIOTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkMetaImageSequenceIOTest
  --input-img-seq-file-name=${TestDataDir}/MetaImageSequenceIOTest1.mhd
  --output-img-seq-file-name=_MetaImageSequenceIOTest1.mha
  )
SET_TESTS_PROPERTIES( vtkMetaImageSequenceIOTest PROPERTIES FAIL_REGULAR_EXPRESSION "WARNING" )
 
#***************************  MetaImageSequenceViewer  ***************************
ADD_EXECUTABLE(MetaImageSequenceViewer MetaImageSequenceViewer.cxx )
TARGET_LINK_LIBRARIES(MetaImageSequenceViewer vtkImageAcquisition )
ADD_TEST(MetaImageSequenceViewerTest
  ${EXECUTABLE_OUTPUT_PATH}/MetaImageSequenceViewer
  --input-img-seq-file-name=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
  --input-transform-name=TemplateToStepperHomeTransform
  --rendering-off
  )
SET_TESTS_PROPERTIES( MetaImageSequenceViewerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  itkFcsvReaderTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvReaderTest1 itkFcsvReaderTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvReaderTest1 vtkImageAcquisition )
ADD_TEST(itkFcsvReaderTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvReaderTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv 
  )
SET_TESTS_PROPERTIES( itkFcsvReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  itkFcsvWriterTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvWriterTest1 itkFcsvWriterTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvWriterTest1 vtkImageAcquisition )
ADD_TEST(itkFcsvWriterTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvWriterTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv ${CMAKE_CURRENT_BINARY_DIR}/FcsvWriterTest1.fcsv
  )
SET_TESTS_PROPERTIES( itkFcsvWriterTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  vtkSonixVideoSourceTest1.cxx ***************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
	ADD_EXECUTABLE(vtkSonixVideoSourceTest1 vtkSonixVideoSourceTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkSonixVideoSourceTest1 vtkImageAcquisition vtkCharts)
	ADD_TEST(vtkSonixVideoSourceTest1 
	${EXECUTABLE_OUTPUT_PATH}/vtkSonixVideoSourceTest1
	 --rendering-off
     --sonix-ip=130.15.7.20
	)
	SET_TESTS_PROPERTIES( vtkSonixVideoSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

#***************************  vtkICCapturingSourceTest1.cxx ***************************
IF (PLUS_USE_ICCAPTURING_VIDEO)
ADD_EXECUTABLE(vtkICCapturingSourceTest1 vtkICCapturingSourceTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkICCapturingSourceTest1 vtkImageAcquisition )
	# ADD_TEST(vtkICCapturingSourceTest1 
	# ${EXECUTABLE_OUTPUT_PATH}/vtkICCapturingSourceTest1
	 # --rendering-off
	# )
	#SET_TESTS_PROPERTIES( vtkICCapturingSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ICCAPTURING_VIDEO)

#***************************  vtkSonixVolumeReaderTest1.cxx ***************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
	ADD_EXECUTABLE(vtkSonixVolumeReaderTest1 vtkSonixVolumeReaderTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkSonixVolumeReaderTest1 vtkImageAcquisition )
	ADD_TEST(vtkSonixVolumeReaderTest1 
	${EXECUTABLE_OUTPUT_PATH}/vtkSonixVolumeReaderTest1
	--input-file-name=${TestDataDir}/UltrasonixVolume.b8
	--input-baseline-name=${TestDataDir}/UltrasonixFrame5Baseline.tiff
	--input-frame-number=5 
	)
	SET_TESTS_PROPERTIES( vtkSonixVolumeReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

#************************  vtkSonixPortaVideoSourceTest.cxx ************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
	ADD_EXECUTABLE(vtkSonixPortaVideoSourceTest vtkSonixPortaVideoSourceTest.cxx )
	TARGET_LINK_LIBRARIES(vtkSonixPortaVideoSourceTest vtkImageAcquisition vtkCharts)
	# ADD_TEST(vtkSonixPortaVideoSourceTest 
	# ${EXECUTABLE_OUTPUT_PATH}/vtkSonixPortaVideoSourceTest
	# To be added if the 3D probe is attached to the machine
	# --setting-path=${ULTRASONIX_SDK_DIR}/porta/dat/
	# --license-path=D:/Ultrasonix Settings/licenses.txt
	# --firmware-path=${ULTRASONIX_SDK_DIR}/porta/fw/
	# --lut-path=C:/luts
	# --rendering-off
	# )
	# SET_TESTS_PROPERTIES( vtkSonixPortaVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

#***************************  vtkURFSavedVideoSourceTest1.cxx ***************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
	ADD_EXECUTABLE(vtkURFSavedVideoSourceTest1 vtkURFSavedVideoSourceTest1.cxx )
	TARGET_LINK_LIBRARIES(vtkURFSavedVideoSourceTest1 vtkImageAcquisition )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)
#Test is to be added.

#***************************  vtkURFSavedVideoSourceTest1.cxx ***************************
IF (PLUS_USE_VFW_VIDEO)
	ADD_EXECUTABLE(vtkWin32VideoSourceTest vtkWin32VideoSourceTest.cxx )
	TARGET_LINK_LIBRARIES(vtkWin32VideoSourceTest vtkImageAcquisition )
ENDIF (PLUS_USE_VFW_VIDEO)


#*************************** vtkPlusVideoSourceFactoryTest ***************************
ADD_EXECUTABLE(vtkPlusVideoSourceFactoryTest vtkPlusVideoSourceFactoryTest.cxx)
TARGET_LINK_LIBRARIES(vtkPlusVideoSourceFactoryTest vtkImageAcquisition vtkPlusCommon)
ADD_TEST(vtkPlusVideoSourceFactoryTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPlusVideoSourceFactoryTest
  )
SET_TESTS_PROPERTIES( vtkPlusVideoSourceFactoryTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR" )

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS MetaImageSequenceViewer
  DESTINATION bin
  COMPONENT RuntimeLibraries
  )
