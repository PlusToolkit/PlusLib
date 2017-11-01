PROJECT(MfVideoCaptureLibrary)

SET(${PROJECT_NAME}_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

SET(${PROJECT_NAME}_SRCS
  FormatReader.cpp
  MediaFoundationVideoCaptureApi.cpp
  MediaFoundationVideoDevice.cpp
  MediaFoundationVideoDevices.cpp
  MediaFoundationCaptureLibrary.cpp
  MfVideoCaptureLogger.cpp
  )

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  SET(${PROJECT_NAME}_HDRS
    FormatReader.h
    MediaFoundationVideoCaptureApi.h
    MediaFoundationVideoDevice.h
    MediaFoundationVideoDevices.h
    MediaFoundationCaptureLibrary.h
    MfVideoCaptureLogger.h
    MfVideoCaptureLoggerMacros.h
    )
endif()

SET(Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${${PROJECT_NAME}_INCLUDE_DIRS} ${WINDOWS_SDK_INCLUDE_DIRS} CACHE INTERNAL "")

SET(${PROJECT_NAME}_LIBS
  Evr
  Mf
  Mfplat
  Shlwapi
  Mfreadwrite
  mfuuid
  Strmiids
  )

ADD_LIBRARY(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER Utilities)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_LIBS})
FOREACH(p IN LISTS ${PROJECT_NAME}_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:${PLUSLIB_INCLUDE_INSTALL}> ${WINDOWS_SDK_INCLUDE_DIRS})

# --------------------------------------------------------------------------
# Set external MS project
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} ${PROJECT_NAME} CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_${PROJECT_NAME} ${PROJECT_NAME};${PlusLib_BINARY_DIR}/src/Utilities/${PROJECT_NAME}/${PROJECT_NAME}.vcxproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
PlusLibInstallLibrary(${PROJECT_NAME} ${PROJECT_NAME})