# --------------------------------------------------------------------------
# Options

OPTION (PLUS_USE_HEARTSIGNALBOX "Enable Heart Signal Box" OFF)
OPTION (PLUS_USE_USBECGBOX "Enable USB ECG Signal Box" OFF)

# --------------------------------------------------------------------------
# Sources

SET ( Signaling_SRCS 
		vtkSignalBox.cxx 
		)

SET (Signaling_HDRS)
if (WIN32)	
	SET (Signaling_HDRS
		vtkSignalBox.h
		)
endif (WIN32)	

SET (External_Libraries_Install)

# --------------------------------------------------------------------------
# Heart Signal Box support

SET (HEARTSIGNALBOX_LIBRARY)
IF (PLUS_USE_HEARTSIGNALBOX)
	SET (Signaling_SRCS ${Signaling_SRCS}
         vtkHeartSignalBox.cxx
        )
	IF (WIN32)	
		SET (Signaling_HDRS
			${Signaling_HDRS}
			vtkHeartSignalBox.h
			)
	ENDIF (WIN32)
	SET (HEARTSIGNALBOX_LIBRARY Inpout/inpout32.lib)
	SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
ENDIF (PLUS_USE_HEARTSIGNALBOX)

# --------------------------------------------------------------------------
# USB ECG Signal Box support

SET (USBECGBOX_LIBRARY)
IF (PLUS_USE_USBECGBOX)
	SET ( Signaling_SRCS ${Signaling_SRCS}
        vtkUSBECGBox.cxx
      )
	IF (WIN32)	
		SET (Signaling_HDRS
			${Signaling_HDRS}
			vtkUSBECGBox.h
			)
	ENDIF (WIN32)
	
	SET (USBECGBOX_LIBRARY cbw/cbw32.lib)
	SET ( External_Libraries_Install ${External_Libraries_Install}
		
		)
ENDIF (PLUS_USE_USBECGBOX)

# --------------------------------------------------------------------------
# Build the library

SET (Signaling_LIBS ${Signaling_LIBS} ${USBECGBOX_LIBRARY} ${HEARTSIGNALBOX_LIBRARY})

ADD_LIBRARY(vtkSignaling ${Signaling_SRCS} ${Signaling_HDRS})

TARGET_LINK_LIBRARIES(	vtkSignaling vtkCommon vtkHybrid vtkFiltering vtkPlusCommon
						${Signaling_LIBS}
						)

# --------------------------------------------------------------------------
# Install
#

INSTALL(FILES 
	${External_Libraries_Install}
	DESTINATION bin
	COMPONENT Runtime
	)
	
INSTALL(TARGETS vtkSignaling
  DESTINATION bin
  COMPONENT Runtime)
