PROJECT(AMSStepper)

SET ( AMSStepper_SRCS 
  AMSStepper.cxx
  SerialLine.cxx
)

SET (AMSStepper_HDRS)
if (WIN32)	
	SET (AMSStepper_HDRS
		AMSStepper.h 
		SerialLine.h 
		vtkStepperWin32Header.h 
		)
endif (WIN32)	

ADD_LIBRARY(vtkAMSStepper STATIC ${AMSStepper_SRCS} ${AMSStepper_HDRS})
