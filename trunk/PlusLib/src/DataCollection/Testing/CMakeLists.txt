SET( TestDataDir ${PLUS_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUS_DATA_DIR}/ConfigFiles )

#***************************  vtkDataCollectorTest1 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest1 vtkDataCollectorTest1.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest1 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
  --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_SavedDataset.xml 
  --input-video-buffer-metafile=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --input-tracker-buffer-metafile=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --rendering-off
  )

IF (PLUS_USE_SONIX_VIDEO)
	ADD_TEST(vtkDataCollectorTest1_SonixVideo
	  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
	  --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_SonixVideo.xml 
	  --rendering-off
	  )
ENDIF (PLUS_USE_SONIX_VIDEO)


#***************************  vtkDataCollectorSynchronizerTest ***************************
ADD_EXECUTABLE(vtkDataCollectorSynchronizerTest vtkDataCollectorSynchronizerTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorSynchronizerTest vtkDataCollection )
ADD_TEST(vtkDataCollectorSynchronizerTest1 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorSynchronizerTest
  --input-test-data-dir=${TestDataDir}
  --input-threshold-multiplier=5
  --input-video-buffer-seq-file-name=DataCollectorSyncVideoBuffer.mha 
  --input-tracker-buffer-seq-file-name=DataCollectorSyncTrackerBuffer.mha 
  --input-number-of-averaged-frames=6
  --input-max-frame-difference=10
  --input-min-frame-threshold=200
  --input-number-of-averaged-transforms=15
  --input-max-transform-difference=2.0
  --input-min-transform-threshold=5.0
  --verbose=4
  )
 
 
 #***************************  vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest2 vtkDataCollectorTest2.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest2 vtkDataCollection )
# ADD_TEST(vtkDataCollectorTest2 
  # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest2
  # --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig.xml 
  # --verbose=2
  # )
 
 #***************************  vtkDataCollectorTrackingTest ***************************
ADD_EXECUTABLE(vtkDataCollectorTrackingTest vtkDataCollectorTrackingTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTrackingTest vtkDataCollection )
# ADD_TEST(vtkDataCollectorTrackingTest 
  # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTrackingTest
  # --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_VideoNone.xml 
  # --verbose=2
  # )

#***************************  vtkDataCollectorVideoAcqTest ***************************
ADD_EXECUTABLE(vtkDataCollectorVideoAcqTest vtkDataCollectorVideoAcqTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorVideoAcqTest vtkDataCollection )
# IF (PLUS_USE_SONIX_VIDEO)
	# ADD_TEST(vtkDataCollectorVideoAcqTest 
	  # ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorVideoAcqTest
	  # --input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_TrackerNone.xml 
	  # --verbose=2
	  # )
# ENDIF (PLUS_USE_SONIX_VIDEO)


ADD_EXECUTABLE( qtOpenIGTLinkBroadcasterTest1 qtOpenIGTLinkBroadcasterTest1.cxx)
TARGET_LINK_LIBRARIES( qtOpenIGTLinkBroadcasterTest1 vtkDataCollection ${QT_LIBRARIES} vtkCommon )
IF (PLUS_USE_OpenIGTLink )
  ADD_TEST( qtOpenIGTLinkBroadcasterTest1
    ${EXECUTABLE_OUTPUT_PATH}/qtOpenIGTLinkBroadcasterTest1
    --input-config-file=${ConifgFilesDir}/OpenIGTLinkTest.xml
    --input-video-buffer-metafile=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
    --input-tracker-buffer-metafile=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
    )
ENDIF (PLUS_USE_OpenIGTLink )
