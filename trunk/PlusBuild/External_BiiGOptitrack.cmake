#/*=========================================================================
#Date: Ag 2015
#Authors include:
#- Eugenio Marinetto [*][ç] emarinetto@hggm.es
#- Laura Sanz [*] lsanz@hggm.es
#- Javier Pascau [*][ç] jpascau@hggm.es
#[*] Laboratorio de Imagen Medica, Hospital Gregorio Maranon - http://image.hggm.es/
#[ç] Departamento de Bioingeniería e Ingeniería Aeroespacial. Universidad Carlos III de Madrid
#=========================================================================*/

IF(BiiGOptitrack_DIR)

  # OpenIGTLink has been built already
  FIND_PACKAGE(BiiGOptitrack REQUIRED PATHS ${BiiGOptitrack_DIR} NO_DEFAULT_PATH)

  MESSAGE(STATUS "Using BiiGOptitrack available at: ${BiiGOptitrack_DIR}")

  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY
      ${BiiGOptitrack_LIBRARY_DIRS}/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY
      ${BiiGOptitrack_LIBRARY_DIRS}/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ELSE()
      FILE(COPY
        ${BiiGOptitrack_LIBRARY_DIRS}/
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
  ENDIF()
  SET (PLUS_BiiGOptitrack_DIR "${BiiGOptitrack_DIR}" CACHE INTERNAL "Path to store BiiGOptitrack binaries")
ELSE(BiiGOptitrack_DIR)

  # BiiGOptitrack has not been built yet, so download and build it as an external project
  SET (PLUS_BiiGOptitrack_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/BiiGOptitrack")
  SET (PLUS_BiiGOptitrack_DIR "${CMAKE_BINARY_DIR}/Deps/BiiGOptitrack-bin" CACHE INTERNAL "Path to store BiiGOptitrack binaries")
  ExternalProject_Add( BiiGOptitrack
    PREFIX "${CMAKE_BINARY_DIR}/Deps/BiiGOptitrack-prefix"
    SOURCE_DIR "${PLUS_BiiGOptitrack_SRC_DIR}"
    BINARY_DIR "${PLUS_BiiGOptitrack_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/BiiGOptitrack"
    GIT_TAG "master"
    #--Configure step-------------
    CMAKE_ARGS
        ${ep_common_args}
        -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
        -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS}
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_TESTING:BOOL=OFF
        -DBiiGOptitrack_USE_FAKE_NPAPI:BOOL=OFF
        -DUSE_CalibrationValidation:BOOL=OFF
        -DUSE_IGTLinkPython:BOOL=OFF
        -DUSE_Python:BOOL=OFF
        -DUSE_OptitrackClient:BOOL=OFF
        -DUSE_OptitrackServer:BOOL=OFF
        -DUSE_TestOptitrack:BOOL=OFF
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DBiiGOptitrack_OpenIGTLink_EXTERNAL_DIR:PATH=${PLUS_OpenIGTLink_DIR}
        -DBiiGOptitrack_ITK_EXTERNAL_DIR:PATH=${PLUS_ITK_DIR}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${BiiGOptitrack_DEPENDENCIES}
    )

ENDIF(BiiGOptitrack_DIR)
