# --------------------------------------------------------------------------
# Sources

SET ( PlusCommon_SRCS 
		vtkAccurateTimer.cxx 
		PlusLogger.cxx
		vtkHTMLGenerator.cxx 
		vtkGnuplotExecuter.cxx
		)

SET (PlusCommon_HDRS)
if (WIN32)	
	SET (PlusCommon_HDRS
		PlusCommon.h
		vtkAccurateTimer.h
		WindowsAccurateTimer.h
		PlusLogger.h
		vtkHTMLGenerator.h
		vtkGnuplotExecuter.h
		)
endif (WIN32)	

IF (BUILD_TESTING)
	SUBDIRS (Testing)
ENDIF (BUILD_TESTING)

SET (External_Libraries_Install)

# --------------------------------------------------------------------------
# Build the library

SET (PlusCommon_LIBS ${PlusCommon_LIBS} )

if (WIN32)	
	SET (PlusCommon_LIBS ${PlusCommon_LIBS} Winmm )
endif (WIN32)

ADD_LIBRARY(vtkPlusCommon ${PlusCommon_SRCS} ${PlusCommon_HDRS})

TARGET_LINK_LIBRARIES(	vtkPlusCommon vtkCommon vtkHybrid vtkFiltering 
						${PlusCommon_LIBS}
						)

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
	COMPONENT Runtime
	)
	
INSTALL(TARGETS vtkPlusCommon
  DESTINATION bin
  COMPONENT Runtime)
