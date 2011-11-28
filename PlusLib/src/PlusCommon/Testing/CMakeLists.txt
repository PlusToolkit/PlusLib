SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkGnuplotExecuterTest vtkGnuplotExecuterTest.cxx )
TARGET_LINK_LIBRARIES(vtkGnuplotExecuterTest vtkCommon vtkPlusCommon )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPlusLoggerTest vtkPlusLoggerTest.cxx )
TARGET_LINK_LIBRARIES(vtkPlusLoggerTest vtkPlusCommon )

ADD_TEST(vtkPlusLoggerTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkPlusLoggerTest
  --verbose=5
  )

 #--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusCommonTest PlusCommonTest.cxx )
TARGET_LINK_LIBRARIES(PlusCommonTest vtkPlusCommon )

ADD_TEST(PlusCommonTest 
  ${EXECUTABLE_OUTPUT_PATH}/PlusCommonTest
  )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(PlusMathTest PlusMathTest.cxx )
TARGET_LINK_LIBRARIES(PlusMathTest vtkPlusCommon )

ADD_TEST(PlusMathTest 
  ${EXECUTABLE_OUTPUT_PATH}/PlusMathTest
  --input-data-file=${TestDataDir}/PlusMathTestData.xml 
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

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkTransformRepositoryTest vtkTransformRepositoryTest.cxx )
TARGET_LINK_LIBRARIES(vtkTransformRepositoryTest vtkPlusCommon )

ADD_TEST(vtkTransformRepositoryTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkTransformRepositoryTest
  --verbose=3
  )
# This test prints some errors when testing error cases, therefore the output is not
# checked for the presence of ERROR or WARNING string
