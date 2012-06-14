
// PLUS Includes


// BK Includes
#include "AcquisitionGrabberSapera.h"
#include "AcquisitionInjector.h"
#include "AcquisitionSettings.h"
#include "CommandAndControl.h"
#include "ParamConnectionSettings.h"

// Other Includes
#include "PlusBKOEMReceiver.h"

class vtkBKOEMVideoSource::vtkInternal
{
public:
  vtkBKOEMVideoSource External;
  
  // OEM references
  CommandAndControl* BKcmdCtrl
  CmdCtrlSettings BKcmdCtrlSettings;
  ParamConnectionSettings BKparamSettings;
  AcquisitionInjector BKAcqInjector;
  AcquisitionSettings BKAcqSettings;
  AcquisitionGrabberSapera BKAcqSapera;


  vtkBKOEMVideoSource::vtkInternal::vtkInternal(vtkBKOEMVideoSource* external) 
  {
    this->e = external;
  }
  
};

///////// PLUS Video Interface

//----------------------------------------------------------------------------
vtkBKOEMVideoSource::vtkBKOEMVideoSource()
{
  // Changes:
  this->p = new vtkInternal(this);
  this->DataReceiver = new PlusBKOEMDataReceiver();
  
  // End Changes
  
  
  this->Connected = 0;
 
  this->SpawnThreadForRecording=0;

  this->Recording = 0;

  this->FrameRate = 30;

  this->FrameCount = 0;
  this->FrameNumber = 0;

  //this->StartTimeStamp = 0;
  this->FrameTimeStamp = 0;

  this->OutputNeedsInitialization = 1;

  this->NumberOfOutputFrames = 1;

  this->RecordThreader = vtkMultiThreader::New();
  this->RecordThreadId = -1;

  this->CurrentVideoBufferItem = new VideoBufferItem();

  this->Buffer = vtkVideoBuffer::New();

  this->UpdateWithDesiredTimestamp = 0;
  this->DesiredTimestamp = -1;
  this->TimestampClosestToDesired = -1;

  this->SetNumberOfInputPorts(0);

  this->UsImageOrientation = US_IMG_ORIENT_XX; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBKOEMVideoSource::InternalConnect()
{

	// TODO this->BKIniFileNameInit ..
	this->i->BKcmdCtrl = new CommandAndControl(&parConnectSettings, &cmdCtrlSet);
	
	int numSamples = 0;
	int numLines = 0;

	bool success = this->i->BKcmdCtrl->CalcSaperaBufSize(&numSamples, &numLines);
	if (success)
	{
		success = this->i.CmdCtrlSettings.LoadIni(this->BKIniFilename);
		if (!success)
			{
			// ?plus log function? error..
			}
		this->i.CmdCtrlSettings.SetRFLineLength(numSamples);
		this->i.CmdCtrlSettings.SetLinesPerFrame(numLines);
		this->i.CmdCtrlSettings.SetFramesToGrab(0); // continuous

		success = this->i.BKAcqSapera.Init(this->i.CmdCtrlSettings));
		if () {} //TODO
		else { return }

		// TODO: init
		this->DataReceiver->SetDataCallback(NewFrameCallback);
		this->i.BKAcqInjector.AddDataReceiver(this->DataReceiver);
	}
	


}

PlusStatus vtkBKOEMVideoSource::InternalDisconnect()
{
}

//----------------------------------------------------------------------------
bool vtkBKOEMVideoSource::NewFrameCallback(void * data, int type, int sz, int frmnum)
{    
	if(data==NULL || sz==0)
	{
    LOG_DEBUG("Error: no actual frame data received"); 
    return false;
	}

	vtkBKOEMVideoSource::GetInstance()->Buffer->AddItem(data,
				this->GetUsImageOrientation(), sz, 
				type, 0, ?FrameNumber?);    // ?? TODO

	return true;;
}