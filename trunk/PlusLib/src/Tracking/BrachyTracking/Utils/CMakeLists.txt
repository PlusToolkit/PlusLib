PROJECT(BrachyStepper)

SET ( BrachyStepper_SRCS 
  CmsBrachyStepper.cxx
  SerialLine.cxx
  BrachyStepper.cxx 
  CivcoBrachyStepper.cxx 
)

SET (BrachyStepper_HDRS)
if (WIN32)	
	SET (BrachyStepper_HDRS
		CmsBrachyStepper.h 
		SerialLine.h 
		vtkStepperWin32Header.h 
        BrachyStepper.h 
        CivcoBrachyStepper.h
		)
endif (WIN32)	

ADD_LIBRARY(BrachyStepper STATIC ${BrachyStepper_SRCS} ${BrachyStepper_HDRS})
