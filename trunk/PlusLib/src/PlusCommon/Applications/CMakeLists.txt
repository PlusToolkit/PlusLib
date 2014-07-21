#--------------------------------------------------------------------------------------------
ADD_EXECUTABLE(EditSeqMetaFile EditSeqMetaFile.cxx )
TARGET_LINK_LIBRARIES(EditSeqMetaFile vtkPlusCommon )
GENERATE_HELP_DOC(EditSeqMetaFile)

# --------------------------------------------------------------------------
# Install
#

INSTALL(TARGETS EditSeqMetaFile
  DESTINATION bin
  COMPONENT RuntimeExecutables
  )
