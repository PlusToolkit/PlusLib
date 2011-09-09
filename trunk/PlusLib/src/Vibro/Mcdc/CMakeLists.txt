
# --------------------------------------------------------------------------
# Options 


# --------------------------------------------------------------------------
# Sources
SET (VibroMcdc_SRCS
	#MCDCController.cxx
	MCDCProgrammer.cxx
	SerialBuffer.cxx
	SerialCommHelper.cxx
	)

SET (VibroMcdc_HDRS)
if (WIN32)	
	SET (VibroMcdc_HDRS
	MCDCProgrammer.h
	SerialBuffer.h
	SerialCommHelper.h
        )
endif (WIN32)

# --------------------------------------------------------------------------
# Build the library

SET (VibroMcdc_LIBS 
	vtkPlusCommon
    )     

ADD_LIBRARY(VibroMcdc ${VibroMcdc_SRCS} ${VibroMcdc_HDRS})
TARGET_LINK_LIBRARIES(VibroMcdc ${VibroMcdc_LIBS})

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} VibroMcdc CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_VibroMcdc VibroMcdc ${PlusLib_BINARY_DIR}/src/Vibro/Mcdc/VibroMcdc.vcproj CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Install
#
INSTALL(TARGETS VibroMcdc
    RUNTIME
    DESTINATION bin
    CONFIGURATIONS Release
    COMPONENT RuntimeLibraries)


