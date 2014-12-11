SET(PlusLib_BIN_DIRECTORY "PlusLib_BIN_DIRECTORY-NOT-FOUND" CACHE PATH "Path to PlusLib binary directory" ) 

IF("${PlusLib_BIN_DIRECTORY}" STREQUAL "PlusLib_BIN_DIRECTORY-NOT-FOUND")
  MESSAGE(FATAL_ERROR "Please select PlusLib binary directory first!")
ENDIF()  
            
# ------------------------------------------------------------------------
# Find Plus use file
FIND_PACKAGE (PlusLib PATHS ${PlusLib_BIN_DIRECTORY}/src NO_DEFAULT_PATH)
IF ( PlusLib_FOUND)
  INCLUDE( ${PlusLib_USE_FILE} )
ELSE( PlusLib_FOUND )
  MESSAGE( FATAL_ERROR "This project requires PlusLib. One of these components is missing. Please verify configuration")
ENDIF( PlusLib_FOUND )

add_executable(PhidgetVolumeReconstruction PhidgetVolumeReconstruction.cxx)
target_link_libraries(PhidgetVolumeReconstruction vtkTracking vtkDataCollection vtkPlusCommon)
