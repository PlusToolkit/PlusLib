SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
IF (PLUS_USE_BRACHY_TRACKER)
    ADD_EXECUTABLE(vtkUSCalibrationControllerTest1 vtkUSCalibrationControllerTest1.cxx)
    TARGET_LINK_LIBRARIES(vtkUSCalibrationControllerTest1 itkvnl itkvnl_algo vtkCommon vtkRendering vtkHybrid vtkTrusCalibration vtkDataCollection )

    ADD_TEST(vtkUSCalibrationControllerTest
            ${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
            --input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
            --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
            --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_iCal.xml
            --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
            )
    SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

    ADD_TEST(vtkUSCalibrationControllerTest_FrameGrabber
            ${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
            --input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
            --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
            --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml
            --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
            )
    SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest_FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ENDIF ()
		
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkStylusCalibrationTest vtkStylusCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkStylusCalibrationTest itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection )

ADD_TEST(vtkStylusCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkStylusCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_VideoNone_FakeTracker_PivotCalibration_fCal.xml
		--input-baseline-file-name=${TestDataDir}/StylusCalibration.results.xml 
		--number-of-acquired-points=100
		)
SET_TESTS_PROPERTIES( vtkStylusCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkPhantomRegistrationTest vtkPhantomRegistrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationTest itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection )

ADD_TEST(vtkPhantomRegistrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_VideoNone_FakeTracker_RecordPhantomLandmarks_fCal.xml
		--input-baseline-file-name=${TestDataDir}/PhantomRegistration.results.xml
		)
SET_TESTS_PROPERTIES( vtkPhantomRegistrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkFreehandCalibrationTest vtkFreehandCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkFreehandCalibrationTest itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection )

# This test has not been completed yet, see #138
ADD_TEST(vtkFreehandCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_fCal.xml
		--input-freehand-motion-1-sequence-metafile=${TestDataDir}/fCal_Test_Calibration.mha 
		--input-freehand-motion-2-sequence-metafile=${TestDataDir}/fCal_Test_Validation.mha 
		--input-baseline-file-name=${TestDataDir}/FreehandCalibration.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkCenterOfRotationCalibAlgoTest vtkCenterOfRotationCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkCenterOfRotationCalibAlgoTest vtkPlusCommon vtkTrusCalibration vtkImageAcquisition )

ADD_TEST(CenterOfRotationCalibration-Ulterius
        ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
        --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
        --input-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha
        --input-baseline-file-name=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
        )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(CenterOfRotationCalibration-FrameGrabber
        ${EXECUTABLE_OUTPUT_PATH}/vtkCenterOfRotationCalibAlgoTest
        --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
        --input-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha
        --input-baseline-file-name=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
        )
SET_TESTS_PROPERTIES( CenterOfRotationCalibration-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkSpacingCalibAlgoTest vtkSpacingCalibAlgoTest.cxx)
TARGET_LINK_LIBRARIES(vtkSpacingCalibAlgoTest vtkPlusCommon vtkTrusCalibration vtkImageAcquisition )

ADD_TEST(SpacingCalibAlgoTest-Ulterius
            ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml 
            --input-sequence-metafiles ${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
            --input-baseline-file-name=${TestDataDir}/USTC_Ulterius_StepperCalibrationResultBaseline.xml
            )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-Ulterius PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(SpacingCalibAlgoTest-FrameGrabber
            ${EXECUTABLE_OUTPUT_PATH}/vtkSpacingCalibAlgoTest
            --input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml 
            --input-sequence-metafiles ${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
            --input-baseline-file-name=${TestDataDir}/USTC_FrameGrabber_StepperCalibrationResultBaseline.xml
            )
SET_TESTS_PROPERTIES( SpacingCalibAlgoTest-FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

