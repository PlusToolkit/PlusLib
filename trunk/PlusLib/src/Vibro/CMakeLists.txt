
# --------------------------------------------------------------------------
# Options 


# --------------------------------------------------------------------------
# Sources
SET (Vibro_SRCS
	DirectSoundBuffer.cxx
	DirectSoundCapture.cxx
	DirectSoundCaptureBuffer.cxx
	DirectSoundInstance.cxx
	#MCDCController.cxx
	MCDCProgrammer.cxx
	SerialBuffer.cxx
	SerialCommHelper.cxx
	SignalGenerator.cxx
	Timer.cxx
	VibroLib.cxx
	Wave.cxx
	)

SET (Vibro_HDRS)
if (WIN32)	
	SET (Vibro_HDRS
	DirectSoundBuffer.h
	DirectSoundCapture.h
	DirectSoundCaptureBuffer.h
	DirectSoundInstance.h
	MCDCProgrammer.h
	SerialBuffer.h
	SerialCommHelper.h
	SignalGenerator.h
	Timer.h
	VibroLib.h
	Wave.h
		)
endif (WIN32)

# --------------------------------------------------------------------------
# Build the library

SET (Vibro_LIBS 
	${CMAKE_CURRENT_SOURCE_DIR}/dsound${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_CURRENT_SOURCE_DIR}/VibroLib${CMAKE_STATIC_LIBRARY_SUFFIX}
	winmm.lib
	vtkPlusCommon
    )     

ADD_LIBRARY(Vibro ${Vibro_SRCS} ${Vibro_HDRS})
TARGET_LINK_LIBRARIES(Vibro ${Vibro_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} Vibro CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_Vibro Vibro ${PlusLib_BINARY_DIR}/src/Vibro/Vibro.vcproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS Vibro
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)


