IF(OvrvisionPro_DIR)
  # OvrvisionPro has been built already
  FIND_PACKAGE(OvrvisionPro REQUIRED PATHS ${OvrvisionPro_DIR} NO_DEFAULT_PATH)

  MESSAGE(STATUS "Using OvrvisionPro available at: ${OvrvisionPro_DIR}")
  
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY 
      ${OvrvisionPro_LIBRARY_DIRS}/Release/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY 
      ${OvrvisionPro_LIBRARY_DIRS}/Debug/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )    
  ELSE()
      FILE(COPY 
        ${OvrvisionPro_LIBRARY_DIRS}/
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
  ENDIF()
  SET (PLUS_OvrvisionPro_DIR "${OvrvisionPro_DIR}" CACHE INTERNAL "Path to store OvrvisionPro binaries")
  
ELSE(OvrvisionPro_DIR)

  # OvrvisionPro has not been built yet, so download and build it as an external project
  SET (PLUS_OvrvisionPro_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/OvrvisionPro")
  SET (PLUS_OvrvisionPro_DIR "${CMAKE_BINARY_DIR}/Deps/OvrvisionPro-bin" CACHE INTERNAL "Path to store OvrvisionPro binaries")
  ExternalProject_Add( OvrvisionPro
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX "${CMAKE_BINARY_DIR}/Deps/OvrvisionPro-prefix"
    SOURCE_DIR "${PLUS_OvrvisionPro_SRC_DIR}"
    BINARY_DIR "${PLUS_OvrvisionPro_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/PLUSToolkit/OvrvisionPro.git"
    GIT_TAG "master"
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
        -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS}
        -DBUILD_EXAMPLES:BOOL=OFF
        -DOpenCV_DIR:PATH=${PLUS_OpenCV_DIR}
        -DArUco_DIR:PATH=${PLUS_ArUco_DIR}
        -DBUILD_TESTING:BOOL=OFF
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${OvrvisionPro_DEPENDENCIES}
    )  
    
ENDIF(OvrvisionPro_DIR)