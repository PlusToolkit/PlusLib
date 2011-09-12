PROJECT(BrachyStepper)

SET(CIVCO_SEI_INCLUDE_DIR ${PLTOOLS_DIR}/CIVCO/SEI/inc CACHE PATH "Path to the CIVCO SEI include directory." FORCE)
SET(CIVCO_SEI_BINARY_DIR ${PLTOOLS_DIR}/CIVCO/SEI/bin CACHE PATH "Path to the CIVCO SEI binary directory." FORCE)
SET(CIVCO_SEI_LIB_DIR ${PLTOOLS_DIR}/CIVCO/SEI/lib CACHE PATH "Path to the CIVCO SEI library directory." FORCE)

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
