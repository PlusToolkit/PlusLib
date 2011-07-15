
ADD_EXECUTABLE(USTCalibValidation
  USTCalibValidation.cxx  
  )

TARGET_LINK_LIBRARIES(
  USTCalibValidation
  vtkDataCollection
)

INSTALL(TARGETS USTCalibValidation
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
