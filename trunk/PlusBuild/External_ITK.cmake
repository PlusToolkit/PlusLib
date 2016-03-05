IF(ITK_DIR)

  # ITK has been built already
  FIND_PACKAGE(ITK REQUIRED PATHS ${ITK_DIR} NO_DEFAULT_PATH)
  
  MESSAGE(STATUS "Using ITK available at: ${ITK_DIR}")
  
  IF(ITK_FOUND)
    MESSAGE(STATUS "Using ITK available at: ${ITK_DIR}")
    
    # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
    FOREACH(lib ${ITK_LIBRARIES})
      IF(NOT TARGET ${lib})
        continue()
      ENDIF()

      GET_TARGET_PROPERTY(ITK_DEBUG_FILE ${lib} IMPORTED_LOCATION_DEBUG)
      GET_TARGET_PROPERTY(ITK_RELEASE_FILE ${lib} IMPORTED_LOCATION_RELEASE)

      IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
        IF( EXISTS ${ITK_RELEASE_FILE} )
          FILE(COPY ${ITK_RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release)
        ENDIF()
        IF( EXISTS ${ITK_DEBUG_FILE} )
          FILE(COPY ${ITK_DEBUG_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug)
        ENDIF()
      ELSE()
        IF( ITK_DEBUG_FILE EQUAL ITK_RELEASE_FILE AND EXISTS ${ITK_RELEASE_FILE})
          FILE(COPY ${ITK_RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
        ELSE()
          IF( EXISTS ${ITK_RELEASE_FILE} )
            FILE(COPY ${ITK_RELEASE_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
          ENDIF()
          IF( EXISTS ${ITK_DEBUG_FILE} )
            FILE(COPY ${ITK_DEBUG_FILE} DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH})
          ENDIF()
        ENDIF()
      ENDIF()
    ENDFOREACH()
 
    SET (PLUS_ITK_DIR "${ITK_DIR}" CACHE INTERNAL "Path to store itk binaries")
  ENDIF()
  
ELSE(ITK_DIR)

  # ITK has not been built yet, so download and build it as an external project
  SET (ITKv4_REPOSITORY ${GIT_PROTOCOL}://itk.org/ITK.git)
  SET (ITKv4_GIT_TAG 8d58556089399c11d51795d46d6b17c355af95dc) #v4.7.2 from 2015-04-30

  # Strip -std=c++11 and/or -std=c++0x from ep_common_args because ITK doesn't support it
  STRING(REGEX REPLACE "(.*)-std=c\\+\\+..(.*)" "\\1\\2" itk_ep_common_cxx_flags ${ep_common_cxx_flags})

  SET (PLUS_ITK_SRC_DIR "${CMAKE_BINARY_DIR}/itk")
  SET (PLUS_ITK_DIR "${CMAKE_BINARY_DIR}/itk-bin" CACHE INTERNAL "Path to store itk binaries")
  ExternalProject_Add( itk
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    SOURCE_DIR "${PLUS_ITK_SRC_DIR}"
    BINARY_DIR "${PLUS_ITK_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${ITKv4_REPOSITORY}"
    GIT_TAG "${ITKv4_GIT_TAG}"
    #--Configure step-------------
    CMAKE_ARGS 
      ${ep_common_args}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DITK_LEGACY_REMOVE:BOOL=ON
      -DKWSYS_USE_MD5:BOOL=ON
      -DITK_USE_REVIEW:BOOL=ON
      -DCMAKE_CXX_FLAGS:STRING=${itk_ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DITK_LEGACY_REMOVE:BOOL=ON
      -DKWSYS_USE_MD5:BOOL=ON
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${ITK_DEPENDENCIES}
    )

ENDIF(ITK_DIR)
