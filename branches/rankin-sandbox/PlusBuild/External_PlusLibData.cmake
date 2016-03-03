#--------------------------------------------------------------------------
# PlusLibData

IF( "${PLUSBUILD_PlusDATA_SVNREVISION}" STREQUAL "HEAD" )
  SET(PLUSLIBDATA_SVN_REV)
ELSE()
  SET(PLUSLIBDATA_SVN_REV "SVN_REVISION ${PLUSBUILD_PlusDATA_SVNREVISION}")
ENDIF()

SET (PLUS_PLUSLIBDATA_DIR ${CMAKE_BINARY_DIR}/PlusLibData CACHE INTERNAL "Path to store PlusLib contents.")
ExternalProject_Add(PlusLibData
  "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
  SOURCE_DIR "${PLUS_PLUSLIBDATA_DIR}" 
  #--Download step--------------
  SVN_USERNAME ${PLUSBUILD_ASSEMBLA_USERNAME}
  SVN_PASSWORD ${PLUSBUILD_ASSEMBLA_PASSWORD}
  SVN_REPOSITORY https://subversion.assembla.com/svn/plus/branches/rankin-sandbox/PlusLibData
  ${PLUSLIBDATA_SVN_REV}
  #--Configure step-------------
  CONFIGURE_COMMAND ""
  #--Build step-----------------
  BUILD_COMMAND ""
  #--Install step-----------------
  INSTALL_COMMAND ""
  )
SET(PLUSLIB_DATA_DIR ${PLUS_PLUSLIBDATA_DIR} CACHE PATH "The directory containing PlusLib data" FORCE)