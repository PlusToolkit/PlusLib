SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#*************************** NDICertusTest ***************************
IF (PLUS_USE_CERTUS)
	ADD_EXECUTABLE(NDICertusTest NDICertusTest.cxx)
	TARGET_LINK_LIBRARIES(NDICertusTest vtkTracking)
ENDIF (PLUS_USE_CERTUS)

#*************************** CmsBrachyStepperTest ***************************
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(CmsBrachyStepperTest CmsBrachyStepperTest.cxx)
    TARGET_LINK_LIBRARIES(CmsBrachyStepperTest vtkTracking vtkCommon vtkPlusCommon)
ENDIF (PLUS_USE_BRACHY_TRACKER) 

#*************************** CivcoBrachyStepperTest ***************************
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(CivcoBrachyStepperTest CivcoBrachyStepperTest.cxx)
    TARGET_LINK_LIBRARIES(CivcoBrachyStepperTest vtkTracking vtkCommon vtkPlusCommon)
ENDIF (PLUS_USE_BRACHY_TRACKER) 

#*************************** vtkTrackerFactoryTest ***************************
ADD_EXECUTABLE(vtkTrackerFactoryTest vtkTrackerFactoryTest.cxx)
TARGET_LINK_LIBRARIES(vtkTrackerFactoryTest vtkTracking vtkPlusCommon)
ADD_TEST(vtkTrackerFactoryTest 
  ${EXECUTABLE_OUTPUT_PATH}/vtkTrackerFactoryTest
  )
# output is not checked for errors and warnings, as some error logs are expected
  
#*************************** TransformInterpolationTest ***************************
ADD_EXECUTABLE(TransformInterpolationTest TransformInterpolationTest.cxx)
TARGET_LINK_LIBRARIES(TransformInterpolationTest vtkTracking vtkPlusCommon vtkImageAcquisition)

ADD_TEST(TransformInterpolationTest 
  ${EXECUTABLE_OUTPUT_PATH}/TransformInterpolationTest
  --source-seq-file=${TestDataDir}/TransformInterpolationTest.mha 
  --transform=ProbeToTracker
  --max-rotation-difference=1.0
  --max-translation-difference=0.5
  )
