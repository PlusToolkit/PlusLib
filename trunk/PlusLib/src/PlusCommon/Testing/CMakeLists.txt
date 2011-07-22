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
  --input-metafile=${TestDataDir}/
  --input-averaged-items-for-filtering=20
  --input-max-timestamp-difference=0.08
  --input-min-stdev-reduction-factor=3.0
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusLoggerTest PlusLoggerTest.cxx )
TARGET_LINK_LIBRARIES(PlusLoggerTest vtkPlusCommon )

ADD_TEST(PlusLoggerTest 
  ${EXECUTABLE_OUTPUT_PATH}/PlusLoggerTest
  --verbose=5
  )
