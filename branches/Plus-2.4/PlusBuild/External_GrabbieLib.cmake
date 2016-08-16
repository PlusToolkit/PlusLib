# --------------------------------------------------------------------------
# GrabbieLib 
FIND_PATH(PLUS_GRABBIELIB_SOURCE_DIR GrabbieLibInfo.txt 
  PATHS
    "../GrabbieLib-1.1.0"
    "../PLTools/BK/ProFocus/GrabbieLib-1.1.0"
    "../../PLTools/BK/ProFocus/GrabbieLib-1.1.0"
    "../trunk/PLTools/BK/ProFocus/GrabbieLib-1.1.0"
    "${CMAKE_CURRENT_BINARY_DIR}/PLTools/BK/ProFocus/GrabbieLib-1.1.0"
  DOC "Path to the BK GrabbieLib source directory."    
)

SET(GRABBIELIB_ADDITIONAL_SDK_ARGS)

IF("${PLUS_GRABBIELIB_SOURCE_DIR}" STREQUAL "PLUS_GRABBIELIB_SOURCE_DIR-NOTFOUND")
  MESSAGE(FATAL_ERROR "The PLUS_GRABBIELIB_SOURCE_DIR must be defined to enable BK ultrasound scanner support. Please verify configuration or turn off PLUS_USE_BKPROFOCUS_VIDEO.")
ENDIF()

# Find the DALSA Sapera framegrabber SDK files from here to let the user fully configure all the external dependencies now
SET(CMAKE_MODULE_PATH
  ${PLUS_GRABBIELIB_SOURCE_DIR}
  ${CMAKE_MODULE_PATH}
  ) 
IF (PLUS_USE_BKPROFOCUS_CAMERALINK)
  FIND_PACKAGE (DALSASAPERA)
  IF (NOT DALSASAPERA_FOUND)
    MESSAGE( FATAL_ERROR "This project requires Dalsa Sapera SDK for BK ProFocus support. Please verify configuration or turn off PLUS_USE_BKPROFOCUS_CAMERALINK.")
  ENDIF()
  SET(GRABBIELIB_ADDITIONAL_SDK_ARGS ${GRABBIELIB_ADDITIONAL_SDK_ARGS}
    -DDALSASAPERA_DIR:PATH=${DALSASAPERA_DIR}
    -DDALSASAPERA_LIB_DIR:PATH=${DALSASAPERA_LIB_DIR}    
    )
ENDIF (PLUS_USE_BKPROFOCUS_CAMERALINK)

ExternalProject_Add( GrabbieLib
  PREFIX "${CMAKE_BINARY_DIR}/Deps/GrabbieLib-prefix"
  SOURCE_DIR "${PLUS_GRABBIELIB_SOURCE_DIR}" 
  BINARY_DIR "Deps/GrabbieLib-bin"
  #--Download step--------------
  DOWNLOAD_COMMAND ""
  #--Configure step-------------
  CMAKE_ARGS    
    -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
    -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
    -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    -DGRABBIE_USE_CAMERALINK:BOOL=${PLUS_USE_BKPROFOCUS_CAMERALINK}    
    ${GRABBIELIB_ADDITIONAL_SDK_ARGS}
  #--Build step-----------------
  BUILD_ALWAYS 1
  #--Install step-----------------
  INSTALL_COMMAND ""
  )

SET (PLUS_GRABBIELIB_DIR "${CMAKE_BINARY_DIR}/GrabbieLib-bin" CACHE INTERNAL "Path to store GrabbieLib binaries")
