PROJECT(PlusModelCatalog)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# ------------------------------------------------------------------------
# Find Plus use file
IF ( NOT PLUSLIB_DIR )
    FIND_PACKAGE (PlusLib)
ELSE ()
    FIND_PACKAGE (PlusLib REQUIRED PATHS "${PLUSLIB_DIR}/src" NO_DEFAULT_PATH)
ENDIF() 

IF ( PlusLib_FOUND)
  INCLUDE( ${PlusLib_USE_FILE} )
ELSE( PlusLib_FOUND )
    MESSAGE( FATAL_ERROR "This project requires PlusLib for generating rendering images of STL model files. Please verify configuration.")
ENDIF( PlusLib_FOUND )

# Need subversion for the automatic update of the repository
MARK_AS_ADVANCED(CLEAR Subversion_SVN_EXECUTABLE)
FIND_FILE(Subversion_SVN_EXECUTABLE svn${CMAKE_EXECUMODEL_TABLE_SUFFIX} 
  PATHS 
    "c:/Program Files/TortoiseSVN/bin/"
    "c:/Program Files/SlikSvn/bin/"
  )
FIND_PACKAGE(Subversion REQUIRED)

Subversion_WC_INFO("${CMAKE_CURRENT_SOURCE_DIR}" PlusCatalog)
SET(CATALOG_URL "https://app.assembla.com/spaces/plus/subversion/source/${PlusCatalog_WC_REVISION}/trunk/PlusModelCatalog")
SET(HTML_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

INCLUDE(CatalogHtmlWriterMacros.cmake)
INCLUDE(CatalogContents.cmake)

# The solution has to be re-built each time when the contents is modified,
# create a convenience batch file that performs this.
STRING(TOUPPER ${CMAKE_MAKE_PROGRAM} MAKE_PROGRAM_UPPERCASE)
IF (${MAKE_PROGRAM_UPPERCASE} MATCHES "MSBUILD")
  SET(IS_MAKE_PROGRAM_MSBUILD TRUE)
ELSE()
  SET(IS_MAKE_PROGRAM_MSBUILD FALSE)
ENDIF()
CONFIGURE_FILE(
  CreateModelCatalog.bat.in
  ${CMAKE_CURRENT_BINARY_DIR}/CreateModelCatalog.bat
  )
# Batch file for build with dashboard submission
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildAndTest.bat.in 
  ${CMAKE_CURRENT_BINARY_DIR}/BuildAndTest.bat
  )

# --------------------------------------------------------------------------
# Testing
#

# Required for dashboard submission  
INCLUDE(CTest)