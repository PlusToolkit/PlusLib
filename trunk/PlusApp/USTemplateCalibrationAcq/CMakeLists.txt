
ADD_EXECUTABLE(USTemplateCalibrationAcq
    USTemplateCalibrationAcq.cxx 
    vtkCalibDataProcessor.cxx
    vtkCalibDataProcessor.h
    vtkCalibDataSender.cxx
    vtkCalibDataSender.h
    vtkUltrasoundDataCollector.cxx 
    )

INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(
    USTemplateCalibrationAcq 
    vtkCommon
    vtkFiltering
    vtkHybrid
    vtkTracking
    vtkImageAcquisition
    vtkVolumeReconstruction 
    vtkPlusCommon
    vtkSignaling
    )

