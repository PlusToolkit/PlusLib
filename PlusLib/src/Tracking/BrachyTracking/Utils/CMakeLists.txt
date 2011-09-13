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
    ${CIVCO_SEI_INCLUDE_DIR}
    )	

INCLUDE_DIRECTORIES( ${BrachyStepper_INCLUDE_DIRS} )
SET (Tracking_INCLUDE_DIRS ${Tracking_INCLUDE_DIRS} ${BrachyStepper_INCLUDE_DIRS} CACHE INTERNAL "" )
ADD_LIBRARY(BrachyStepper STATIC ${BrachyStepper_SRCS} ${BrachyStepper_HDRS} )

SET (PLUSLIB_DEPENDENCIES ${PLUSLIB_DEPENDENCIES} BrachyStepper CACHE INTERNAL "" FORCE	)
