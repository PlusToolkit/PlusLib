SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------

#***************************  TrackingTest ***************************
ADD_EXECUTABLE(TrackingTest TrackingTest.cxx )
TARGET_LINK_LIBRARIES(TrackingTest vtkDataCollection vtkPlusCommon )
GENERATE_HELP_DOC(TrackingTest)

#***************************  TimestampFilteringTest ***************************
ADD_EXECUTABLE(TimestampFilteringTest TimestampFilteringTest.cxx )
TARGET_LINK_LIBRARIES(TimestampFilteringTest vtkPlusCommon vtkDataCollection )

ADD_TEST(TimestampFilteringTest 
  ${EXECUTABLE_OUTPUT_PATH}/TimestampFilteringTest
  --source-seq-file=${TestDataDir}/TimestampFilteringTest.mha 
  --averaged-items-for-filtering=20
  --max-timestamp-difference=0.08
  --min-stdev-reduction-factor=3.0
  --transform=IdentityToIdentityTransform
  )
SET_TESTS_PROPERTIES( TimestampFilteringTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  vtkDataCollectorTest1 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest1 vtkDataCollectorTest1.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest1 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SavedDataset.xml 
  --video-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
  --tracker-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer-trimmed.mha
  --rendering-off
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

IF (PLUS_USE_ULTRASONIX_VIDEO AND PLUS_TEST_ULTRASONIX)
  ADD_TEST(vtkDataCollectorTest1_SonixVideo
    ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SonixVideo_FakeTracker.xml 
    --rendering-off
    --sonix-ip=${PLUS_TEST_ULTRASONIX_IP_ADDRESS}
    )
  SET_TESTS_PROPERTIES( vtkDataCollectorTest1_SonixVideo PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO AND PLUS_TEST_ULTRASONIX)
 
#***************************  vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest2 vtkDataCollectorTest2.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest2 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest2 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest2
  --video-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.mha
  --tracker-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer-trimmed.mha
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SavedDataset.xml 
  --acq-time-length=5
  --verbose=3
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  vtk3DDataCollectorTest1 ***************************
IF (PLUS_USE_MMF_VIDEO AND PLUS_TEST_MMF_VIDEO)
ADD_EXECUTABLE(vtk3DDataCollectorTest1 vtk3DDataCollectorTest1.cxx)
TARGET_LINK_LIBRARIES(vtk3DDataCollectorTest1 vtkDataCollection )
ADD_TEST(vtk3DDataCollectorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtk3DDataCollectorTest1
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_3DSavedDataset.xml
  --minimum=0 
  --maximum=255 
  --mean=25.243336787840136 
  --median=0 
  --standard-deviation=57.955927672415285
  --xDimension=112
  --yDimension=112
  --zDimension=48
  --verbose=3
  )
SET_TESTS_PROPERTIES( vtk3DDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF(PLUS_USE_MMF_VIDEO AND PLUS_TEST_MMF_VIDEO)

#***************************  vtkDataCollectorVideoAcqTest ***************************
ADD_EXECUTABLE(vtkDataCollectorVideoAcqTest vtkDataCollectorVideoAcqTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorVideoAcqTest vtkDataCollection )
# IF (PLUS_USE_ULTRASONIX_VIDEO)
  # ADD_TEST(vtkDataCollectorVideoAcqTest 
    # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorVideoAcqTest
    # --config-file=${ConfigFilesDir}/USDataCollectionConfig_TrackerNone.xml 
    # --verbose=2
    # )
    # SET_TESTS_PROPERTIES( vtkDataCollectorVideoAcqTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
# ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

#***************************  vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE( ReplayRecordedDataTest
  ReplayRecordedDataTest.cxx
  )
TARGET_LINK_LIBRARIES( ReplayRecordedDataTest vtkDataCollection ${VTK_LIBRARIES} )
ADD_TEST( ReplayRecordedDataTest
  ${EXECUTABLE_OUTPUT_PATH}/ReplayRecordedDataTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestServer.xml
  )
SET_TESTS_PROPERTIES( ReplayRecordedDataTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR" )

#***************************  vtkDataCollectorFileTest ***************************
ADD_EXECUTABLE(vtkDataCollectorFileTest vtkDataCollectorFileTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorFileTest vtkDataCollection )
ADD_TEST(vtkDataCollectorFileTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorFileTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_File.xml 
  )
SET_TESTS_PROPERTIES( vtkDataCollectorFileTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusVersion PlusVersion.cxx )
TARGET_LINK_LIBRARIES(PlusVersion vtkPlusCommon vtkDataCollection)
GENERATE_HELP_DOC(PlusVersion)

ADD_TEST(PlusVersion 
  ${EXECUTABLE_OUTPUT_PATH}/PlusVersion
  )

#***************************  vtkMetaImageSequenceIOTest  ***************************
ADD_EXECUTABLE(vtkMetaImageSequenceIOTest vtkMetaImageSequenceIOTest.cxx )
TARGET_LINK_LIBRARIES(vtkMetaImageSequenceIOTest vtkDataCollection )
ADD_TEST(vtkMetaImageSequenceIOTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkMetaImageSequenceIOTest
  --img-seq-file=${TestDataDir}/MetaImageSequenceIOTest1.mhd
  --output-img-seq-file=MetaImageSequenceIOTest1Output.mha
  )
SET_TESTS_PROPERTIES( vtkMetaImageSequenceIOTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
#***************************  ViewSeqMetaFile  ***************************
ADD_EXECUTABLE(ViewSeqMetaFile ViewSeqMetaFile.cxx )
TARGET_LINK_LIBRARIES(ViewSeqMetaFile vtkDataCollection )
GENERATE_HELP_DOC(ViewSeqMetaFile)

ADD_TEST(ViewSeqMetaFileTest
  ${EXECUTABLE_OUTPUT_PATH}/ViewSeqMetaFile
  --source-seq-file=${TestDataDir}/SpinePhantomFreehand.mha
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SpinePhantomFreehandReconstructionOnly.xml
  --image-to-reference-transform=ImageToTracker
  --rendering-off
  )
SET_TESTS_PROPERTIES( ViewSeqMetaFileTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  itkFcsvReaderTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvReaderTest1 itkFcsvReaderTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvReaderTest1 vtkDataCollection )
ADD_TEST(itkFcsvReaderTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvReaderTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv 
  )
SET_TESTS_PROPERTIES( itkFcsvReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  itkFcsvWriterTest1.cxx ***************************
ADD_EXECUTABLE(itkFcsvWriterTest1 itkFcsvWriterTest1.cxx )
TARGET_LINK_LIBRARIES(itkFcsvWriterTest1 vtkDataCollection )
ADD_TEST(itkFcsvWriterTest1 ${EXECUTABLE_OUTPUT_PATH}/itkFcsvWriterTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv ${CMAKE_CURRENT_BINARY_DIR}/FcsvWriterTest1.fcsv
  )
SET_TESTS_PROPERTIES( itkFcsvWriterTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  vtkSonixVideoSourceTest1.cxx ***************************
IF (PLUS_USE_ULTRASONIX_VIDEO AND PLUS_TEST_ULTRASONIX)
  ADD_EXECUTABLE(vtkSonixVideoSourceTest1 vtkSonixVideoSourceTest1.cxx )
  TARGET_LINK_LIBRARIES(vtkSonixVideoSourceTest1 vtkDataCollection vtkPlusCommon)
  ADD_TEST(vtkSonixVideoSourceTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkSonixVideoSourceTest1
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SonixVideoSourceTest.xml
    --sonix-ip=${PLUS_TEST_ULTRASONIX_IP_ADDRESS}
  )
  SET_TESTS_PROPERTIES( vtkSonixVideoSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO AND PLUS_TEST_ULTRASONIX)

#***************************  vtkPhilips3DProbeVideoSourceTest1.cxx ***************************
IF (PLUS_USE_PHILIPS_3D_ULTRASOUND AND PLUS_TEST_PHILIPS_3D_ULTRASOUND)
  ADD_EXECUTABLE(vtkPhilips3DProbeVideoSourceTest1 vtkPhilips3DProbeVideoSourceTest1.cxx )
  TARGET_LINK_LIBRARIES(vtkPhilips3DProbeVideoSourceTest1 vtkDataCollection vtkPlusCommon)
  ADD_TEST(vtkPhilips3DProbeVideoSourceTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPhilips3DProbeVideoSourceTest1
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_PhilipsVideoSourceTest.xml
    --philips-ip=${PLUS_TEST_PHILIPS_3D_ULTRASOUND_IP_ADDRESS}
	--verbose=3
  )
  SET_TESTS_PROPERTIES( vtkPhilips3DProbeVideoSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_PHILIPS_3D_ULTRASOUND AND PLUS_TEST_PHILIPS_3D_ULTRASOUND)

#***************************  vtkIntersonSDKCxxVideoSourceTest.cxx ***************************
IF (PLUS_USE_INTERSONSDKCXX_VIDEO)
  ADD_EXECUTABLE(vtkIntersonSDKCxxVideoSourceTest vtkintersonSDKCxxVideoSourceTest.cxx)
  TARGET_LINK_LIBRARIES(vtkIntersonSDKCxxVideoSourceTest vtkDataCollection vtkPlusCommon)
  ADD_TEST(NAME vtkIntersonSDKCxxVideoSourceTest 
    COMMAND $<TARGET_FILE:vtkIntersonSDKCxxVideoSourceTest>
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonSDKCxxVideoSourceTest.xml
  )
  ADD_TEST(NAME vtkIntersonSDKCxxVideoSourceScanConvertTest 
    COMMAND $<TARGET_FILE:vtkIntersonSDKCxxVideoSourceTest>
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonSDKCxxVideoSourceScanConvertTest.xml
  )
  ADD_TEST(NAME vtkIntersonSDKCxxVideoSourceRfTest 
    COMMAND $<TARGET_FILE:vtkIntersonSDKCxxVideoSourceTest>
    --rendering-off 
    --acq-mode=RF
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonSDKCxxVideoSourceRfTest.xml
  )
  ADD_TEST(NAME vtkIntersonSDKCxxVideoSourceRfBmodeTest 
    COMMAND $<TARGET_FILE:vtkIntersonSDKCxxVideoSourceTest>
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonSDKCxxVideoSourceRfBmodeTest.xml
  )
  ADD_TEST(NAME vtkIntersonSDKCxxVideoSourceRfDecimationBmodeTest 
    COMMAND $<TARGET_FILE:vtkIntersonSDKCxxVideoSourceTest>
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonSDKCxxVideoSourceRfDecimationBmodeTest.xml
  )
  SET_TESTS_PROPERTIES( vtkIntersonSDKCxxVideoSourceTest
    vtkIntersonSDKCxxVideoSourceScanConvertTest
    vtkIntersonSDKCxxVideoSourceRfTest
    vtkIntersonSDKCxxVideoSourceRfBmodeTest
    vtkIntersonSDKCxxVideoSourceRfDecimationBmodeTest
  PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF ()

IF (PLUS_USE_BKPROFOCUS_VIDEO)

  #***************************  vtkBkProFocusOemVideoSourceTest.cxx ***************************
  ADD_EXECUTABLE(vtkBkProFocusOemVideoSourceTest vtkBkProFocusOemVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkBkProFocusOemVideoSourceTest vtkDataCollection vtkPlusCommon)
  IF (PLUS_TEST_BKPROFOCUS)
    ADD_TEST(vtkBkProFocusOemVideoSourceTest 
      ${EXECUTABLE_OUTPUT_PATH}/vtkBkProFocusOemVideoSourceTest
      --rendering-off
      )
    SET_TESTS_PROPERTIES( vtkBkProFocusOemVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
  ENDIF (PLUS_TEST_BKPROFOCUS)

  #***************************  vtkBkProFocusCameraLinkVideoSourceTest.cxx ***************************
  IF (PLUS_USE_BKPROFOCUS_CAMERALINK)
    ADD_EXECUTABLE(vtkBkProFocusCameraLinkVideoSourceTest vtkBkProFocusCameraLinkVideoSourceTest.cxx )
    TARGET_LINK_LIBRARIES(vtkBkProFocusCameraLinkVideoSourceTest vtkDataCollection vtkPlusCommon)
    IF (PLUS_TEST_BKPROFOCUS)
      ADD_TEST(vtkBkProFocusCameraLinkVideoSourceTest 
        ${EXECUTABLE_OUTPUT_PATH}/vtkBkProFocusCameraLinkVideoSourceTest
        --rendering-off
        )
      SET_TESTS_PROPERTIES( vtkBkProFocusCameraLinkVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
    ENDIF (PLUS_TEST_BKPROFOCUS)
  ENDIF (PLUS_USE_BKPROFOCUS_CAMERALINK)
  
ENDIF (PLUS_USE_BKPROFOCUS_VIDEO)

#***************************  vtkICCapturingSourceTest1.cxx ***************************
IF (PLUS_USE_ICCAPTURING_VIDEO)
ADD_EXECUTABLE(vtkICCapturingSourceTest1 vtkICCapturingSourceTest1.cxx )
  TARGET_LINK_LIBRARIES(vtkICCapturingSourceTest1 vtkDataCollection )
  # ADD_TEST(vtkICCapturingSourceTest1 
  # ${EXECUTABLE_OUTPUT_PATH}/vtkICCapturingSourceTest1
   # --rendering-off
  # )
  #SET_TESTS_PROPERTIES( vtkICCapturingSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ICCAPTURING_VIDEO)

#***************************  vtkSonixVolumeReaderTest1.cxx ***************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
  ADD_EXECUTABLE(vtkSonixVolumeReaderTest1 vtkSonixVolumeReaderTest1.cxx )
  TARGET_LINK_LIBRARIES(vtkSonixVolumeReaderTest1 vtkDataCollection )
  ADD_TEST(vtkSonixVolumeReaderTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkSonixVolumeReaderTest1
  --volume-file=${TestDataDir}/UltrasonixVolume.b8
  --baseline=${TestDataDir}/UltrasonixFrame5Baseline.tiff
  --frame-number=5 
  )
  SET_TESTS_PROPERTIES( vtkSonixVolumeReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)

#***************************  vtkCapistranoVideoSourceTest.cxx ***************************
IF (PLUS_USE_CAPISTRANO_VIDEO)
  ADD_EXECUTABLE(vtkCapistranoVideoSourceTest vtkCapistranoVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkCapistranoVideoSourceTest vtkDataCollection ) 
  ADD_TEST(NAME vtkCapistranoVideoSourceTest 
    COMMAND $<TARGET_FILE:vtkCapistranoVideoSourceTest>
    --rendering-off 
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_CapistranoVideoSourceTest.xml
  )
ENDIF (PLUS_USE_CAPISTRANO_VIDEO)

#***************************  vtkIntersonVideoSourceTest.cxx ***************************
IF (PLUS_USE_INTERSON_VIDEO)
  ADD_EXECUTABLE(vtkIntersonVideoSourceTest vtkIntersonVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkIntersonVideoSourceTest vtkDataCollection ) 
ENDIF (PLUS_USE_INTERSON_VIDEO)

#***************************  vtkTelemedVideoSourceTest.cxx ***************************
IF (PLUS_USE_TELEMED_VIDEO)
  ADD_EXECUTABLE(vtkTelemedVideoSourceTest vtkTelemedVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkTelemedVideoSourceTest vtkDataCollection )
  ADD_TEST(vtkTelemedVideoSourceTest
    ${EXECUTABLE_OUTPUT_PATH}/vtkTelemedVideoSourceTest
    #--rendering-off
    )
  SET_TESTS_PROPERTIES( vtkTelemedVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_TELEMED_VIDEO)

#***************************  vtkThorLabsVideoSourceTest.cxx ***************************
IF (PLUS_USE_THORLABS_VIDEO)
  ADD_EXECUTABLE(vtkThorLabsVideoSourceTest vtkThorLabsVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkThorLabsVideoSourceTest vtkDataCollection )
  ADD_TEST(vtkThorLabsVideoSourceTest
    ${EXECUTABLE_OUTPUT_PATH}/vtkThorLabsVideoSourceTest
    --rendering-off
    )
  SET_TESTS_PROPERTIES( vtkThorLabsVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_THORLABS_VIDEO)

#************************  vtkSonixPortaVideoSourceTest.cxx ************************
IF (PLUS_USE_ULTRASONIX_VIDEO)
  ADD_EXECUTABLE(vtkSonixPortaVideoSourceTest vtkSonixPortaVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkSonixPortaVideoSourceTest vtkDataCollection vtkPlusCommon)
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
  TARGET_LINK_LIBRARIES(vtkURFSavedVideoSourceTest1 vtkDataCollection )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)
#Test is to be added.

#***************************  vtkWin32VideoSourceTest.cxx ***************************
IF (PLUS_USE_VFW_VIDEO)
  ADD_EXECUTABLE(vtkWin32VideoSourceTest vtkWin32VideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkWin32VideoSourceTest vtkDataCollection )
ENDIF (PLUS_USE_VFW_VIDEO)

#***************************  vtkMmfVideoSourceTest.cxx ***************************
IF (PLUS_USE_MMF_VIDEO)
  ADD_EXECUTABLE(vtkMmfVideoSourceTest vtkMmfVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkMmfVideoSourceTest vtkDataCollection )

  IF (PLUS_TEST_MMF_VIDEO)
    ADD_TEST(MmfVideoSourceTest 
      ${EXECUTABLE_OUTPUT_PATH}/vtkMmfVideoSourceTest
      --rendering-off
      --frame-size 640 480
      )
  ENDIF(PLUS_TEST_MMF_VIDEO)

ENDIF (PLUS_USE_MMF_VIDEO)

#***************************  vtkEpiphanVideoSourceTest.cxx ***************************
IF (PLUS_USE_EPIPHAN)
  ADD_EXECUTABLE(vtkEpiphanVideoSourceTest vtkEpiphanVideoSourceTest.cxx )
  TARGET_LINK_LIBRARIES(vtkEpiphanVideoSourceTest vtkDataCollection )
ENDIF (PLUS_USE_EPIPHAN)

#*************************** MicronTrackerTest ***************************
IF (PLUS_USE_MICRONTRACKER)
    ADD_EXECUTABLE(vtkMicronTrackerTest vtkMicronTrackerTest.cxx)
    TARGET_LINK_LIBRARIES(vtkMicronTrackerTest vtkDataCollection vtkPlusCommon)    
ENDIF (PLUS_USE_MICRONTRACKER) 

#*************************** vtkPlusDeviceFactoryTest ***************************
ADD_EXECUTABLE(vtkPlusDeviceFactoryTest vtkPlusDeviceFactoryTest.cxx)
TARGET_LINK_LIBRARIES(vtkPlusDeviceFactoryTest vtkDataCollection vtkPlusCommon)
ADD_TEST(vtkPlusDeviceFactoryTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPlusDeviceFactoryTest
  )
# output is not checked for errors and warnings, as some error logs are expected

#*************************** NDICertusTest ***************************
IF (PLUS_USE_CERTUS)
  ADD_EXECUTABLE(NDICertusTest NDICertusTest.cxx)
  TARGET_LINK_LIBRARIES(NDICertusTest vtkDataCollection)
ENDIF (PLUS_USE_CERTUS)

#*************************** CmsBrachyStepperTest ***************************
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(CmsBrachyStepperTest CmsBrachyStepperTest.cxx)
    TARGET_LINK_LIBRARIES(CmsBrachyStepperTest vtkDataCollection ${VTK_LIBRARIES} vtkPlusCommon)
ENDIF (PLUS_USE_BRACHY_TRACKER) 

#*************************** CivcoBrachyStepperTest ***************************
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(CivcoBrachyStepperTest CivcoBrachyStepperTest.cxx)
    TARGET_LINK_LIBRARIES(CivcoBrachyStepperTest vtkDataCollection ${VTK_LIBRARIES} vtkPlusCommon)
ENDIF (PLUS_USE_BRACHY_TRACKER) 
  
#*************************** TransformInterpolationTest ***************************
ADD_EXECUTABLE(TransformInterpolationTest TransformInterpolationTest.cxx)
TARGET_LINK_LIBRARIES(TransformInterpolationTest vtkPlusCommon vtkDataCollection)

ADD_TEST(TransformInterpolationTest 
  ${EXECUTABLE_OUTPUT_PATH}/TransformInterpolationTest
  --source-seq-file=${TestDataDir}/TransformInterpolationTest.mha 
  --transform=ProbeToTracker
  --max-rotation-difference=1.0
  --max-translation-difference=0.5
  )

# OpenIGTLink 
IF( PLUS_USE_OpenIGTLink )

  ADD_EXECUTABLE(BrainLabTrackerSim BrainLabTrackerSim.cxx)
  TARGET_LINK_LIBRARIES(BrainLabTrackerSim OpenIGTLink)
  
ENDIF( PLUS_USE_OpenIGTLink )
  
  
# --------------------------------------------------------------------------
# Install
#

INSTALL(
  TARGETS 
    PlusVersion 
    ViewSeqMetaFile
    TrackingTest
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
