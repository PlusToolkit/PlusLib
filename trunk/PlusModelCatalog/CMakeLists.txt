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
SET(CATALOG_URL "https://www.assembla.com/code/plus/subversion/nodes/${PlusCatalog_WC_REVISION}/trunk/PlusModelCatalog")
SET(HTML_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

INCLUDE(CatalogHtmlWriterMacros.cmake)
INCLUDE(CatalogContents.cmake)
