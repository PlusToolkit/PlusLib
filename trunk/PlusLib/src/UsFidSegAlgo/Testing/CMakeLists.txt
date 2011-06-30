SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

###################################################
ADD_EXECUTABLE( ConvertFcsvToXml
  ConvertFcsvToXml.cpp 
  Segmentation.cpp
  )

 SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertFcsvToXml
	UsFidSegAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkImageAcquisition
  ) 
  
ADD_TEST(ConvertFcsvToXml
	${EXECUTABLE_OUTPUT_PATH}/ConvertFcsvToXml
	--input-test-data-dir=${TestDataDir}
	--input-fcsv-file=UsTestSeqIla5ManualFidSeg.fcsv
	--input-img-seq-file=ila5.mhd
	--input-testcase-name=ila5
	--output-xml-file=${TestDataDir}/UsTestSeqIla5ManualFidSeg.xml
	)
SET_TESTS_PROPERTIES( ConvertFcsvToXml PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( ConvertXmlToFcsv
  ConvertXmlToFcsv.cpp 
  )

 SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( ConvertXmlToFcsv
	UsFidSegAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkImageAcquisition
  ) 
  
ADD_TEST(ConvertXmlToFcsv
	${EXECUTABLE_OUTPUT_PATH}/ConvertXmlToFcsv
	--input=${TestDataDir}/UsTestSeqIla5ManualFidSeg.xml
	--output=${TestDataDir}/UsTestSeqIla5ManualFidSeg2.xml
	)
SET_TESTS_PROPERTIES( ConvertXmlToFcsv PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

###################################################
ADD_EXECUTABLE( UsFidSegTest
  UsFidSegTest.cpp
  Segmentation.cpp
  )

INCLUDE_DIRECTORIES( ${PlusLib_SOURCE_DIR}/src/TrusCalibration )

SET(ITK_LIBRARIES
	ITKCommon
	)

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( UsFidSegTest
	UsFidSegAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkImageAcquisition
	vtkTrusCalibration
  ) 
 

###################################################
ADD_TEST(UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened
	${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	--input-test-data-dir=${TestDataDir}
	--input-img-seq-file=UsTestSeqBaselineThomasShortened.mha
	--input-testcase-name=UsTestSeqBaselineThomasShortened
	--input-baseline-file=${TestDataDir}/UsTestSeqBaselineThomasShortened_baseline.xml
	--output-result-file=testcomparisons.xml
	--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig.xml
	--input-phantom-definition-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
	)
SET_TESTS_PROPERTIES( UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_UsTestSeqBaselineThomasShortened PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2
	${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	--input-test-data-dir=${TestDataDir}
	--input-img-seq-file=SegmentationTest_BKMedical_RandomStepperMotionData2.mha
	--input-testcase-name=SegmentationTest_BKMedical_RandomStepperMotionData2
	--input-baseline-file=${TestDataDir}/SegmentationTest_BKMedical_RandomStepperMotionData2_Segmentation_baseline.xml
	--output-result-file=testcomparisons.xml
	--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_iCal_BKMedical_FrameGrabber.xml
	--input-phantom-definition-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
	)
SET_TESTS_PROPERTIES( UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_BKMedical_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2
	${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	--input-test-data-dir=${TestDataDir}
	--input-img-seq-file=SegmentationTest_VLCUS_RandomStepperMotionData2.mha
	--input-testcase-name=SegmentationTest_VLCUS_RandomStepperMotionData2
	--input-baseline-file=${TestDataDir}/SegmentationTest_VLCUS_RandomStepperMotionData2_Segmentation_baseline.xml
	--output-result-file=testcomparisons.xml
	--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_iCal_VLCUS_FrameGrabber.xml
	--input-phantom-definition-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
	)
SET_TESTS_PROPERTIES( UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_VLCUS_RandomStepperMotionData2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData
	${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	--input-test-data-dir=${TestDataDir}
	--input-img-seq-file=USTC_FrameGrabber_ProbeRotationData.mha
	--input-testcase-name=USTC_FrameGrabber_ProbeRotationData
	--input-baseline-file=${TestDataDir}/USTC_FrameGrabber_ProbeRotationData_Segmentation_baseline.xml
	--output-result-file=testcomparisons.xml
	--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_iCal_SonixRP_FrameGrabber.xml
	--input-phantom-definition-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
	)
SET_TESTS_PROPERTIES( UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_USTC_FrameGrabber_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

ADD_TEST(UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData
	${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	--input-test-data-dir=${TestDataDir}
	--input-img-seq-file=USTC_Ulterius_ProbeRotationData.mha
	--input-testcase-name=USTC_Ulterius_ProbeRotationData
	--input-baseline-file=${TestDataDir}/USTC_Ulterius_ProbeRotationData_Segmentation_baseline.xml
	--output-result-file=testcomparisons.xml
	--input-config-file-name=${ConfigFilesDir}/USCalibrationConfig_iCal_SonixRP_Ulterius.xml
	--input-phantom-definition-file-name=${ConfigFilesDir}/PhantomDefinition_iCal_1.0.xml
	)
SET_TESTS_PROPERTIES( UsFidSegTest_CALIBRATION_PHANTOM_6_POINT_USTC_Ulterius_ProbeRotationData PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(UsFidSegTest_TAB2_5_POINT_1
	# ${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	# --input-test-data-dir=${TestDataDir}
	# --input-img-seq-file=UsFidSegTestNov19_01.mha
	# --input-testcase-name=Nov19_01
	# --input-baseline-file=${TestDataDir}/UsTestSeqNov19_01ManualFidSeg.xml
	# --output-result-file=${TestDataDir}/UsFidSegTest_01comparisons.xml
	# )
#SET_TESTS_PROPERTIES( UsFidSegTest_TAB2_5_POINT_1 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(UsFidSegTest_TAB2_5_POINT_2
	# ${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	# --input-test-data-dir=${TestDataDir}
	# --input-img-seq-file=UsFidSegTestNov19_02.mha
	# --input-testcase-name=Nov19_02
	# --input-baseline-file=${TestDataDir}/UsTestSeqNov19_02ManualFidSeg.xml
	# --output-result-file=${TestDataDir}/UsFidSegTest_02comparisons.xml
	# )
#SET_TESTS_PROPERTIES( UsFidSegTest_TAB2_5_POINT_2 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(UsFidSegTest_TAB2_5_POINT_3
	# ${EXECUTABLE_OUTPUT_PATH}/UsFidSegTest
	# --input-test-data-dir=${TestDataDir}
	# --input-img-seq-file=UsFidSegTestNov19_03.mha
	# --input-testcase-name=Nov19_03
	# --input-baseline-file=${TestDataDir}/UsTestSeqNov19_03ManualFidSeg.xml
	# --output-result-file=${TestDataDir}/UsFidSegTest_03comparisons.xml
	# )
#SET_TESTS_PROPERTIES( UsFidSegTest_TAB2_5_POINT_3 PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )

# ADD_TEST(vtkSegmentedWiresPositionsTest 
	# ${EXECUTABLE_OUTPUT_PATH}/vtkSegmentedWiresPositionsTest
	# --input-test-data-dir=${TestDataDir}
	# --input-img-seq-file=UsTestSeqBaselineThomasShortened.mha
	# --input-baseline-file=${TestDataDir}/baselineTestWithThomasData.xml
	# --output-result-file=testcomparisons.xml
	# )
# SET_TESTS_PROPERTIES( vtkSegmentedWiresPositionsTest PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;WARNING" )
 
###################################################
ADD_EXECUTABLE( vtkSegmentedWiresPositionsTest
  vtkSegmentedWiresPositionsTest.cxx
  )

# Link the executable to the algo library.
TARGET_LINK_LIBRARIES( vtkSegmentedWiresPositionsTest
	UsFidSegAlgo
	${ITK_LIBRARIES}
	vtksys
	vtkCommon
	vtkIO
	vtkImageAcquisition
  ) 
 