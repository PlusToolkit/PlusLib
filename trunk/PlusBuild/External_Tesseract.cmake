SET(tesseract_DEPENDENCIES leptonica)
SET(leptonica_DEPENDENCIES vtk) #for vtkzlib
SET(tesseract_ROOT_DIR ${CMAKE_BINARY_DIR}/Deps)

# --------------------------------------------------------------------------
# leptonica
SET (PLUS_leptonica_src_DIR ${tesseract_ROOT_DIR}/leptonica CACHE INTERNAL "Path to store leptonica contents.")
SET (PLUS_leptonica_prefix_DIR ${tesseract_ROOT_DIR}/leptonica-prefix CACHE INTERNAL "Path to store leptonica prefix data.")
SET (PLUS_leptonica_DIR "${tesseract_ROOT_DIR}/leptonica-bin" CACHE INTERNAL "Path to store leptonica binaries")
ExternalProject_Add( leptonica
    PREFIX ${PLUS_leptonica_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_leptonica_src_DIR}"
    BINARY_DIR "${PLUS_leptonica_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/leptonica.git"
    GIT_TAG master
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DVTK_DIR:PATH=${PLUS_VTK_DIR} #get vtkzlib and vtkpng from vtk
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND "" #don't install
    #--Dependencies-----------------
    DEPENDS ${leptonica_DEPENDENCIES}
    )

# --------------------------------------------------------------------------
# tessdata
SET (PLUS_tessdata_src_DIR ${tesseract_ROOT_DIR}/tessdata CACHE INTERNAL "Path to store tesseract language data contents.")
SET (PLUS_tessdata_prefix_DIR ${tesseract_ROOT_DIR}/tessdata-prefix CACHE INTERNAL "Path to store tesseract language prefix data.")
ExternalProject_Add( tessdata
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX ${PLUS_tessdata_prefix_DIR}
    SOURCE_DIR "${PLUS_tessdata_src_DIR}"
    BINARY_DIR "${PLUS_tessdata_src_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/tessdata.git"
    GIT_TAG master
    #--Configure step-------------
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step-----------------
    #--Dependencies-----------------
    INSTALL_COMMAND ""
    DEPENDS ""
    )
SET( tesseract_DEPENDENCIES ${tesseract_DEPENDENCIES} tessdata )

IF( "$ENV{TESSDATA_PREFIX}" STREQUAL "" OR NOT "$ENV{TESSDATA_PREFIX}" STREQUAL "${PLUS_tessdata_src_DIR}")
  MESSAGE(STATUS "Setting TESSDATA_PREFIX environment variable to enable loading of OCR languages.")
  IF( WIN32 )
    EXECUTE_PROCESS(COMMAND setx TESSDATA_PREFIX ${PLUS_tessdata_src_DIR})
  ELSEIF( UNIX )
    # Linux GUI environment env variables are not affected by this, so if they close cmake-gui
    # and re-open it, this env variable will not be found
    # Perform a more advanced check to see if the export is in the .xsessionrc file, if so, simply 
    # set the ENV for this cmake-gui session
    IF(EXISTS $ENV{HOME}/.xsessionrc )
        FILE(READ $ENV{HOME}/.xsessionrc XSESSIONRC_FILE_CONTENTS)
      STRING(REGEX MATCH "export TESSDATA_PREFIX=(.*)" TESSDATA_PREFIX_MATCH ${XSESSIONRC_FILE_CONTENTS})
      IF( TESSDATA_PREFIX_MATCH )
          IF(NOT ${CMAKE_MATCH_1} STREQUAL ${PLUS_tessdata_src_DIR})
        # modify .xsessionrc file
        STRING(REPLACE ${TESSDATA_PREFIX_MATCH} "" XSESSIONRC_FILE_CONTENTS ${XSESSIONRC_FILE_CONTENTS})
        FILE(WRITE $ENV{HOME}/.xsessionrc ${XSESSIONRC_FILE_CONTENTS})
        FILE(APPEND $ENV{HOME}/.xsessionrc "export TESSDATA_PREFIX=${PLUS_tessdata_src_DIR}")
          ENDIF()
        ELSE()
        FILE(APPEND $ENV{HOME}/.xsessionrc "export TESSDATA_PREFIX=${PLUS_tessdata_src_DIR}")
        ENDIF()
    ELSE()
        FILE(WRITE $ENV{HOME}/.xsessionrc "export TESSDATA_PREFIX=${PLUS_tessdata_src_DIR}")
    ENDIF()
  ENDIF()

  # Lastly, set it for this cmake session as well
  SET(ENV{TESSDATA_PREFIX} ${PLUS_tessdata_src_DIR})
ELSE()
  MESSAGE(STATUS "Using TESSDATA_PREFIX=$ENV{TESSDATA_PREFIX}.")
ENDIF()

# --------------------------------------------------------------------------
# tesseract-ocr-cmake
SET (PLUS_tesseract_src_DIR ${tesseract_ROOT_DIR}/tesseract CACHE INTERNAL "Path to store tesseract contents.")
SET (PLUS_tesseract_prefix_DIR ${tesseract_ROOT_DIR}/tesseract-prefix CACHE INTERNAL "Path to store tesseract prefix data.")
SET (PLUS_tesseract_DIR "${tesseract_ROOT_DIR}/tesseract-bin" CACHE INTERNAL "Path to store tesseract binaries")
ExternalProject_Add( tesseract
    PREFIX ${PLUS_tesseract_prefix_DIR}
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_tesseract_src_DIR}"
    BINARY_DIR "${PLUS_tesseract_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/tesseract-ocr-cmake.git"
    GIT_TAG cd300ab908ed39c739c1047805df22a4d3cae7f8
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
        -DCMAKE_INSTALL_PREFIX:PATH=${PLUS_tesseract_DIR}
        -DLeptonica_DIR:PATH=${PLUS_leptonica_DIR}
    #--Build step-----------------
    #--Install step-----------------
    #--Dependencies-----------------
    DEPENDS ${tesseract_DEPENDENCIES}
    )
