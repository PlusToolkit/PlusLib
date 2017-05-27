SET( TestDataDir ${PLUSLIB_DATA_DIR}/TestImages )
SET( ConfigFilesDir ${PLUSLIB_DATA_DIR}/ConfigFiles )

#--------------------------------------------------------------------------------------------
# Tests
# 

  
# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS 
  DESTINATION "${PLUSLIB_BINARY_INSTALL}"
  COMPONENT RuntimeExecutables
  )