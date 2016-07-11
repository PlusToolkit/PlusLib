IF(OpenIGTLink_DIR)

  # OpenIGTLink has been built already
  FIND_PACKAGE(OpenIGTLink REQUIRED PATHS ${OpenIGTLink_DIR} NO_DEFAULT_PATH)

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
  
ELSE(OpenIGTLink_DIR)

  # OpenIGTLink has not been built yet, so download and build it as an external project
  
  SET (PLUS_OpenIGTLink_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink")
  SET (PLUS_OpenIGTLink_DIR "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink-bin" CACHE INTERNAL "Path to store OpenIGTLink binaries")
  ExternalProject_Add( OpenIGTLink
    "${PLUSBUILD_EXTERNAL_PROJECT_CUSTOM_COMMANDS}"
    PREFIX "${CMAKE_BINARY_DIR}/Deps/OpenIGTLink-prefix"
    SOURCE_DIR "${PLUS_OpenIGTLink_SRC_DIR}"
    BINARY_DIR "${PLUS_OpenIGTLink_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_PROTOCOL}://github.com/IGSIO/OpenIGTLink.git"
    GIT_TAG "2ee0f6ffe466d36d0c6b993051a2d45545b384ac"
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
    #--Install step-----------------
    INSTALL_COMMAND ""
    DEPENDS ${OpenIGTLink_DEPENDENCIES}
    )  
    
ENDIF(OpenIGTLink_DIR)
