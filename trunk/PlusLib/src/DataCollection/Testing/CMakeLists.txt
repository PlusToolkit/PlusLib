SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#***************************  vtkDataCollectorTest1 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest1 vtkDataCollectorTest1.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest1 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
  --input-config-file-name=${ConfigFilesDir}/Test_USDataCollectionConfig_SavedDataset.xml 
  --input-video-buffer-metafile=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --input-tracker-buffer-metafile=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --rendering-off
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

IF (PLUS_USE_SONIX_VIDEO)
	ADD_TEST(vtkDataCollectorTest1_SonixVideo
	  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
	  --input-config-file-name=${ConfigFilesDir}/Test_USDataCollectionConfig_SonixVideo_FakeTracker.xml 
	  --rendering-off
	  )
	SET_TESTS_PROPERTIES( vtkDataCollectorTest1_SonixVideo PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_SONIX_VIDEO)


#***************************  vtkDataCollectorSynchronizerTest ***************************
ADD_EXECUTABLE(vtkDataCollectorSynchronizerTest vtkDataCollectorSynchronizerTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorSynchronizerTest vtkDataCollection )
ADD_TEST(vtkDataCollectorSynchronizerTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorSynchronizerTest
  --input-threshold-multiplier=5
  --input-video-buffer-seq-file-name=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --input-tracker-buffer-seq-file-name=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --input-number-of-averaged-frames=6
  --input-max-frame-difference=10
  --input-min-frame-threshold=200
  --input-number-of-averaged-transforms=15
  --input-max-transform-difference=2.0
  --input-min-transform-threshold=5.0
  --verbose=4
  ) 
SET_TESTS_PROPERTIES( vtkDataCollectorSynchronizerTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
 #***************************  vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest2 vtkDataCollectorTest2.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest2 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest2 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest2
  --input-video-buffer-metafile=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --input-tracker-buffer-metafile=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --input-config-file-name=${ConfigFilesDir}/Test_USDataCollectionConfig_SavedDataset.xml 
  --input-acq-time-length=5
  --verbose=2
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
 #***************************  vtkDataCollectorTrackingTest ***************************
ADD_EXECUTABLE(vtkDataCollectorTrackingTest vtkDataCollectorTrackingTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTrackingTest vtkDataCollection )
# ADD_TEST(vtkDataCollectorTrackingTest 
  # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTrackingTest
  # --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_VideoNone.xml 
  # --verbose=2
  # )
# SET_TESTS_PROPERTIES( vtkDataCollectorTrackingTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#***************************  vtkDataCollectorVideoAcqTest ***************************
ADD_EXECUTABLE(vtkDataCollectorVideoAcqTest vtkDataCollectorVideoAcqTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorVideoAcqTest vtkDataCollection )
# IF (PLUS_USE_SONIX_VIDEO)
	# ADD_TEST(vtkDataCollectorVideoAcqTest 
	  # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorVideoAcqTest
	  # --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_TrackerNone.xml 
	  # --verbose=2
	  # )
	  # SET_TESTS_PROPERTIES( vtkDataCollectorVideoAcqTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
# ENDIF (PLUS_USE_SONIX_VIDEO)

#***************************  vtkOpenIGTLinkBroadcasterTest1 ***************************
ADD_EXECUTABLE( vtkOpenIGTLinkBroadcasterTest1 vtkOpenIGTLinkBroadcasterTest1.cxx)
TARGET_LINK_LIBRARIES( vtkOpenIGTLinkBroadcasterTest1 vtkDataCollection vtkCommon )
IF (PLUS_USE_OpenIGTLink )
  ADD_TEST( vtkOpenIGTLinkBroadcasterTest1
    ${EXECUTABLE_OUTPUT_PATH}/vtkOpenIGTLinkBroadcasterTest1
    --input-config-file=${ConfigFilesDir}/OpenIGTLinkTest.xml
    --input-video-buffer-metafile=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
    --input-tracker-buffer-metafile=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
    )
  SET_TESTS_PROPERTIES( vtkOpenIGTLinkBroadcasterTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_OpenIGTLink )
