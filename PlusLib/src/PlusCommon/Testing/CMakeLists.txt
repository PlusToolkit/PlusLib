SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkGnuplotExecuterTest vtkGnuplotExecuterTest.cxx )
TARGET_LINK_LIBRARIES(vtkGnuplotExecuterTest vtkCommon vtkPlusCommon )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(TimestampFilteringTest TimestampFilteringTest.cxx )
TARGET_LINK_LIBRARIES(TimestampFilteringTest vtkPlusCommon vtkImageAcquisition vtkTracking )

ADD_TEST(TimestampFilteringTest 
  ${EXECUTABLE_OUTPUT_PATH}/TimestampFilteringTest
  --input-metafile=${TestDataDir}/TimestampFilteringTest.mha 
  --input-averaged-items-for-filtering=20
  --input-max-timestamp-difference=0.08
  --input-min-stdev-reduction-factor=3.0
  )
SET_TESTS_PROPERTIES( TimestampFilteringTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPlusLoggerTest vtkPlusLoggerTest.cxx )
TARGET_LINK_LIBRARIES(vtkPlusLoggerTest vtkPlusCommon )

ADD_TEST(vtkPlusLoggerTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPlusLoggerTest
  --verbose=5
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkAccurateTimerTest vtkAccurateTimerTest.cxx )
TARGET_LINK_LIBRARIES(vtkAccurateTimerTest vtkPlusCommon )

ADD_TEST(vtkAccurateTimerTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkAccurateTimerTest
  --testTimeSec=10
  --averageIntendedDelaySec=0.005
  --numberOfThreads=3
  --verbose=3
  )
SET_TESTS_PROPERTIES( vtkAccurateTimerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
