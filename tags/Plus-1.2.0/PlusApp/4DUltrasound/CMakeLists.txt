INCLUDE_DIRECTORIES(${ULTERIUS_INCLUDE_DIR})
#
# Try to find OpenIGTLink and include its settings (otherwise complain)
# Should use OpenIGTLink built in Slicer
# 
FIND_PACKAGE(OpenIGTLink REQUIRED)
INCLUDE(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(4DUltrasound
  4DUltrasound.cxx 
  vtkDataProcessor.cxx
  vtkDataSender.cxx
  vtkInstrumentTracker.cxx
  )

#Use tracker device
TARGET_LINK_LIBRARIES(
  4DUltrasound 
  OpenIGTLink
  igtlutil
  vtkCommon
  vtkHybrid
  vtkPlusCommon
  vtkTracking
  vtkImageAcquisition
  vtkVolumeReconstruction
  vtkSignaling
)

INSTALL(TARGETS 4DUltrasound
  RUNTIME DESTINATION ${PLUSAPP_INSTALL_BIN_DIR} COMPONENT Applications
  )
