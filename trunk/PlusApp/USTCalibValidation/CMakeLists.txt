
ADD_EXECUTABLE(USTCalibValidation
  USTCalibValidation.cxx  
  )

TARGET_LINK_LIBRARIES(
  USTCalibValidation
  vtkDataCollection
)

INSTALL(FILES 
	${PLUS_DATA_DIR}/ConfigFiles/TemplateModel.xml 
	DESTINATION ${PLUSAPP_INSTALL_CONFIG_DIR}
	COMPONENT RuntimeLibraries
  )

INSTALL(TARGETS USTCalibValidation
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
