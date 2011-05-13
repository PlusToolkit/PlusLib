SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#***************************  NDICertusTest ***************************
IF (PLUS_USE_CERTUS)
	ADD_EXECUTABLE(NDICertusTest NDICertusTest.cxx)
	TARGET_LINK_LIBRARIES(NDICertusTest vtkTracking)
ENDIF (PLUS_USE_CERTUS)

#***************************  AMSStepperTest ***************************
IF (PLUS_USE_AMS_TRACKER)
    ADD_EXECUTABLE(AMSStepperTest AMSStepperTest.cxx)
    TARGET_LINK_LIBRARIES(AMSStepperTest vtkTracking vtkCommon vtkPlusCommon)
ENDIF (PLUS_USE_AMS_TRACKER) 
