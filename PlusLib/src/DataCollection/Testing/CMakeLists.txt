SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(TimestampFilteringTest TimestampFilteringTest.cxx )
TARGET_LINK_LIBRARIES(TimestampFilteringTest vtkPlusCommon vtkImageAcquisition vtkTracking )

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
  --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_DataCollectionOnly_SavedDataset.xml 
  --video-buffer-seq-file=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --tracker-buffer-seq-file=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --rendering-off
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

IF (PLUS_USE_ULTRASONIX_VIDEO)
	ADD_TEST(vtkDataCollectorTest1_SonixVideo
	  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest1
	  --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml 
	  --rendering-off
	  )
	SET_TESTS_PROPERTIES( vtkDataCollectorTest1_SonixVideo PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
ENDIF (PLUS_USE_ULTRASONIX_VIDEO)
 
#***************************  vtkDataCollectorTest2 ***************************
ADD_EXECUTABLE(vtkDataCollectorTest2 vtkDataCollectorTest2.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorTest2 vtkDataCollection )
ADD_TEST(vtkDataCollectorTest2 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorTest2
  --video-buffer-seq-file=${TestDataDir}/DataCollectorSyncVideoBuffer.mha 
  --tracker-buffer-seq-file=${TestDataDir}/DataCollectorSyncTrackerBuffer.mha 
  --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_DataCollectionOnly_SavedDataset.xml 
  --acq-time-length=5
  --verbose=3
  )
SET_TESTS_PROPERTIES( vtkDataCollectorTest2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

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
TARGET_LINK_LIBRARIES( ReplayRecordedDataTest vtkDataCollection vtkCommon )
ADD_TEST( ReplayRecordedDataTest
  ${EXECUTABLE_OUTPUT_PATH}/ReplayRecordedDataTest
  --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_OpenIGTLinkTest.xml
  )
SET_TESTS_PROPERTIES( ReplayRecordedDataTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR" )

#***************************  vtkDataCollectorFileTest ***************************
ADD_EXECUTABLE(vtkDataCollectorFileTest vtkDataCollectorFileTest.cxx)
TARGET_LINK_LIBRARIES(vtkDataCollectorFileTest vtkDataCollection )
ADD_TEST(vtkDataCollectorFileTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkDataCollectorFileTest
  --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_DataCollectionOnly_File.xml 
  )
SET_TESTS_PROPERTIES( vtkDataCollectorFileTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusVersion PlusVersion.cxx )
TARGET_LINK_LIBRARIES(PlusVersion vtkPlusCommon vtkTracking vtkImageAcquisition)
ADD_TEST(PlusVersion 
  ${EXECUTABLE_OUTPUT_PATH}/PlusVersion
  )
  
# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS PlusVersion
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
