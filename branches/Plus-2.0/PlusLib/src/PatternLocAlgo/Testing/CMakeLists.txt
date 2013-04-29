SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

###################################################
ADD_EXECUTABLE( ConvertFcsvToXml
  ConvertFcsvToXml.cxx 
  )

 SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertFcsvToXml
	PatternLocAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkDataCollection
  ) 
  
ADD_TEST(ConvertFcsvToXml
	${EXECUTABLE_OUTPUT_PATH}/ConvertFcsvToXml
	--test-data-dir=${TestDataDir}
	--fcsv-file=UsTestSeqIla5ManualFidSeg.fcsv
	--img-seq-file=ila5.mhd
	--testcase=ila5
	--output-xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg.xml
	)
SET_TESTS_PROPERTIES( ConvertFcsvToXml PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( ConvertXmlToFcsv
  ConvertXmlToFcsv.cxx 
  )

 SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertXmlToFcsv
	PatternLocAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkDataCollection
  ) 
  
ADD_TEST(ConvertXmlToFcsv
	${EXECUTABLE_OUTPUT_PATH}/ConvertXmlToFcsv
	--xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg.xml
	--output-xml-file=${CMAKE_CURRENT_BINARY_DIR}/UsTestSeqIla5ManualFidSeg2.xml
	)
