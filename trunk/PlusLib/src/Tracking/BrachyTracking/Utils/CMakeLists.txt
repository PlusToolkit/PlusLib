PROJECT(BrachyStepper)

SET ( BrachyStepper_SRCS 
  CmsBrachyStepper.cxx
  SerialLine.cxx
  CivcoBrachyStepper.cxx 
)

SET (BrachyStepper_HDRS)
if (WIN32)	
	SET (BrachyStepper_HDRS
		CmsBrachyStepper.h 
		SerialLine.h 
    BrachyStepper.h 
    CivcoBrachyStepper.h
		)
endif (WIN32)	

SET (BrachyStepper_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${USDIGITAL_SEI_INCLUDE_DIR}
    )	

SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} ${BrachyStepper_INCLUDE_DIRS} CACHE INTERNAL "" )

INCLUDE_DIRECTORIES(${BrachyStepper_INCLUDE_DIRS})
ADD_LIBRARY(BrachyStepper STATIC ${BrachyStepper_SRCS} ${BrachyStepper_HDRS} )
TARGET_LINK_LIBRARIES(BrachyStepper vtkPlusCommon)

# --------------------------------------------------------------------------
# Set external MS project 
SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} BrachyStepper CACHE INTERNAL "" FORCE)
LIST(REMOVE_DUPLICATES PLUSLIB_DEPENDENCIES)
# Add this variable to UsePlusLib.cmake.in INCLUDE_PLUSLIB_MS_PROJECTS macro
SET(vcProj_BrachyStepper BrachyStepper "${PlusLib_BINARY_DIR}/src/Tracking/BrachyTracking/Utils/BrachyStepper.vcproj" vtkPlusCommon CACHE INTERNAL "" FORCE)

