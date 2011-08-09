/*=========================================================================

Module:  $RCSfile: vtkCalibDataProcessor.h,v $
Authors: Jan Gumprecht, Haiying Liu, Nobuhiko Hata, Harvard Medical School
Authors: Danielle Pace,
Robarts Research Institute and The University of Western Ontario

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

* Neither the name of Harvard Medical School nor the names of any
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// .NAME vtkCalibDataProcessor - manage the vtk pipeline of SynchroGrab.
// .SECTION Description
// vtkCalibDataProcessor is responsible for the creation and configuration
// of the pipeline used by the application called SynchroGrab.
// Once, the pipeline is properly configured, it also manage the data collection.
// .SECTION Usage
// See SynchroGrab.cxx

#ifndef VTKDATAPROCESSOR_H_
#define VTKDATAPROCESSOR_H_

#include "PlusConfigure.h"

#include <queue>
#include <map>

class vtkMatrix4x4;;
class vtkUltrasoundDataCollector;
class vtkCalibDataSender;
class vtkMutexLock;
class vtkMultiThreader;
class vtkImageData;
class vtkVideoSource2;
class vtkTracker;
class vtkTrackerTool;
class vtkImageExtractComponents;

class vtkCalibDataProcessor : public vtkObject
{
public:
	static vtkCalibDataProcessor *New();
	vtkTypeRevisionMacro(vtkCalibDataProcessor, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Perform the real-time reconstruction
	virtual void SetUSDataCollector(vtkUltrasoundDataCollector *);
	vtkGetObjectMacro(USDataCollector, vtkUltrasoundDataCollector);

	// Description:
	// Sends resulting image data over OpenIGTLink to server
	vtkGetMacro(DataSender, vtkCalibDataSender*);

	// Description:
	// Extracts 0th component of reconstructor output
	vtkGetMacro(ComponentExtractor, vtkImageExtractComponents*);

	// Description:
	// Period with which the reconstructor's output is polled and
	// contents stored in the data sender's buffer
	vtkSetMacro(ProcessPeriod, float);
	vtkGetMacro(ProcessPeriod, float);

	// Description:
	// Have we successfully started processing?
	vtkGetMacro(Processing, bool);

	// Description:
	// Timestamp when we started the data sender
	vtkSetMacro(StartUpTime, double);
	vtkGetMacro(StartUpTime, double);

	// Description:
	// Get the number of seconds we've been operating for
	double GetUpTime();

	// Description:
	// File used for logging
	void SetLogStream(ofstream &LogStream);
	ofstream& GetLogStream();

	// Description:
	// For verbose output
	vtkSetMacro(Verbose, bool);
	vtkGetMacro(Verbose, bool);

	// Description:
	// Start pulling data from the reconstructor's output and sending to the
	// data sender
	int StartProcessing(vtkCalibDataSender * sender);

	// Description:
	// Stop pulling data from the reconstructor's output
	int StopProcessing();

	//----------------------------
	// The following need to be public for the data processor thread
	// Please don't call them directly!

	// Description:
	// Forward data to the data sender
	int ForwardData(vtkImageData * image, int port);

	// Description:
	// Try to delete data in the data sender
	void ResetOldVolume(int dataSenderIndex);

protected:
	vtkCalibDataProcessor();
	~vtkCalibDataProcessor();

	// performs the reconstruction
	vtkUltrasoundDataCollector * USDataCollector;

	// sends images over the OpenIGTLink connection
	vtkCalibDataSender* DataSender;

	// extracts 0th component of reconstructor output
	vtkImageExtractComponents *ComponentExtractor;

	// time between putting subsequent images into the data sender's buffer
	float ProcessPeriod;

	// current status
	bool Processing;
	double StartUpTime;

	//Multithreader to run a thread of collecting and sending data
	vtkMultiThreader *PlayerThreader;
	int PlayerThreadId;

	// logging
	bool Verbose;
	ofstream LogStream;

private:

	vtkCalibDataProcessor(const vtkCalibDataProcessor&);  // Not implemented.
	void operator=(const vtkCalibDataProcessor&);  // Not implemented.
};

#endif /*VTKDATAPROCESSOR_H_*/
