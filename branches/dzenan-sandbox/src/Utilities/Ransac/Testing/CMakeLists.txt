ADD_EXECUTABLE(planeEstimationTest PlaneParametersEstimatorTest.cxx)
SET_TARGET_PROPERTIES(planeEstimationTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(planeEstimationTest PUBLIC 
  ITKCommon
  itkvnl
  itkvnl_algo
  )

ADD_EXECUTABLE(sphereEstimationTest SphereParametersEstimatorTest.cxx)
SET_TARGET_PROPERTIES(sphereEstimationTest PROPERTIES FOLDER Tests)
TARGET_LINK_LIBRARIES(sphereEstimationTest PUBLIC 
  ITKCommon
  itkvnl
  itkvnl_algo
  )

ADD_TEST(PlaneEstimationTest planeEstimationTest)
ADD_TEST(SphereEstimationTest sphereEstimationTest)