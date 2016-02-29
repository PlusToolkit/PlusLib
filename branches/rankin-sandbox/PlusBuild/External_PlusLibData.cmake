#--------------------------------------------------------------------------
# PlusLibData
SET (PLUS_PLUSLIBDATA_DIR ${PLUS_PLUSLIB_DIR}/data CACHE INTERNAL "Path to store PlusLib contents.")
ExternalProject_Add(PlusLibData
  "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
  SOURCE_DIR "${PLUS_PLUSLIBDATA_DIR}" 
  #--Download step--------------
  SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
  SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
  SVN_REPOSITORY https://subversion.assembla.com/svn/plus/branches/rankin-sandbox/PlusLibData
  #--Configure step-------------
  CONFIGURE_COMMAND ""
  #--Build step-----------------
  BUILD_COMMAND ""
  #--Install step-----------------
  INSTALL_COMMAND ""
  )
SET(PLUSLIBDATA_DIR ${PLUS_PLUSLIBDATA_DIR} CACHE PATH "The directory containing PlusLib data" FORCE)