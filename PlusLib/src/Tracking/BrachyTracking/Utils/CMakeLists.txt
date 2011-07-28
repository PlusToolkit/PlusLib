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

SET(CIVCO_SEI_INCLUDE_DIR ${PLTOOLS_DIR}/CIVCO/SEI/inc CACHE PATH "Path to the Ulterius SDK include directory." FORCE)
SET(CIVCO_SEI_BINARY_DIR ${PLTOOLS_DIR}/CIVCO/SEI/bin CACHE PATH "Path to the Ulterius SDK binary directory." FORCE)
SET(CIVCO_SEI_LIB_DIR ${PLTOOLS_DIR}/CIVCO/SEI/lib CACHE PATH "Path to the Ulterius SDK library directory." FORCE)

SET (BrachyStepper_LIBS
    ${CIVCO_SEI_LIB_DIR}/SEIDrv32${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

SET ( External_Libraries_Install ${External_Libraries_Install}
      ${CIVCO_SEI_BINARY_DIR}/SEIDrv32${CMAKE_SHARED_LIBRARY_SUFFIX}
      )

SET (PLUSLIB_INCLUDE_DIRS ${PLUSLIB_INCLUDE_DIRS} ${CIVCO_SEI_INCLUDE_DIR} CACHE INTERNAL "" )

INCLUDE_DIRECTORIES( ${PLUSLIB_INCLUDE_DIRS} )

ADD_LIBRARY(BrachyStepper STATIC ${BrachyStepper_SRCS} ${BrachyStepper_HDRS} ${BrachyStepper_LIBS})
