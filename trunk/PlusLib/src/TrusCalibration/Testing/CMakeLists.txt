
#--------------------------------------------------------------------------------------------
SET (vtkUSCalibrationControllerTest_LIBS ${vtkUSCalibrationControllerTest_LIBS} itkvnl itkvnl_algo vtkCommon vtkRendering vtkHybrid vtkTrusCalibration vtkDataCollection)
ADD_EXECUTABLE(vtkUSCalibrationControllerTest1 vtkUSCalibrationControllerTest1.cxx)
TARGET_LINK_LIBRARIES(vtkUSCalibrationControllerTest1 ${vtkUSCalibrationControllerTest_LIBS} )

SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

ADD_TEST(vtkUSCalibrationControllerTest-Exact
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
        --input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
        --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
        --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig.xml
		--input-phantom-definition-xml-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
		--input-template-model-config-file-name=${ConfigFilesDir}/TemplateModel.xml 
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		--verbose=1
		)

ADD_TEST(vtkUSCalibrationControllerTest
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData1.mha 
        --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_Ulterius_RandomStepperMotionData2.mha 
        --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_Ulterius_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig.xml
		--input-template-model-config-file-name=${ConfigFilesDir}/TemplateModel.xml 
		--input-phantom-definition-xml-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		--translation-error-threshold=1.0
		--rotation-error-threshold=1.0
		--verbose=1
		)

ADD_TEST(vtkUSCalibrationControllerTest_FrameGrabber-Exact
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
        --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
        --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_FrameGrabber.xml
		--input-template-model-config-file-name=${ConfigFilesDir}/TemplateModel.xml 
		--input-phantom-definition-xml-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
		--verbose=1
		)

ADD_TEST(vtkUSCalibrationControllerTest_FrameGrabber
		${EXECUTABLE_OUTPUT_PATH}/vtkUSCalibrationControllerTest1
		--input-random-stepper-motion-1-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData1.mha 
        --input-random-stepper-motion-2-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_RandomStepperMotionData2.mha 
        --input-probe-rotation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData.mha 
		--input-probe-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_ProbeTranslationData.mha 
		--input-template-translation-sequence-metafile=${TestDataDir}/USTC_FrameGrabber_TemplateTranslationData.mha 
		--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_FrameGrabber.xml
		--input-template-model-config-file-name=${ConfigFilesDir}/TemplateModel.xml 
		--input-phantom-definition-xml-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
		--input-baseline-file-name=${TestDataDir}/UsTemplateCalibration_FrameGrabber.results.xml 
		--translation-error-threshold=1.0
		--rotation-error-threshold=1.0
		--verbose=1
		)
		
#--------------------------------------------------------------------------------------------
SET (vtkTemplateToStepperCalibratorTest1_LIBS ${vtkTemplateToStepperCalibratorTest1_LIBS} itkvnl itkvnl_algo vtkCommon vtkIO vtkImageAcquisition vtkTrusCalibration UsFidSegAlgo)
ADD_EXECUTABLE(vtkTemplateToStepperCalibratorTest1 vtkTemplateToStepperCalibratorTest1.cxx)
TARGET_LINK_LIBRARIES(vtkTemplateToStepperCalibratorTest1 ${vtkTemplateToStepperCalibratorTest1_LIBS} )		


#--------------------------------------------------------------------------------------------
SET (vtkStepperCalibrationControllerTest_LIBS ${vtkStepperCalibrationControllerTest_LIBS} itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection vtkImageAcquisition)
ADD_EXECUTABLE(vtkStepperCalibrationControllerTest vtkStepperCalibrationControllerTest.cxx)
TARGET_LINK_LIBRARIES(vtkStepperCalibrationControllerTest ${vtkStepperCalibrationControllerTest_LIBS} )

# ADD_TEST(vtkStepperCalibrationControllerTest
		# ${EXECUTABLE_OUTPUT_PATH}/vtkStepperCalibrationControllerTest
        # --input-calibration-sequence-metafile=${TestDataDir}/UsTemplateCalibrationBaseline.mha
        # --input-validation-sequence-metafile=${TestDataDir}/UsTemplateCalibrationBaseline.mha
        # --input-centerofrotation-sequence-metafile=${TestDataDir}/UsTemplateCalibrationRotationBaseline.mha
		# --input-config-file-name=${ConfigFilesDir}/USCalibrationConfig.xml
		# --input-baseline-file-name=${TestDataDir}/UsTemplateCalibration.results.xml 
		# --verbose=1
		# )

#--------------------------------------------------------------------------------------------
SET (vtkStylusCalibrationTest_LIBS ${vtkStylusCalibrationTest_LIBS} itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection)
ADD_EXECUTABLE(vtkStylusCalibrationTest vtkStylusCalibrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkStylusCalibrationTest ${vtkStylusCalibrationTest_LIBS} )

ADD_TEST(vtkStylusCalibrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkStylusCalibrationTest
		--input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_FakeTracker_PivotCalibration.xml
		--input-baseline-file-name=${TestDataDir}/StylusCalibration.results.xml 
		--number-of-acquired-points=100
		--verbose=1
		)

#--------------------------------------------------------------------------------------------
SET (vtkPhantomRegistrationTest_LIBS ${vtkPhantomRegistrationTest_LIBS} itkvnl itkvnl_algo vtkCommon vtkTrusCalibration vtkDataCollection)
ADD_EXECUTABLE(vtkPhantomRegistrationTest vtkPhantomRegistrationTest.cxx)
TARGET_LINK_LIBRARIES(vtkPhantomRegistrationTest ${vtkPhantomRegistrationTest_LIBS} )

ADD_TEST(vtkPhantomRegistrationTest
		${EXECUTABLE_OUTPUT_PATH}/vtkPhantomRegistrationTest
		--input-config-file-name=${ConfigFilesDir}/USDataCollectionConfig_FakeTracker_RecordPhantomLandmarks.xml
		--input-baseline-file-name=${TestDataDir}/PhantomRegistration.results.xml
		--input-stylus-calibration-xml-file-name=${TestDataDir}/StylusCalibration.results.xml
		--input-phantom-definition-xml-file-name=${ConfigFilesDir}/PhantomDefinition_fCal_1.0.xml
		--verbose=1
		)
