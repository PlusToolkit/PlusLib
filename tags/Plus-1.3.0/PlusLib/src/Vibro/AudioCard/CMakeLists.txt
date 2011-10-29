# --------------------------------------------------------------------------
# Sources
SET (VibroAudioCard_SRCS
	DirectSoundBuffer.cxx
	DirectSoundCapture.cxx
	DirectSoundCaptureBuffer.cxx
	DirectSoundInstance.cxx
	SignalGenerator.cxx
	Timer.cxx
	Wave.cxx
	)

SET (VibroAudioCard_HDRS)
if (WIN32)	
	SET (VibroAudioCard_HDRS
  	DirectSoundBuffer.h
  	DirectSoundCapture.h
  	DirectSoundCaptureBuffer.h
  	DirectSoundInstance.h
  	SignalGenerator.h
  	Timer.h
  	Wave.h
    )
endif (WIN32)

SET (VibroAudioCard_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library

SET (VibroAudioCard_LIBS 
	${CMAKE_CURRENT_SOURCE_DIR}/dsound${CMAKE_STATIC_LIBRARY_SUFFIX}
	winmm
	vtkPlusCommon
  )     

SET (Vibro_INCLUDE_DIRS ${Vibro_INCLUDE_DIRS} ${VibroAudioCard_INCLUDE_DIRS} CACHE INTERNAL "" FORCE	)
INCLUDE_DIRECTORIES( ${VibroAudioCard_INCLUDE_DIRS} )
ADD_LIBRARY(VibroAudioCard ${VibroAudioCard_SRCS} ${VibroAudioCard_HDRS})
TARGET_LINK_LIBRARIES(VibroAudioCard ${VibroAudioCard_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} VibroAudioCard CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_VibroAudioCard VibroAudioCard ${PlusLib_BINARY_DIR}/src/Vibro/AudioCard/VibroAudioCard.vcproj vtkPlusCommon CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS VibroAudioCard
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

