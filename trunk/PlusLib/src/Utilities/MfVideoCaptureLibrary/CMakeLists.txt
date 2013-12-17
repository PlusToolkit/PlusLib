PROJECT(MfVideoCapture)

SET (MfVideoCapture_INCLUDE_DIRS
  ${MfVideoCapture_SOURCE_DIR} 
  ${MfVideoCapture_BINARY_DIR}
  )

SET(MfVideoCapture_SRCS
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
  
SET (MfVideoCapture_HDRS ${MfVideoCapture_HDRS}
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

INCLUDE_DIRECTORIES( ${MfVideoCapture_INCLUDE_DIRS} )
SET (Utilities_INCLUDE_DIRS ${Utilities_INCLUDE_DIRS} ${MfVideoCapture_INCLUDE_DIRS} CACHE INTERNAL "")

ADD_LIBRARY(MfVideoCapture STATIC ${MfVideoCapture_SRCS} ${MfVideoCapture_HDRS})
TARGET_LINK_LIBRARIES( MfVideoCapture )

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} MfVideoCapture CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_MfVideoCapture MfVideoCapture;"${PlusLib_BINARY_DIR}/src/Utilities/MfVideoCapture/MfVideoCapture.vcproj" CACHE INTERNAL "" FORCE)

INSTALL_TARGETS(/lib MfVideoCapture)
