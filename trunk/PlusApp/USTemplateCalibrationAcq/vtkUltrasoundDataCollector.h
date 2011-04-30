// .NAME vtkUltrasoundDataCollector - real-time freehand ultrasound reconstruction
// with ECG gating
// .SECTION Description
// vtkFreehandUltrasound2 will incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice.  An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// vtkFreehandUltrasound2, vtkVideoSource2, vtkTracker, vtkTrackerTool

#ifndef __vtkUltrasoundDataCollector_h
#define __vtkUltrasoundDataCollector_h

#include "vtkFreehandUltrasound2Dynamic.h"
#include <vtkstd/vector>

class vtkSignalBox;
class vtkBMPWriter;


struct CalibDataStruct{
	vtkImageData* ImageData;
	vtkMatrix4x4* TProbe2World;
	vtkMatrix4x4* TFrame2RAS;
	vtkMatrix4x4* TFrame2World;

	double Timestamp; 
};

enum FLIP {
	FLIP_NONE = -1,
	FLIP_X = 0, 
	FLIP_Y = 1 
};

class VTK_EXPORT vtkUltrasoundDataCollector : public vtkFreehandUltrasound2Dynamic
{
public:
	static vtkUltrasoundDataCollector *New();
	vtkTypeRevisionMacro(vtkUltrasoundDataCollector, vtkFreehandUltrasound2);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Configure the freehand ultrasound reconstruction according to a summary file
	virtual int ReadSummaryFile(const char *filename);

	// Description:
	// Sets the directory for saved timestamps and/or slices
	virtual void SetSaveInsertedDirectory(const char *directory);

	// Description:
	// Saves the timestamps used to insert the slices if turned on (default off)
	// The directory for saving inserted timestamps and/or slices should be set previously
	virtual void SaveInsertedTimestampsOn();
	virtual void SaveInsertedTimestampsOff();
	vtkGetMacro(SaveInsertedTimestamps, int);

	// Description:
	// Saves the inserted slices if turned on (default off).  Automatically saves
	// the inserted timestamps as well.  The directory for saving inserted timestamps
	// and/or slices should be set previously
	virtual void SaveInsertedSlicesOn();
	virtual void SaveInsertedSlicesOff();
	vtkGetMacro(SaveInsertedSlices, int);

	// Description:
	// Save the slice to .bmp and save the matrix
	void SaveDataToFolder(const char* folder); 

	// Description:
	// Start doing real-time reconstruction from the video source.
	// This will spawn a thread that does the reconstruction in the
	// background.
	virtual void StartRealTimeReconstruction();

	//virtual int SyncronizeUSToTracker();

	vtkMatrix4x4 *GetIndexMatrixHelper(vtkMatrix4x4* tProbe2World, vtkLinearTransform* sliceTransform);
	vtkMatrix4x4 *GetFrameToWorldMatrix(vtkMatrix4x4* tProbe2World, vtkLinearTransform* sliceTransform);
	
	//////////////////////////////////////////////////////////////////////////////////
	// Not protected to be available from the reconstruction thread
	// These are internal functions, don't use them!!!

	//BTX
	// Description:
	// Not protected to be accessible from reconstruction thread - do not use this function
	virtual void ReconstructSlice(double timestamp, vtkImageData* inData);

	//
	//  // Description:
	//  // Do any old inserts before inserting the current slice (not used in base class, but
	//  // used in derived classes), for the reconstruction thread
	//  virtual void ReconstructOldSlice(double timestamp, vtkImageData* inData);
	//
	//  // Description:
	//  // Get the time to sleep, for the reconstruction thread
	//  virtual double GetSleepTime();
	//
	//  // Description:
	//  // Get the timestamp for the video slice, for the reconstruction thread
	//  virtual double CalculateCurrentVideoTime(vtkImageData* inData);
	//
	//  // Description:
	//  // Update the input slice, for the reconstruction thread
	//  virtual int UpdateSlice(vtkImageData* inData);
	//
	//  // Description:
	//  // Update the accumulation buffer, for the reconstruction thread
	//  virtual void UpdateAccumulationBuffers();

	//ETX
	//////////////////////////////////////////////////////////////////////////////////

protected:
	vtkUltrasoundDataCollector();
	~vtkUltrasoundDataCollector();

	// Description:
	// Save the slice to .bmp and save the Transform to Calibdata
	void SaveSliceAndTransform(vtkImageData* inData, vtkMatrix4x4* transMatrix, double timestamp);

	// Description:
	// Stores CalibData
	std::vector<CalibDataStruct> CalibData;  

	// Description:
	// Returns an XMLDataElement describing the freehand object
	virtual vtkXMLDataElement* MakeXMLElement();

	FLIP FlipFrame; 

private:
	vtkUltrasoundDataCollector(const vtkUltrasoundDataCollector&);
	void operator=(const vtkUltrasoundDataCollector&);
};

#endif
