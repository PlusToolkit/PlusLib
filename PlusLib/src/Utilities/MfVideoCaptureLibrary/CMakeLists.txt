SET(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules 
  ${CMAKE_MODULE_PATH}
  )

find_package(WindowsSDK)

IF( NOT WINSDK7.1_FOUND )
	MESSAGE("Media Foundation Capture library requires Windows SDK 7.1 be installed. It was not found.")
	Return()
ENDIF(NOT WINSDK7.1_FOUND)

PROJECT(MfVideoCaptureLibrary)

SET (MfVideoCaptureLibrary_INCLUDE_DIRS
  ${MfVideoCaptureLibrary_SOURCE_DIR} 
  ${MfVideoCaptureLibrary_BINARY_DIR}
  )

SET(MfVideoCaptureLibrary_SRCS
    FormatReader.cpp 
    MediaFoundationVideoCaptureApi.cpp 
    MediaFoundationVideoDevice.cpp 
    MediaFoundationVideoDevices.cpp 
    RawImage.cpp 
    ImageGrabberThread.cpp 
    ImageGrabber.cpp 
    RawImage.cpp 
	MediaFoundationCaptureLibrary.cpp 
	MfVideoCaptureLogger.cpp 
  )
  
SET (MfVideoCaptureLibrary_HDRS ${MfVideoCaptureLibrary_HDRS}
    FormatReader.h 
    MediaFoundationVideoCaptureApi.h 
    MediaFoundationVideoDevice.h 
    MediaFoundationVideoDevices.h 
    RawImage.h 
    ImageGrabberThread.h 
    ImageGrabber.h 
    RawImage.h 
    MediaFoundationCaptureLibrary.h 
	MfVideoCaptureLogger.h 
	MfVideoCaptureLoggerMacros.h 
	)

INCLUDE_DIRECTORIES( ${MfVideoCaptureLibrary_INCLUDE_DIRS} )
SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${MfVideoCaptureLibrary_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_LIBRARY(MfVideoCaptureLibrary STATIC ${MfVideoCaptureLibrary_SRCS} ${MfVideoCaptureLibrary_HDRS})
TARGET_LINK_LIBRARIES( MfVideoCaptureLibrary )

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MfVideoCaptureLibrary CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MfVideoCaptureLibrary MfVideoCaptureLibrary;"${PlusLib_BINARY_DIR}/src/Utilities/MfVideoCaptureLibrary/MfVideoCaptureLibrary.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib MfVideoCaptureLibrary)
