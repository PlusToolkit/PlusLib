IF(ITK_DIR)

  # ITK has been built already
  FIND_PACKAGE(ITK REQUIRED PATHS ${ITK_DIR} NO_DEFAULT_PATH)
  
  MESSAGE(STATUS "Using ITK available at: ${ITK_DIR}")
  
  IF ("${ITK_LIBRARY_DIRS}" STREQUAL "")
    # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
    SET(ITK_LIBRARY_DIRS "${ITK_DIR}/bin")
  ENDIF()
  
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
    FILE(COPY 
      ${ITK_LIBRARY_DIRS}/Release/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY 
      ${ITK_LIBRARY_DIRS}/Debug/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )    
  ELSE()
    FILE(COPY 
      ${ITK_LIBRARY_DIRS}/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
  SET (PLUS_ITK_DIR "${ITK_DIR}" CACHE INTERNAL "Path to store itk binaries")
  
ELSE(ITK_DIR)

  # ITK has not been built yet, so download and build it as an external project
  SET (ITKv4_REPOSITORY ${GIT_PROTOCOL}://itk.org/ITK.git)
  SET (ITKv4_GIT_TAG 1866ef42887df677a6197ad11ed0ef6e9b239567) #2013-04-03 

  SET (PLUS_ITK_SRC_DIR "${CMAKE_BINARY_DIR}/itk")
  SET (PLUS_ITK_DIR "${CMAKE_BINARY_DIR}/itk-bin" CACHE INTERNAL "Path to store itk binaries")
  ExternalProject_Add( itk
    SOURCE_DIR "${PLUS_ITK_SRC_DIR}"
    BINARY_DIR "${PLUS_ITK_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${ITKv4_REPOSITORY}"
    GIT_TAG "${ITKv4_GIT_TAG}"
    #--Configure step-------------
    CMAKE_ARGS 
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
      -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS} 
      -DBUILD_TESTING:BOOL=OFF 
      -DBUILD_EXAMPLES:BOOL=OFF
      -DITK_LEGACY_REMOVE:BOOL=ON
      -DKWSYS_USE_MD5:BOOL=ON
      -DITK_USE_REVIEW:BOOL=ON
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DITK_LEGACY_REMOVE:BOOL=ON
      -DKWSYS_USE_MD5:BOOL=ON
    #--Build step-----------------
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${ITK_DEPENDENCIES}
    )

ENDIF(ITK_DIR)
