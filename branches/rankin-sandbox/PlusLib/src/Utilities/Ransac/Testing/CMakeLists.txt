ADD_EXECUTABLE(planeEstimationTest PlaneParametersEstimatorTest.cxx)
TARGET_LINK_LIBRARIES(planeEstimationTest ${LINK_LIBRARIES})

ADD_EXECUTABLE(sphereEstimationTest SphereParametersEstimatorTest.cxx)
TARGET_LINK_LIBRARIES(sphereEstimationTest ${LINK_LIBRARIES})


ADD_TEST(PlaneEstimationTest planeEstimationTest)
ADD_TEST(SphereEstimationTest sphereEstimationTest)
