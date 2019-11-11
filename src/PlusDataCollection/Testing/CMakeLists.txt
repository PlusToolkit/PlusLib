SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#*************************** TrackingTest ***************************
ADD_EXECUTABLE(TrackingTest TrackingTest.cxx )
SET_TARGET_PROPERTIES(TrackingTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(TrackingTest vtkPlusDataCollection vtkPlusCommon )
GENERATE_HELP_DOC(TrackingTest)

#*************************** TimestampFilteringTest ***************************
ADD_EXECUTABLE(TimestampFilteringTest TimestampFilteringTest.cxx )
SET_TARGET_PROPERTIES(TimestampFilteringTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(TimestampFilteringTest vtkPlusCommon vtkPlusDataCollection )

ADD_TEST(TimestampFilteringTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/TimestampFilteringTest
  --source-seq-file=${TestDataDir}/TimestampFilteringTest.igs.mha
  --averaged-items-for-filtering=20
  --max-timestamp-difference=0.08
  --min-stdev-reduction-factor=3.0
  --transform=IdentityToIdentityTransform
  )
SET_TESTS_PROPERTIES(TimestampFilteringTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#*************************** vtkDataCollectorTest1 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest1 vtkDataCollectorTest1.cxx)
SET_TARGET_PROPERTIES(vtkDataCollectorTest1 PROPERTIES FOLDER Tests)
SET(LIBRARIES vtkDataCollectorTest1 vtkPlusDataCollection)
IF(PLUS_RENDERING_ENABLED)
  LIST(APPEND LIBRARIES vtkInteractionImage)
ENDIF()
TARGET_LINK_LIBRARIES(${LIBRARIES})

ADD_TEST(vtkDataCollectorTest1
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SavedDataset.xml
  --video-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.igs.mha
  --tracker-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer-trimmed.igs.mha
  --rendering-off
  )
SET_TESTS_PROPERTIES(vtkDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

IF(PLUS_USE_ULTRASONIX_VIDEO AND PLUS_TEST_ULTRASONIX)
  ADD_TEST(vtkDataCollectorTest1_SonixVideo
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SonixVideo_FakeTracker.xml
    --rendering-off
    --sonix-ip=${PLUS_TEST_ULTRASONIX_IP_ADDRESS}
    )
  SET_TESTS_PROPERTIES(vtkDataCollectorTest1_SonixVideo PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest2 vtkDataCollectorTest2.cxx)
SET_TARGET_PROPERTIES(vtkDataCollectorTest2 PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest2 vtkPlusDataCollection )
ADD_TEST(vtkDataCollectorTest2
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest2
  --video-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationVideoBuffer.igs.mha
  --tracker-buffer-seq-file=${TestDataDir}/WaterTankBottomTranslationTrackerBuffer-trimmed.igs.mha
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_SavedDataset.xml
  --acq-time-length=5
  --verbose=3
  )
SET_TESTS_PROPERTIES(vtkDataCollectorTest2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#*************************** vtk3DDataCollectorTest1 ***************************
ADD_EXECUTABLE(vtk3DDataCollectorTest1 vtk3DDataCollectorTest1.cxx)
SET_TARGET_PROPERTIES(vtk3DDataCollectorTest1 PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtk3DDataCollectorTest1 vtkPlusDataCollection )
ADD_TEST(vtk3DDataCollectorTest1
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtk3DDataCollectorTest1
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_3DSavedDataset.xml
  --minimum=0
  --maximum=253
  --mean=8.08658
  --median=0
  --standard-deviation=21.6785
  --xDimension=112
  --yDimension=112
  --zDimension=48
  --verbose=3
  )
SET_TESTS_PROPERTIES(vtk3DDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#*************************** vtkDataCollectorVideoAcqTest ***************************
ADD_EXECUTABLE(vtkDataCollectorVideoAcqTest vtkDataCollectorVideoAcqTest.cxx)
SET_TARGET_PROPERTIES(vtkDataCollectorVideoAcqTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkDataCollectorVideoAcqTest vtkPlusDataCollection )
# IF(PLUS_USE_ULTRASONIX_VIDEO)
  # ADD_TEST(vtkDataCollectorVideoAcqTest
    # ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorVideoAcqTest
    # --config-file=${ConfigFilesDir}/USDataCollectionConfig_TrackerNone.xml
    # --verbose=2
    # )
    # SET_TESTS_PROPERTIES(vtkDataCollectorVideoAcqTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
# ENDIF()

#*************************** vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE( ReplayRecordedDataTest ReplayRecordedDataTest.cxx )
SET_TARGET_PROPERTIES( ReplayRecordedDataTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES( ReplayRecordedDataTest vtkPlusDataCollection )
ADD_TEST( ReplayRecordedDataTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/ReplayRecordedDataTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTestServer.xml
  )
SET_TESTS_PROPERTIES(ReplayRecordedDataTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR")

#*************************** vtkDataCollectorFileTest ***************************
ADD_EXECUTABLE(vtkDataCollectorFileTest vtkDataCollectorFileTest.cxx)
SET_TARGET_PROPERTIES(vtkDataCollectorFileTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkDataCollectorFileTest vtkPlusDataCollection )
ADD_TEST(vtkDataCollectorFileTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorFileTest
  --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_File.xml
  )
SET_TESTS_PROPERTIES(vtkDataCollectorFileTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#--------------------------------------------------------------------------------------------
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_TEST(PlusVersion
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/PlusVersion
    )
ENDIF()

#*************************** ViewSequenceFile  ***************************
IF(PLUSBUILD_BUILD_PlusLib_TOOLS)
  ADD_TEST(ViewSequenceFileTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/ViewSequenceFile
    --source-seq-file=${TestDataDir}/SpinePhantomFreehand.igs.mha
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SpinePhantomFreehandReconstructionOnly.xml
    --image-to-reference-transform=ImageToTracker
    --rendering-off
    )
  SET_TESTS_PROPERTIES(ViewSequenceFileTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkFcsvReaderTest1.cxx ***************************
ADD_EXECUTABLE(vtkFcsvReaderTest1 vtkFcsvReaderTest1.cxx )
SET_TARGET_PROPERTIES(vtkFcsvReaderTest1 PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkFcsvReaderTest1 vtkPlusDataCollection )
ADD_TEST(vtkFcsvReaderTest1 ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkFcsvReaderTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv
  )
SET_TESTS_PROPERTIES(vtkFcsvReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#*************************** vtkFcsvWriterTest1.cxx ***************************
ADD_EXECUTABLE(vtkFcsvWriterTest1 vtkFcsvWriterTest1.cxx )
SET_TARGET_PROPERTIES(vtkFcsvWriterTest1 PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkFcsvWriterTest1 vtkPlusDataCollection )
ADD_TEST(vtkFcsvWriterTest1 ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkFcsvWriterTest1
  ${TestDataDir}/FcsvReaderTest1.fcsv ${CMAKE_CURRENT_BINARY_DIR}/FcsvWriterTest1.fcsv
  )
SET_TESTS_PROPERTIES(vtkFcsvWriterTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")

#*************************** vtkSonixVideoSourceTest1.cxx ***************************
IF(PLUS_USE_ULTRASONIX_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkSonixVideoSourceTest1 vtkSonixVideoSourceTest1.cxx )
  SET_TARGET_PROPERTIES(vtkSonixVideoSourceTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkSonixVideoSourceTest1 vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)

  IF(PLUS_TEST_ULTRASONIX)
    ADD_TEST(vtkSonixVideoSourceTest1
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkSonixVideoSourceTest1
      --rendering-off
      --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_SonixVideoSourceTest.xml
      --sonix-ip=${PLUS_TEST_ULTRASONIX_IP_ADDRESS}
      )
    SET_TESTS_PROPERTIES(vtkSonixVideoSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ENDIF()
ENDIF()

#*************************** vtkPhilips3DProbeVideoSourceTest1.cxx ***************************
IF(PLUS_USE_PHILIPS_3D_ULTRASOUND)
  ADD_EXECUTABLE(vtkPhilips3DProbeVideoSourceTest1 vtkPhilips3DProbeVideoSourceTest1.cxx )
  SET_TARGET_PROPERTIES(vtkPhilips3DProbeVideoSourceTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkPhilips3DProbeVideoSourceTest1 vtkPlusDataCollection vtkPlusCommon)

  IF(PLUS_TEST_PHILIPS_3D_ULTRASOUND)
    ADD_TEST(vtkPhilips3DProbeVideoSourceTest1
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPhilips3DProbeVideoSourceTest1
      --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_PhilipsVideoSourceTest.xml
      --philips-ip=${PLUS_TEST_PHILIPS_3D_ULTRASOUND_IP_ADDRESS}
      --verbose=3
      )
    SET_TESTS_PROPERTIES(vtkPhilips3DProbeVideoSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ENDIF()
ENDIF()

#*************************** vtkIntersonSDKCxxVideoSourceTest.cxx ***************************
IF(PLUS_USE_INTERSONSDKCXX_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkIntersonSDKCxxVideoSourceTest vtkintersonSDKCxxVideoSourceTest.cxx)
  SET_TARGET_PROPERTIES(vtkIntersonSDKCxxVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkIntersonSDKCxxVideoSourceTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
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
  SET_TESTS_PROPERTIES(vtkIntersonSDKCxxVideoSourceTest
    vtkIntersonSDKCxxVideoSourceScanConvertTest
    vtkIntersonSDKCxxVideoSourceRfTest
    vtkIntersonSDKCxxVideoSourceRfBmodeTest
    vtkIntersonSDKCxxVideoSourceRfDecimationBmodeTest
  PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkIntersonArraySDKCxxVideoSourceTest.cxx ***************************
IF(PLUS_USE_INTERSONARRAYSDKCXX_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkIntersonArraySDKCxxVideoSourceTest vtkintersonArraySDKCxxVideoSourceTest.cxx)
  SET_TARGET_PROPERTIES(vtkIntersonArraySDKCxxVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkIntersonArraySDKCxxVideoSourceTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
  ADD_TEST(NAME vtkIntersonArraySDKCxxVideoSourceTest
    COMMAND $<TARGET_FILE:vtkIntersonArraySDKCxxVideoSourceTest>
    --rendering-off
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonArraySDKCxxVideoSourceTest.xml
    )
  ADD_TEST(NAME vtkIntersonArraySDKCxxVideoSourceScanConvertTest
    COMMAND $<TARGET_FILE:vtkIntersonArraySDKCxxVideoSourceTest>
    --rendering-off
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonArraySDKCxxVideoSourceScanConvertTest.xml
    )
  ADD_TEST(NAME vtkIntersonArraySDKCxxVideoSourceRfTest
    COMMAND $<TARGET_FILE:vtkIntersonArraySDKCxxVideoSourceTest>
    --rendering-off
    --acq-mode=RF
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonArraySDKCxxVideoSourceRfTest.xml
    )
  ADD_TEST(NAME vtkIntersonArraySDKCxxVideoSourceRfBmodeTest
    COMMAND $<TARGET_FILE:vtkIntersonArraySDKCxxVideoSourceTest>
    --rendering-off
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_IntersonArraySDKCxxVideoSourceRfBmodeTest.xml
    )
  SET_TESTS_PROPERTIES(vtkIntersonArraySDKCxxVideoSourceTest
    vtkIntersonArraySDKCxxVideoSourceScanConvertTest
    vtkIntersonArraySDKCxxVideoSourceRfTest
    vtkIntersonArraySDKCxxVideoSourceRfBmodeTest
  PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()


IF(PLUS_USE_BKPROFOCUS_VIDEO)
  #*************************** vtkBkProFocusOemVideoSourceTest.cxx ***************************
  ADD_EXECUTABLE(vtkBkProFocusOemVideoSourceTest vtkBkProFocusOemVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkBkProFocusOemVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkBkProFocusOemVideoSourceTest vtkPlusDataCollection vtkPlusCommon)

  IF(PLUS_TEST_BKPROFOCUS)
    ADD_TEST(vtkBkProFocusOemVideoSourceTest
      ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkBkProFocusOemVideoSourceTest
      --rendering-off
      )
    SET_TESTS_PROPERTIES(vtkBkProFocusOemVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ENDIF()

  #*************************** vtkBkProFocusCameraLinkVideoSourceTest.cxx ***************************
  IF(PLUS_USE_BKPROFOCUS_CAMERALINK)
    ADD_EXECUTABLE(vtkBkProFocusCameraLinkVideoSourceTest vtkBkProFocusCameraLinkVideoSourceTest.cxx )
    SET_TARGET_PROPERTIES(vtkBkProFocusCameraLinkVideoSourceTest PROPERTIES FOLDER Tests)
    TARGET_LINK_LIBRARIES(vtkBkProFocusCameraLinkVideoSourceTest vtkPlusDataCollection vtkPlusCommon)

    IF(PLUS_TEST_BKPROFOCUS)
      ADD_TEST(vtkBkProFocusCameraLinkVideoSourceTest
        ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkBkProFocusCameraLinkVideoSourceTest
        --rendering-off
        )
      SET_TESTS_PROPERTIES(vtkBkProFocusCameraLinkVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
    ENDIF()
  ENDIF()
ENDIF()

#*************************** vtkICCapturingSourceTest1.cxx ***************************
IF(PLUS_USE_ICCAPTURING_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkICCapturingSourceTest1 vtkICCapturingSourceTest1.cxx )
  SET_TARGET_PROPERTIES(vtkICCapturingSourceTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkICCapturingSourceTest1 vtkPlusDataCollection vtkInteractionImage)
  # ADD_TEST(vtkICCapturingSourceTest1 
  #   ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkICCapturingSourceTest1
  #   --rendering-off
  #   )
  #SET_TESTS_PROPERTIES(vtkICCapturingSourceTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkPlusClariusTest1 ***************************
IF(PLUS_USE_CLARIUS)
  ADD_EXECUTABLE(vtkPlusClariusTest1 vtkClariusTest.cxx)
  SET_TARGET_PROPERTIES(vtkPlusClariusTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkPlusClariusTest1 vtkPlusDataCollection) # add any other dependencies to the test executable

  ADD_TEST(vtkPlusClariusTest1
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusClariusTest1
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_Clarius.xml
    )
  SET_TESTS_PROPERTIES(vtkPlusClariusTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkSonixVolumeReaderTest1.cxx ***************************
IF(PLUS_USE_ULTRASONIX_VIDEO)
  ADD_EXECUTABLE(vtkSonixVolumeReaderTest1 vtkSonixVolumeReaderTest1.cxx )
  SET_TARGET_PROPERTIES(vtkSonixVolumeReaderTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkSonixVolumeReaderTest1 vtkPlusDataCollection )
  ADD_TEST(vtkSonixVolumeReaderTest1
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkSonixVolumeReaderTest1
    --volume-file=${TestDataDir}/UltrasonixVolume.b8
    --baseline=${TestDataDir}/UltrasonixFrame5Baseline.tiff
    --frame-number=5
    )
  SET_TESTS_PROPERTIES(vtkSonixVolumeReaderTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkCapistranoVideoSourceTest.cxx ***************************
IF(PLUS_USE_CAPISTRANO_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkCapistranoVideoSourceTest vtkCapistranoVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkCapistranoVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkCapistranoVideoSourceTest vtkPlusDataCollection vtkInteractionImage) 
  ADD_TEST(NAME vtkCapistranoVideoSourceTest 
    COMMAND $<TARGET_FILE:vtkCapistranoVideoSourceTest>
    --rendering-off
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_CapistranoVideoSourceTest.xml
    )
ENDIF()

#*************************** vtkWinProbeVideoSourceTest.cxx ***************************
IF(PLUS_USE_WINPROBE_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkWinProbeVideoSourceTest vtkWinProbeVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkWinProbeVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkWinProbeVideoSourceTest vtkPlusDataCollection vtkInteractionImage) 
  ADD_TEST(NAME vtkWinProbeVideoSourceTest 
    COMMAND $<TARGET_FILE:vtkWinProbeVideoSourceTest>
    --rendering-off
    --output-seq-file=${TEST_OUTPUT_PATH}/WinProbeTest.igs.nrrd
    --config-file=${ConfigFilesDir}/PlusDeviceSet_Server_WinProbe.xml
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
  SET_TESTS_PROPERTIES(vtkWinProbeVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkIntersonVideoSourceTest.cxx ***************************
IF(PLUS_USE_INTERSON_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkIntersonVideoSourceTest vtkIntersonVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkIntersonVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkIntersonVideoSourceTest vtkPlusDataCollection vtkInteractionImage) 
ENDIF()

#*************************** vtkTelemedVideoSourceTest.cxx ***************************
IF(PLUS_USE_TELEMED_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkTelemedVideoSourceTest vtkTelemedVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkTelemedVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkTelemedVideoSourceTest vtkPlusDataCollection vtkInteractionImage)

  IF(PLUS_TEST_TELEMED)
    ADD_TEST(vtkTelemedVideoSourceTest
      ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkTelemedVideoSourceTest
      --rendering-off
      )
    SET_TESTS_PROPERTIES(vtkTelemedVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
  ENDIF()
ENDIF()

#*************************** vtkThorLabsVideoSourceTest.cxx ***************************
IF(PLUS_USE_THORLABS_VIDEO)
  ADD_EXECUTABLE(vtkThorLabsVideoSourceTest vtkThorLabsVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkThorLabsVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkThorLabsVideoSourceTest vtkPlusDataCollection )
  ADD_TEST(vtkThorLabsVideoSourceTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkThorLabsVideoSourceTest
    --rendering-off
    )
  SET_TESTS_PROPERTIES(vtkThorLabsVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#************************ vtkSonixPortaVideoSourceTest.cxx ************************
IF(PLUS_USE_ULTRASONIX_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkSonixPortaVideoSourceTest vtkSonixPortaVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkSonixPortaVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkSonixPortaVideoSourceTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
  # ADD_TEST(vtkSonixPortaVideoSourceTest 
  # ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkSonixPortaVideoSourceTest
  # To be added if the 3D probe is attached to the machine
  # --setting-path=${ULTRASONIX_SDK_DIR}/porta/dat/
  # --license-path=D:/Ultrasonix Settings/licenses.txt
  # --firmware-path=${ULTRASONIX_SDK_DIR}/porta/fw/
  # --lut-path=C:/luts
  # --rendering-off
  # )
  # SET_TESTS_PROPERTIES(vtkSonixPortaVideoSourceTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

#*************************** vtkURFSavedVideoSourceTest1.cxx ***************************
IF(PLUS_USE_ULTRASONIX_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkURFSavedVideoSourceTest1 vtkURFSavedVideoSourceTest1.cxx )
  SET_TARGET_PROPERTIES(vtkURFSavedVideoSourceTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkURFSavedVideoSourceTest1 vtkPlusDataCollection vtkInteractionImage)
ENDIF()
#Test is to be added.

#*************************** vtkWin32VideoSourceTest.cxx ***************************
IF(PLUS_USE_VFW_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkWin32VideoSourceTest vtkWin32VideoSourceTest.cxx)
  SET_TARGET_PROPERTIES(vtkWin32VideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkWin32VideoSourceTest vtkPlusDataCollection vtkInteractionImage)
ENDIF()

#*************************** vtkMmfVideoSourceTest.cxx ***************************
IF(PLUS_USE_MMF_VIDEO AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkMmfVideoSourceTest vtkMmfVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkMmfVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkMmfVideoSourceTest vtkPlusDataCollection vtkInteractionImage)

  IF(PLUS_TEST_MMF_VIDEO)
    ADD_TEST(MmfVideoSourceTest 
      ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkMmfVideoSourceTest
      --rendering-off
      --frame-size 640 480
      )
  ENDIF()
ENDIF()

#*************************** vtkEpiphanVideoSourceTest.cxx ***************************
IF(PLUS_USE_EPIPHAN AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkEpiphanVideoSourceTest vtkEpiphanVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkEpiphanVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkEpiphanVideoSourceTest vtkPlusDataCollection vtkInteractionImage)
ENDIF()

#*************************** MicronTrackerTest ***************************
IF(PLUS_USE_MICRONTRACKER AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkMicronTrackerTest vtkMicronTrackerTest.cxx)
  SET_TARGET_PROPERTIES(vtkMicronTrackerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkMicronTrackerTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)    
ENDIF() 

#*************************** IntelRealSenseTrackerTest ***************************
IF(PLUS_USE_INTELREALSENSE AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkIntelRealSenseTrackerTest vtkIntelRealSenseTrackerTest.cxx)
  SET_TARGET_PROPERTIES(vtkIntelRealSenseTrackerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkIntelRealSenseTrackerTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
ENDIF()

#*************************** IntelRealSenseCamTest ***************************
IF(PLUS_USE_INTELREALSENSE_CAM AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkIntelRealSenseCamTest vtkIntelRealSenseCamTest.cxx)
  SET_TARGET_PROPERTIES(vtkIntelRealSenseCamTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkIntelRealSenseCamTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
ENDIF()

#*************************** vtkNVidiaDVPVideoSourceTest ***************************
IF(PLUS_USE_NVIDIA_DVP)
  ADD_EXECUTABLE(vtkNVidiaDVPVideoSourceTest vtkNVidiaDVPVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkNVidiaDVPVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkNVidiaDVPVideoSourceTest vtkPlusDataCollection )
ENDIF()

#*************************** vtkOpenIGTLinkTrackerTest ***************************
IF(PLUS_USE_OpenIGTLink)
  ADD_EXECUTABLE(vtkOpenIGTLinkTrackerTest vtkOpenIGTLinkTrackerTest.cxx )
  SET_TARGET_PROPERTIES(vtkOpenIGTLinkTrackerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkOpenIGTLinkTrackerTest vtkPlusDataCollection vtkPlusServer)

  ADD_TEST(vtkOpenIGTLinkTrackerTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkOpenIGTLinkTrackerTest
    --client-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTracker_TDATA_Client.xml
	--server-config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_OpenIGTLinkTracker_TDATA_Server.xml
    )
ENDIF()

#*************************** OpenHapticsDeviceTest *******************************
IF(PLUS_USE_OPENHAPTICS)
  ADD_EXECUTABLE(vtkOpenHapticsDeviceTest vtkOpenHapticsDeviceTest.cxx)
  SET_TARGET_PROPERTIES(vtkOpenHapticsDeviceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkOpenHapticsDeviceTest vtkPlusDataCollection vtkPlusCommon)
ENDIF()

#*********************** InfraredSeekCameraDeviceTest *****************************
IF(PLUS_USE_INFRARED_SEEK_CAM AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkInfraredSeekCamTest vtkInfraredSeekCamTest.cxx)
  SET_TARGET_PROPERTIES(vtkInfraredSeekCamTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkInfraredSeekCamTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
ENDIF()

#*********************** InfraredTEQ1CameraDeviceTest *****************************
IF(PLUS_USE_INFRARED_TEQ1_CAM AND PLUS_RENDERING_ENABLED)
  ADD_EXECUTABLE(vtkInfraredTEQ1CamTest vtkInfraredTEQ1CamTest.cxx)
  SET_TARGET_PROPERTIES(vtkInfraredTEQ1CamTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkInfraredTEQ1CamTest vtkPlusDataCollection vtkPlusCommon vtkInteractionImage)
ENDIF()

#*************************** vtkOvrvisionProVideoSourceTest ***************************
IF(PLUS_USE_OvrvisionPro AND PLUS_TEST_OvrvisionPro)
  ADD_EXECUTABLE(vtkOvrvisionProVideoSourceTest vtkOvrvisionProVideoSourceTest.cxx )
  SET_TARGET_PROPERTIES(vtkOvrvisionProVideoSourceTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkOvrvisionProVideoSourceTest vtkPlusDataCollection )
ENDIF()

#*************************** vtkPlusDeviceFactoryTest ***************************
ADD_EXECUTABLE(vtkPlusDeviceFactoryTest vtkPlusDeviceFactoryTest.cxx)
SET_TARGET_PROPERTIES(vtkPlusDeviceFactoryTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(vtkPlusDeviceFactoryTest vtkPlusDataCollection vtkPlusCommon)
ADD_TEST(vtkPlusDeviceFactoryTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkPlusDeviceFactoryTest
  )
# output is not checked for errors and warnings, as some error logs are expected

#*************************** NDICertusTest ***************************
IF(PLUS_USE_NDI_CERTUS)
  ADD_EXECUTABLE(NDICertusTest NDICertusTest.cxx)
  SET_TARGET_PROPERTIES(NDICertusTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(NDICertusTest vtkPlusDataCollection)
ENDIF()

#*************************** CmsBrachyStepperTest ***************************
IF(PLUS_USE_BRACHY_TRACKER)
  ADD_EXECUTABLE(CmsBrachyStepperTest CmsBrachyStepperTest.cxx)
  SET_TARGET_PROPERTIES(CmsBrachyStepperTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(CmsBrachyStepperTest vtkPlusDataCollection vtkPlusCommon)
ENDIF()

#*************************** CivcoBrachyStepperTest ***************************
IF(PLUS_USE_BRACHY_TRACKER)
  ADD_EXECUTABLE(CivcoBrachyStepperTest CivcoBrachyStepperTest.cxx)
  SET_TARGET_PROPERTIES(CivcoBrachyStepperTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(CivcoBrachyStepperTest vtkPlusDataCollection vtkPlusCommon)
ENDIF()

#*************************** USDigitalEncoderStepperTest ***************************
IF(PLUS_USE_USDIGITALENCODERS_TRACKER)
  ADD_EXECUTABLE(vtkUSDigitalEncodersTrackerTest vtkUSDigitalEncodersTrackerTest.cxx)
  SET_TARGET_PROPERTIES(vtkUSDigitalEncodersTrackerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkUSDigitalEncodersTrackerTest vtkPlusDataCollection vtkPlusCommon)
ENDIF()

#*************************** TransformInterpolationTest ***************************
ADD_EXECUTABLE(TransformInterpolationTest TransformInterpolationTest.cxx)
SET_TARGET_PROPERTIES(TransformInterpolationTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(TransformInterpolationTest vtkPlusCommon vtkPlusDataCollection)

ADD_TEST(TransformInterpolationTest
  ${PLUS_EXECUTABLE_OUTPUT_PATH}/TransformInterpolationTest
  --source-seq-file=${TestDataDir}/TransformInterpolationTest.igs.mha
  --transform=ProbeToTracker
  --max-rotation-difference=1.0
  --max-translation-difference=0.5
  )

#*************************** vtkVirtualTextRecognizerTest ***************************
IF(PLUS_TEST_TextRecognizer)
  ADD_EXECUTABLE(vtkVirtualTextRecognizerTest vtkVirtualTextRecognizerTest.cxx)
  SET_TARGET_PROPERTIES(vtkVirtualTextRecognizerTest PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(vtkVirtualTextRecognizerTest vtkPlusDataCollection vtkPlusCommon)

  ADD_TEST(vtkVirtualTextRecognizerTest
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/vtkVirtualTextRecognizerTest
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_VirtualTextRecognizerTest.xml
    --device-id=TextRecognizerDevice
    --field-value=Peters
    )
ENDIF()

#*************************** LeapMotionTest1 ***************************
IF(PLUS_USE_LEAPMOTION AND PLUS_TEST_LEAPMOTION)
  ADD_EXECUTABLE(LeapMotionTest1 vtkLeapMotionTest1.cxx)
  SET_TARGET_PROPERTIES(LeapMotionTest1 PROPERTIES FOLDER Tests)
  TARGET_LINK_LIBRARIES(LeapMotionTest1 vtkPlusDataCollection) # add any other dependencies to the test executable

  ADD_TEST(LeapMotionTest1
    ${PLUS_EXECUTABLE_OUTPUT_PATH}/LeapMotionTest1
    --config-file=${ConfigFilesDir}/Testing/PlusDeviceSet_DataCollectionOnly_LeapMotion.xml
    )
  SET_TESTS_PROPERTIES(LeapMotionTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING")
ENDIF()

# --------------------------------------------------------------------------
# Install
#
INSTALL(
  TARGETS TrackingTest
  DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  COMPONENT RuntimeExecutables
  )