SET_TESTS_PROPERTIES( ConvertXmlToFcsv PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( PatternLocTest
  PatternLocTest.cxx
  )

INCLUDE_DIRECTORIES( ${PlusLib_SOURCE_DIR}/src/CalibrationAlgo )

SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( PatternLocTest
	PatternLocAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkDataCollection
	vtkCalibrationAlgo	
	vtkDataCollection
  ) 


###################################################
ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=UsTestSeqBaselineThomasShortened.mha
	--testcase=UsTestSeqBaselineThomasShortened
	--baseline=${TestDataDir}/UsTestSeqBaselineThomasShortened_baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2.mha
	--testcase=SegmentationTest_BKMedical_RandomStepperMotionData2
	--baseline=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Segmentation_baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_BKMedical_FrameGrabber_iCal.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=SegmentationTest_VLCUS_RandomStepperMotionData2.mha
	--testcase=SegmentationTest_VLCUS_RandomStepperMotionData2
	--baseline=${TestDataDir}/SegmentationTest_VLCUS_RandomStepperMotionData2_Segmentation_baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_VLCUS_FrameGrabber_iCal.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=USTC_FrameGrabber_ProbeRotationData.mha
	--testcase=USTC_FrameGrabber_ProbeRotationData
	--baseline=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData_Segmentation_baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_FrameGrabber_iCal.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=USTC_Ulterius_ProbeRotationData.mha
	--testcase=USTC_Ulterius_ProbeRotationData
	--baseline=${TestDataDir}/USTC_Ulterius_ProbeRotationData_Segmentation_baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(PatternLocTest_CIRS_PHANTOM_13_POINT_TranslationData1
	${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	--test-data-dir=${TestDataDir}
	--img-seq-file=CIRS_TranslationData1.mha
	--testcase=CIRS_TranslationData1
	--baseline=${TestDataDir}/CIRS_Phantom_TranslationData1_Baseline.xml
	--output-xml-file=testcomparisons.xml
	--config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixTouch_Ulterius_CIRS_Phantom.xml
	)
SET_TESTS_PROPERTIES( PatternLocTest_CIRS_PHANTOM_13_POINT_TranslationData1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

#######################################################
#Test with parameters computed from Phantom Definition and configuration file
#######################################################

#These are commented to avoid extra computation time in the BuildAndTest.bat

# ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened_SegParamFromPhantomDef
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=UsTestSeqBaselineThomasShortened.mha
	# --testcase=UsTestSeqBaselineThomasShortened
	# --baseline=${TestDataDir}/UsTestSeqBaselineThomasShortened_baseline.xml
	# --output-xml-file=testcomparisons.xml
	# --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal_SegParamFromPhantomDef.xml
	# )
# SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened_SegParamFromPhantomDef PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2_SegParamFromPhantomDef
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2.mha
	# --testcase=SegmentationTest_BKMedical_RandomStepperMotionData2
	# --baseline=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Segmentation_baseline.xml
	# --output-xml-file=testcomparisons.xml
	# --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_BKMedical_FrameGrabber_iCal_SegParamFromPhantomDef.xml
	# )
# SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2_SegParamFromPhantomDef PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2_SegParamFromPhantomDef
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=SegmentationTest_VLCUS_RandomStepperMotionData2.mha
	# --testcase=SegmentationTest_VLCUS_RandomStepperMotionData2
	# --baseline=${TestDataDir}/SegmentationTest_VLCUS_RandomStepperMotionData2_Segmentation_baseline.xml
	# --output-xml-file=testcomparisons.xml
	# --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_VLCUS_FrameGrabber_iCal_SegParamFromPhantomDef.xml
	# )
# SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2_SegParamFromPhantomDef PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData_SegParamFromPhantomDef
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=USTC_FrameGrabber_ProbeRotationData.mha
	# --testcase=USTC_FrameGrabber_ProbeRotationData
	# --baseline=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData_Segmentation_baseline.xml
	# --output-xml-file=testcomparisons.xml
	# --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_FrameGrabber_iCal_SegParamFromPhantomDef.xml
	# )
# SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData_SegParamFromPhantomDef PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData_SegParamFromPhantomDef
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=USTC_Ulterius_ProbeRotationData.mha
	# --testcase=USTC_Ulterius_ProbeRotationData
	# --baseline=${TestDataDir}/USTC_Ulterius_ProbeRotationData_Segmentation_baseline.xml
	# --output-xml-file=testcomparisons.xml
	# --config-file=${ConfigFilesDir}/Testing/PlusConfiguration_CalibrationOnly_SonixRP_Ulterius_iCal_SegParamFromPhantomDef.xml
	# )
# SET_TESTS_PROPERTIES( PatternLocTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData_SegParamFromPhantomDef PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

####################################################################################
#Other tests for different wire patterns
####################################################################################

# ADD_TEST(PatternLocTest_TAB2_5_POINT_1
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=PatternLocTestNov19_01.mha
	# --testcase=Nov19_01
	# --baseline=${TestDataDir}/UsTestSeqNov19_01ManualFidSeg.xml
	# --output-xml-file=${TestDataDir}/PatternLocTest_01comparisons.xml
	# )
#SET_TESTS_PROPERTIES( PatternLocTest_TAB2_5_POINT_1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_TAB2_5_POINT_2
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=PatternLocTestNov19_02.mha
	# --testcase=Nov19_02
	# --baseline=${TestDataDir}/UsTestSeqNov19_02ManualFidSeg.xml
	# --output-xml-file=${TestDataDir}/PatternLocTest_02comparisons.xml
	# )
#SET_TESTS_PROPERTIES( PatternLocTest_TAB2_5_POINT_2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(PatternLocTest_TAB2_5_POINT_3
	# ${EXECUTABLE_OUTPUT_PATH}/PatternLocTest
	# --test-data-dir=${TestDataDir}
	# --img-seq-file=PatternLocTestNov19_03.mha
	# --testcase=Nov19_03
	# --baseline=${TestDataDir}/UsTestSeqNov19_03ManualFidSeg.xml
	# --output-xml-file=${TestDataDir}/PatternLocTest_03comparisons.xml
	# )
#SET_TESTS_PROPERTIES( PatternLocTest_TAB2_5_POINT_3 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(vtkSegmentedWiresPositionsTest 
	# ${EXECUTABLE_OUTPUT_PATH}/vtkSegmentedWiresPositionsTest
	# --test-data-dir=${TestDataDir}
	# --image-seq-file=UsTestSeqBaselineThomasShortened.mha
	# --baseline=${TestDataDir}/baselineTestWithThomasData.xml
	# --output-xml-file=testcomparisons.xml
	# )
# SET_TESTS_PROPERTIES( vtkSegmentedWiresPositionsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( vtkSegmentedWiresPositionsTest
  vtkSegmentedWiresPositionsTest.cxx
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( vtkSegmentedWiresPositionsTest
	PatternLocAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkDataCollection
  ) 
