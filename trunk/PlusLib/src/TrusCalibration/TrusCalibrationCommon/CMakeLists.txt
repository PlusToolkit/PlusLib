PROJECT( TrusCalibrationCommon )

# --------------------------------------------------------------------------
# Sources

SET (TrusCalibrationCommon_SRCS
	AppServices.cpp 
	Direction.cpp 
	Exception.cpp 
	LinearLeastSquares.cpp 
	Orientation.cpp 
	Position.cpp 
	Properties.cpp 
	StringUtils.cpp 
	TraceUtils.cpp 
	Transform.cpp  
	)

SET (TrusCalibrationCommon_HDRS)	
IF (WIN32)	
	SET (TrusCalibrationCommon_HDRS
		AppServices.h 
		CountedPtr.h 
		Direction.h 
		Exception.h 
		LinearLeastSquares.h 
		Orientation.h 
		Position.h 
		Properties.h 
		StringUtils.h 
		TraceUtils.h 
		Transform.h  
		)
ENDIF (WIN32)


# --------------------------------------------------------------------------
# Build the library

ADD_LIBRARY( vtkTrusCalibrationCommon STATIC ${TrusCalibrationCommon_SRCS} ${TrusCalibrationCommon_HDRS} )

TARGET_LINK_LIBRARIES(vtkTrusCalibrationCommon vtkCommon )
		
