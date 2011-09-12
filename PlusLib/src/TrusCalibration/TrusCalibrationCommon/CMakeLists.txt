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

SET (TrusCalibrationCommon_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} )

# --------------------------------------------------------------------------
# Build the library

SET (TrusCalibration_INCLUDE_DIRS ${TrusCalibration_INCLUDE_DIRS} ${TrusCalibrationCommon_INCLUDE_DIRS} CACHE INTERNAL "" FORCE	)
INCLUDE_DIRECTORIES( ${TrusCalibrationCommon_INCLUDE_DIRS} )
ADD_LIBRARY( vtkTrusCalibrationCommon STATIC ${TrusCalibrationCommon_SRCS} ${TrusCalibrationCommon_HDRS} )
TARGET_LINK_LIBRARIES(vtkTrusCalibrationCommon vtkCommon )

# --------------------------------------------------------------------------
# Set external MS project 
SET(PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} vtkTrusCalibrationCommon CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_vtkTrusCalibrationCommon vtkTrusCalibrationCommon "${PlusLib_BINARY_DIR}/src/TrusCalibration/TrusCalibrationCommon/vtkTrusCalibrationCommon.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE)

		
