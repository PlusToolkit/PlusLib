#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkUSCalibrationControllerTest1 vtkUSCalibrationControllerTest1.cxx)
TARGET_LINK_LIBRARIES(vtkUSCalibrationControllerTest1 itkvnl itkvnl_algo vtkCommon vtkRendering vtkHybrid vtkTrusCalibration vtkDataCollection )

SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_TEST(vtkUSCalibrationControllerTest-Exact
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
    --input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
    --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
    --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_iCal.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		)
SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest-Exact PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUSCalibrationControllerTest
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
    --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
    --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_iCal.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		--translation-error-threshold=1.0
		--rotation-error-threshold=1.0
		)
SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUSCalibrationControllerTest_FrameGrabber-Exact
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
    --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
    --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
		)
SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest_FrameGrabber-Exact PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkUSCalibrationControllerTest_FrameGrabber
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
    --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
    --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_FrameGrabber_iCal.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
		--translation-error-threshold=1.0
		--rotation-error-threshold=1.0
		)
SET_TESTS_PROPERTIES( vtkUSCalibrationControllerTest_FrameGrabber PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
		
#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(vtkStepperCalibrationControllerTest vtkStepperCalibrationControllerTest.cxx)
TARGET_LINK_LIBRARIES(vtkStepperCalibrationControllerTest itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection vtkImageAcquisition )

# ADD_TEST(vtkStepperCalibrationControllerTest
		# ${EXECUTABLE_OUTPUT_PATH}/vtkStepperCalibrationControllerTest
    # --input-calibration-sequence-metafile=${TestDataDir}/UsTemplateCalibrationBaseline.mha
    # --input-validation-sequence-metafile=${TestDataDir}/UsTemplateCalibrationBaseline.mha
    # --input-centerofrotation-sequence-metafile=${TestDataDir}/UsTemplateCalibrationRotationBaseline.mha
		# --input-config-file-name=${ConfigFilesDir}/USCalibrationConfig.xml
		# --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		# )
#SET_TESTS_PROPERTIES( vtkStepperCalibrationControllerTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

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
ADD_TEST(vtkFreehandCalibrationTest-Exact
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_fCal.xml
		--input-freehand-motion-1-sequence-metafile=${TestDataDir}/fCal_Test_Calibration.mha 
		--input-freehand-motion-2-sequence-metafile=${TestDataDir}/fCal_Test_Validation.mha 
		--input-baseline-file-name=${TestDataDir}/FreehandCalibration.results.xml
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibrationTest-Exact PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(vtkFreehandCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkFreehandCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/Test_PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_fCal.xml
		--input-freehand-motion-1-sequence-metafile=${TestDataDir}/fCal_Test_Calibration.mha 
		--input-freehand-motion-2-sequence-metafile=${TestDataDir}/fCal_Test_Validation.mha 
		--input-baseline-file-name=${TestDataDir}/FreehandCalibration.results.xml
		--translation-error-threshold=1.0
		--rotation-error-threshold=1.0
		)
SET_TESTS_PROPERTIES( vtkFreehandCalibrationTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
