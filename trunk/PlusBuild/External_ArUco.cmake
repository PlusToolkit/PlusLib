IF(ArUco_DIR)
  # ArUco has been built already
  FIND_PACKAGE(ArUco 1.3.0 REQUIRED PATHS ${ArUco_DIR} NO_DEFAULT_PATH)

  IF(ArUco_FOUND)
    MESSAGE(STATUS "Using ArUco available at: ${ArUco_DIR}")
    
    # Copy libraries to EXECUTABLE_OUTPUT_PATH
    FOREACH(lib ${ArUco_LIBRARIES})
      IF(NOT TARGET ${lib})
        continue()
      ENDIF()

      GET_TARGET_PROPERTY(ArUco_DEBUG_FILE ${lib} IMPORTED_LOCATION_DEBUG)
      GET_TARGET_PROPERTY(ArUco_RELEASE_FILE ${lib} IMPORTED_LOCATION_RELEASE)

      IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
        IF( EXISTS ${ArUco_RELEASE_FILE} )
          FILE(COPY ${ArUco_RELEASE_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
        ENDIF()
        IF( EXISTS ${ArUco_DEBUG_FILE} )
          FILE(COPY ${ArUco_DEBUG_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
        ENDIF()
      ELSE()
        IF( ArUco_DEBUG_FILE EQUAL ArUco_RELEASE_FILE AND EXISTS ${ArUco_RELEASE_FILE})
          FILE(COPY ${ArUco_RELEASE_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
        ELSE()
          IF( EXISTS ${ArUco_RELEASE_FILE} )
            FILE(COPY ${ArUco_RELEASE_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
          ENDIF()
          IF( EXISTS ${ArUco_DEBUG_FILE} )
            FILE(COPY ${ArUco_DEBUG_FILE} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
          ENDIF()
        ENDIF()
      ENDIF()
    ENDFOREACH()
 
    SET (PLUS_ArUco_DIR "${ArUco_DIR}" CACHE INTERNAL "Path to store ArUco binaries")
  ENDIF()
  
  SET(PLUS_ArUco_DIR ${ArUco_DIR})
  
ELSE(ArUco_DIR)
  # ArUco has not been built yet, so download and build it as an external project
  SET (ArUco_REPOSITORY ${GIT_PROTOCOL}://github.com/PLUSToolkit/ArUco.git)
  SET (ArUco_GIT_TAG fd24574f002f1dbda35fbdbcfb03c479f89ed4aa)

  SET (ArUco_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/ArUco")
  SET (PLUS_ArUco_DIR "${CMAKE_BINARY_DIR}/Deps/ArUco-bin" CACHE INTERNAL "Path to store ArUco binaries")

  ExternalProject_Add( ArUco
    PREFIX "${CMAKE_BINARY_DIR}/Deps/ArUco-prefix"
    SOURCE_DIR "${ArUco_SRC_DIR}"
    BINARY_DIR "${PLUS_ArUco_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${ArUco_REPOSITORY}"
    GIT_TAG "${ArUco_GIT_TAG}"
    #--Configure step-------------
    CMAKE_ARGS 
      ${ep_common_args}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DOpenCV_DIR:PATH=${PLUS_OpenCV_DIR}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${ArUco_DEPENDENCIES}
    )

ENDIF(ArUco_DIR)