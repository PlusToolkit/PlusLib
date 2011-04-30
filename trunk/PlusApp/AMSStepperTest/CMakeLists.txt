
ADD_EXECUTABLE(AMSStepperTest AMSStepperTest.cxx)
TARGET_LINK_LIBRARIES(AMSStepperTest vtkTracking vtkCommon vtkPlusCommon)

# --------------------------------------------------------------------------
# Install
#

IF(PLUSAPP_INSTALL_BIN_DIR)

INSTALL(TARGETS AMSStepperTest
  DESTINATION ${PLUSAPP_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
  
ENDIF(PLUSAPP_INSTALL_BIN_DIR)
