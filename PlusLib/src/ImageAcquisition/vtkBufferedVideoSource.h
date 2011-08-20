#ifndef __vtkBufferedVideoSource_h
#define __vtkBufferedVideoSource_h

#include "PlusConfigure.h"
#include "vtkPlusVideoSource.h"

class VTK_EXPORT vtkBufferedVideoSource;

class VTK_EXPORT vtkBufferedVideoSourceCleanup
{
public:
	vtkBufferedVideoSourceCleanup();
	~vtkBufferedVideoSourceCleanup();
};
//ETX

class VTK_EXPORT vtkBufferedVideoSource : public vtkPlusVideoSource
{
public:
  typedef UsImageConverterCommon::ImageType ImageType; 
  typedef UsImageConverterCommon::PixelType PixelType;

	//static vtkBufferedVideoSource *New();
	vtkTypeRevisionMacro(vtkBufferedVideoSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   
	// Description:
	// This is a singleton pattern New.  There will only be ONE
	// reference to a vtkOutputWindow object per process.  Clients that
	// call this must call Delete on the object so that the reference
	// counting will work.   The single instance will be unreferenced when
	// the program exits.
	static vtkBufferedVideoSource* New();
	// Description:
	// Return the singleton instance with no reference counting.
	static vtkBufferedVideoSource* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(vtkBufferedVideoSource *instance);
	//BTX
	// use this as a way of memory management when the
	// program exits the SmartPointer will be deleted which
	// will delete the Instance singleton
	static vtkBufferedVideoSourceCleanup Cleanup;
	//ETX

	virtual PlusStatus AddFrameToBuffer( ImageType::Pointer image, US_IMAGE_ORIENTATION usImageOrientation, double timestamp); 

protected:
	vtkBufferedVideoSource();
	~vtkBufferedVideoSource();

private:

	static vtkBufferedVideoSource* Instance;
	vtkBufferedVideoSource(const vtkBufferedVideoSource&);  // Not implemented.
	void operator=(const vtkBufferedVideoSource&);  // Not implemented.
};

#endif
