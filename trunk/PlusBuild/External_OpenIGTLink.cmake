IF(OpenIGTLink_DIR)

  # OpenIGTLink has been built already
  FIND_PACKAGE(OpenIGTLink REQUIRED NO_MODULE)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "PLUS requires a build of OpenIGTLink with v3 support enabled. Please point OpenIGTLink_DIR to an implementation with v3 support.")
  ENDIF()

  MESSAGE(STATUS "Using OpenIGTLink available at: ${OpenIGTLink_DIR}")
  
  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF ( ${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    FILE(COPY 
      ${OpenIGTLink_LIBRARY_DIRS}/Release/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Release
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
    FILE(COPY 
      ${OpenIGTLink_LIBRARY_DIRS}/Debug/
      DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}/Debug
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )    
  ELSE()
      FILE(COPY 
        ${OpenIGTLink_LIBRARY_DIRS}/
        DESTINATION ${PLUS_EXECUTABLE_OUTPUT_PATH}
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
  ENDIF()
  SET (PLUS_OpenIGTLink_DIR "${OpenIGTLink_DIR}" CACHE INTERNAL "Path to store OpenIGTLink binaries")
  
ELSE()

  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading OpenIGTLink from ${GIT_PROTOCOL}://github.com/openigtlink/OpenIGTLink.git.")
  
  SET (PLUS_OpenIGTLink_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink")
  SET (PLUS_OpenIGTLink_DIR "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink-bin" CACHE INTERNAL "Path to store OpenIGTLink binaries")
  ExternalProject_Add( OpenIGTLink
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink-prefix"
    SOURCE_DIR "${PLUS_OpenIGTLink_SRC_DIR}"
    BINARY_DIR "${PLUS_OpenIGTLink_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/openigtlink/OpenIGTLink.git"
    GIT_TAG "dcdb5b93eba56aebf84afbca69e972f261c20626"
    #--Configure step-------------
    CMAKE_ARGS 
        ${ep_common_args}
        -DLIBRARY_OUTPUT_PATH:STRING=${PLUS_EXECUTABLE_OUTPUT_PATH}
        -DBUILD_SHARED_LIBS:BOOL=${PLUSBUILD_BUILD_SHARED_LIBS}
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_TESTING:BOOL=OFF
        -DOpenIGTLink_PROTOCOL_VERSION_2:BOOL=OFF
        -DOpenIGTLink_PROTOCOL_VERSION_3:BOOL=ON
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
    #--Build step-----------------
    BUILD_ALWAYS 1
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${OpenIGTLink_DEPENDENCIES}
    )  
    
ENDIF()