/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkFreehandUltrasound2Dynamic.cxx,v $
Language:  C++
Date:      $Date: 2009/07/20 18:48:07 $
Version:   $Revision: 1.20 $
Thanks:    Thanks to David G. Gobbi who developed this class. 
Thanks:    Thanks to Danielle Pace who developed this class.

==========================================================================

Copyright (c) 2000-2007 Atamai, Inc.
Copyright (c) 2008-2009 Danielle Pace

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

#include "PlusConfigure.h"

#include "vtkFreehandUltrasound2Dynamic.h"
#include "vtkFreehandUltrasound2Helper.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkCriticalSection.h"
#include "vtkMutexLock.h"
#include "vtkSignalBox.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkBMPWriter.h"
#include "vtkTimestampedCircularBuffer.h"
#include "vtkVideoBuffer.h"

vtkCxxRevisionMacro(vtkFreehandUltrasound2Dynamic, "$Revisions: 1.0 $");
vtkStandardNewMacro(vtkFreehandUltrasound2Dynamic);

//----------------------------------------------------------------------------
vtkFreehandUltrasound2Dynamic::vtkFreehandUltrasound2Dynamic()
{
  // triggering
  this->Triggering = 0;
  this->SignalBox = NULL;
  this->Retrospective = 1;
  this->CurrentPhase = -1;
  this->PreviousPhase = -1;
  this->PhaseShift = 0;
  this->ECGRate = 0;
  this->DesiredTime = -1;

  // check heart rate
  this->CheckHeartRate = 0;
  this->SliceBuffer = NULL;
  this->SliceAxesBuffer = NULL;
  this->SliceTransformBuffer = NULL;
  this->SliceTimestampsBuffer = NULL;
  this->NumberInsertedIntoSliceBuffers = 0;

  // deciding on expected heart rate when checking heart rate
  this->ECGMonitoringTime = 10;
  this->NumECGTrials = 5;
  this->PercentageIncreasedHeartRateAllowed = 20;
  this->PercentageDecreasedHeartRateAllowed = 20;
  this->MeanHeartRate = 0;
  this->MaxAllowedHeartRate = +1e8;
  this->MinAllowedHeartRate = 0;
  this->PreviousCyclePassed = 0;

  // waiting for next video frame in prospective gating
  this->InsertOnNextVideoFrame = 0;
  this->PhaseToInsert = -1;
  this->PreviousVideoTime = -1.0;
  this->VideoTime = -1.0;
  this->HalfVideoPeriod = 0.0;

  // choosing only select phases from the signal box to reconstruct
  this->UseSelectPhases = 0;
  this->MapPhaseToOutputVolume = NULL;
  this->NumSignalBoxPhases = 0;

  // saving the timestamps and slices of the inserted slices
  this->SaveInsertedTimestamps = 0;
  this->SaveInsertedSlices = 0;
  this->SaveInsertedDirectory = NULL;
  SaveInsertedTimestampsOutputFilename = NULL;
  this->SaveInsertedTimestampsOutputFile = NULL;
  this->SaveInsertedSlicesWriter = NULL;
  this->SaveInsertedTimestampsCounter = 0;

  // no need to call SetupOutputVolumes or SetupAccumulationBuffers:
  // just set number of output volumes here, since accumulator buffer was
  // created in the base class constructor and since the options to check the
  // heart rate and to save inserted timestamps are turned off in the
  // constructor
  // TODO works, but very sketchy as we are calling virtual functions
  // in base class constructor (SetupOutputVolume and SetupAccumulationBuffer)
  // --> should probably put initialize function somewhere
  this->NumberOfOutputVolumes = 1;

}

//----------------------------------------------------------------------------
vtkFreehandUltrasound2Dynamic::~vtkFreehandUltrasound2Dynamic()
{

  this->StopRealTimeReconstruction();
  this->SetSignalBox(NULL);

  this->DeleteAccumulationBuffers();
  this->DeleteSliceBuffers();

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fclose(this->SaveInsertedTimestampsOutputFile);
    this->SaveInsertedTimestampsOutputFile = NULL;
  }

  if (this->SaveInsertedDirectory)
  {
    delete [] this->SaveInsertedDirectory;
    this->SaveInsertedDirectory = NULL;
  }

  if (this->SaveInsertedTimestampsOutputFilename)
  {
    delete [] this->SaveInsertedTimestampsOutputFilename;
    this->SaveInsertedTimestampsOutputFilename = NULL;
  }

  if (this->SaveInsertedSlicesWriter)
  {
    this->SaveInsertedSlicesWriter->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkFreehandUltrasound2Dynamic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Triggering: " << (this->Triggering ? "On\n":"Off\n");
  os << indent << "SignalBox: " << this->SignalBox << "\n";
  if (this->SignalBox)
  {
    this->SignalBox->PrintSelf(os,indent.GetNextIndent());
  }
  if (this->VideoSource)
  {
    os << indent << "VideoUpdatingWithDesiredTimestamps: " << (this->VideoSource->GetUpdateWithDesiredTimestamp() ? "On\n":"Off\n");
  }
  os << indent << "Retrospective: " << (this->Retrospective ? "On\n":"Off\n");
  os << indent << "CurrentPhase: " << this->CurrentPhase << "\n";
  os << indent << "PreviousPhase: " << this->PreviousPhase << "\n";
  os << indent << "PhaseChange: " << this->PhaseShift << "\n";
  os << indent << "ECGRate: " << this->ECGRate << "\n";
  os << indent << "DesiredTime: " << this->DesiredTime << "\n";
  os << indent << "NumberOfOutputVolumes: " << this->NumberOfOutputVolumes << "\n";
  os << indent << "CheckHeartRate: " << (this->CheckHeartRate ? "On\n":"Off\n");
  os << indent << "SliceBuffer: " << this->SliceBuffer << "\n";
  os << indent << "SliceAxesBuffer: " << this->SliceAxesBuffer << "\n";
  os << indent << "SliceTransformBuffer: " << this->SliceTransformBuffer << "\n";
  os << indent << "SliceTimestampsBuffer: " << this->SliceTimestampsBuffer << "\n";
  os << indent << "NumberInsertedIntoSliceBuffers: " << this->NumberInsertedIntoSliceBuffers << "\n";
  os << indent << "ECGMonitoringTime: " << this->ECGMonitoringTime << "\n";
  os << indent << "NumECGTrials: " << this->NumECGTrials << "\n";
  os << indent << "PercentageIncreasedHeartRateAllowed: " << this->PercentageIncreasedHeartRateAllowed << "\n";
  os << indent << "PercentageDecreasedHeartRateAllowed: " << this->PercentageDecreasedHeartRateAllowed << "\n";
  os << indent << "MeanHeartRate: " << this->MeanHeartRate << "\n";
  os << indent << "MaxAllowedHeartRate: " << this->MaxAllowedHeartRate << "\n";
  os << indent << "MinAllowedHeartRate: " << this->MinAllowedHeartRate << "\n";
  os << indent << "PreviousCyclePassed: " << this->PreviousCyclePassed << "\n";
  os << indent << "InsertOnNextVideoFrame: " << this->InsertOnNextVideoFrame << "\n";
  os << indent << "PhaseToInsert: " << this->PhaseToInsert << "\n";
  os << indent << "PreviousVideoTime: " << this->PreviousVideoTime << "\n";
  os << indent << "VideoTime: " << this->VideoTime << "\n";
  os << indent << "HalfVideoPeriod: " << this->HalfVideoPeriod << "\n";
  os << indent << "UseSelectPhases: " << (this->UseSelectPhases ? "On\n":"Off\n");
  os << indent << "NumSignalBoxPhases: " << this->NumSignalBoxPhases << "\n";
  if (this->MapPhaseToOutputVolume)
  {
    if (this->SignalBox)
    {
      os << indent;
      for (int i = 0; i < this->NumSignalBoxPhases; i++)
      {
        os << this->MapPhaseToOutputVolume[i] << " ";
      }
      os << "\n";
    }
  }
  os << indent << "SaveInsertedTimestamps: " << (this->SaveInsertedTimestamps ? "Yes\n":"No\n");
  os << indent << "SaveInsertedSlices" << (this->SaveInsertedSlices ? "On\n":"Off\n");
  //os << indent << "VerboseDebugging: " << (this->VerboseDebugging ? "Yes\n" : "No\n");
}

//****************************************************************************
// BASICS FOR 4D RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// Get the reconstruction volume (for port 0)
// Important that this be for port 0 and not for phase 0, because
// GetIndexMatrixHelper in vtkFreehandUltrasound2 uses GetOutput() to figure
// out output spacing - want to ensure that an output volume exists, even
// if we are selecting which phases to reconstruct and the output for phase 0
// is turned off
vtkImageData *vtkFreehandUltrasound2Dynamic::GetOutput()
{
  return this->GetOutputFromPort(0);
}

//----------------------------------------------------------------------------
// Get the reconstruction volume for a specified phase
vtkImageData *vtkFreehandUltrasound2Dynamic::GetOutputFromPhase(int phase)
{
  int port = this->CalculatePortFromPhase(phase);

  if (port == -1)
  {
    return NULL;
  }

  return this->GetOutputFromPort(port);
}

//----------------------------------------------------------------------------
// Get the reconstruction volume for a specified port
vtkImageData *vtkFreehandUltrasound2Dynamic::GetOutputFromPort(int port)
{

  if (this->GetOutputDataObject(port))
  {
    return vtkImageData::SafeDownCast(this->GetOutputDataObject(port));
  }
  else
  {
    return NULL;
  }

}

//----------------------------------------------------------------------------
// Get the accumulation buffer (for port 0)
// Will be NULL if we are not compounding
vtkImageData *vtkFreehandUltrasound2Dynamic::GetAccumulationBuffer()
{
  return this->GetAccumulationBufferFromPort(0);
}

//----------------------------------------------------------------------------
// Get the accumulation buffer for a specified phase
// Will be NULL if we are not compounding
vtkImageData *vtkFreehandUltrasound2Dynamic::GetAccumulationBufferFromPhase(int phase)
{
  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return NULL;
  }

  return this->GetAccumulationBufferFromPort(port);
}

//----------------------------------------------------------------------------
// Get the accumulation buffer for a specified port
// Will be NULL if we are not compounding
vtkImageData *vtkFreehandUltrasound2Dynamic::GetAccumulationBufferFromPort(int port)
{
  if (port < this->NumberOfOutputVolumes && this->Compounding)
  {
    return this->AccumulationBuffers[port];
  }
  else
  {
    return NULL;
  }
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
// (basically just calls InternalInternalClearOutput)
void vtkFreehandUltrasound2Dynamic::ClearOutput()
{
  // if we are not currently reconstructing...
  vtkImageData* outData;
  vtkImageData* accData;
  if (this->ReconstructionThreadId == -1)
  {
    this->NeedsClear = 1;
    for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
    {
      outData = this->GetOutputFromPort(port);
      outData->UpdateInformation();
      accData = this->GetAccumulationBufferFromPort(port);
      this->InternalInternalClearOutput(outData, accData);
    }
    // clear the buffers for checking heart rate and saving inserted slice
    // timestamps
    this->ClearBuffers();
  }
  this->Modified();
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
// with no check for whether we are reconstructing
// (basically just calls InternalInternalClearOutput)
void vtkFreehandUltrasound2Dynamic::InternalClearOutput()
{
  vtkImageData* outData;
  vtkImageData* accData;
  for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
  {
    outData = this->GetOutputFromPort(port);
    accData = this->GetAccumulationBufferFromPort(port);
    this->InternalInternalClearOutput(outData, accData);
  }

  // clear the buffers for checking heart rate and saving inserted slice
  // timestamps
  this->ClearBuffers();
}

//****************************************************************************
// SET/GET IMAGING PARAMETERS
//****************************************************************************

//----------------------------------------------------------------------------
// Set compounding setting
void vtkFreehandUltrasound2Dynamic::SetCompounding(int compound)
{

  // ignore if we are already reconstructing
  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  this->Compounding = compound;

  // turn compounding on
  if (compound)
  {
    this->SetupAccumulationBuffers(this->NumberOfOutputVolumes);

    // allocate the accumulation buffers
    this->NeedsClear = 1; // TODO sketchy - clears output when turning compounding from off to on

    /*vtkImageData* accData;
    if (!this->NeedsClear) // because otherwise we will be doing it later anyways
    {
    for (int i = 0; i < this->NumberOfOutputVolumes; i++)
    {
    accData = this->GetAccumulationBuffer(i);
    this->InternalClearOutputHelperForAccumulation(accData);
    }
    }*/

  }

  // turn compounding off
  else
  {
    this->DeleteAccumulationBuffers();
  }
}

//****************************************************************************
// RECONSTRUCTION EXECUTION - BASICS
//****************************************************************************

//----------------------------------------------------------------------------
// The transform matrix supplied by the user converts output coordinates
// to input coordinates.  Instead, to speed up the pixel lookup, the following
// function provides a matrix which converts output pixel indices to input
// pixel indices.
// Saves the result to this->IndexMatrix
// Default is for phase 0

vtkMatrix4x4 *vtkFreehandUltrasound2Dynamic::GetIndexMatrix()
{
  return this->GetIndexMatrix(0);
}

//----------------------------------------------------------------------------
// The transform matrix supplied by the user converts output coordinates
// to input coordinates.  Instead, to speed up the pixel lookup, the following
// function provides a matrix which converts output pixel indices to input
// pixel indices.
// Saves the result to this->IndexMatrix
// For a specific phase
vtkMatrix4x4 *vtkFreehandUltrasound2Dynamic::GetIndexMatrix(int phase)
{
  // if we are not triggering or using retrospective gating, then use the current slice axes
  // and slice buffer
  if (!this->Triggering || this->Retrospective)
  {
    return this->GetIndexMatrixHelper(this->GetSliceAxes(), this->SliceTransform);
  }
  // for prospective gating, use the current slice axes and slice buffer if we are not checking heart rate
  else
  {
    if (!this->CheckHeartRate)
    {
      return this->GetIndexMatrixHelper(this->GetSliceAxes(), this->SliceTransform);
    }
    // for prospective gating, if checking heart rate then get current slice axes and slie transform from buffer
    else
    {
      /*if (!this->SliceAxesBuffer)
      {
      LOG_ERROR("GetIndexMatrix(phase) doesn't have a slice axes buffer");
      return NULL;
      }
      if (!this->GetSliceAxesBuffer(phase))
      {
      LOG_ERROR("GetIndexMatrix(phase) doesn't have a slice axes buffer at phase " << phase);
      return NULL;
      }
      if (!this->SliceTransformBuffer)
      {
      LOG_ERROR("GetIndexMatrix(phase) doesn't have a slice transform buffer");
      return NULL;
      }
      if (!this->GetSliceTransformBuffer(phase))
      {
      LOG_ERROR("GetIndexMatrix(phase) doesn't have a slice transform buffer at phase " << phase);
      return NULL;
      }*/
      vtkMatrix4x4* sliceAxes = this->GetSliceAxesBuffer(phase);
      vtkLinearTransform* sliceTransform = this->GetSliceTransformBuffer(phase);
      if (sliceAxes && sliceTransform)
      {
        return this->GetIndexMatrixHelper(sliceAxes, sliceTransform);
      }
    }
  }
  return NULL;
}

//****************************************************************************
// STARTING/STOPPING RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// Things to do before non-real-time reconstruction
int vtkFreehandUltrasound2Dynamic::InitializeReconstruction()
{

  // if we are triggering, then make sure that the signal box is setup
  // properly
  if (this->Triggering)
  {
    if (!this->TestBeforeReconstructingWithTriggering())
    {
      return 0;
    }
  }

  for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
  {
    this->GetOutputFromPort(port)->Update();
  }

  // setup more parameters for the beginning of a new reconstruction
  this->CurrentPhase = -1;
  this->PreviousPhase = -1;
  this->PhaseShift = 0;
  //this->FanRotation = 0;
  this->NumberInsertedIntoSliceBuffers = 0;
  this->PreviousCyclePassed = 0;
  this->InsertOnNextVideoFrame = 0;
  this->PhaseToInsert = -1;
  this->PreviousVideoTime = -1;
  this->VideoTime = -1;

  this->VideoBufferUid = this->VideoSource->GetBuffer()->GetOldestItemUidInBuffer(); 

  this->ClearBuffers();

  if (this->VideoSource)
  {
    this->HalfVideoPeriod = 1.0 / this->VideoSource->GetFrameRate() / 2.0;
  }
  else
  {
    this->HalfVideoPeriod = -1;
  }

  if (this->SaveInsertedTimestampsOutputFilename)
  {
    this->SaveInsertedTimestampsOutputFile = fopen(this->SaveInsertedTimestampsOutputFilename,"a");
  }

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fprintf(this->SaveInsertedTimestampsOutputFile, "Slice#\tDesiredTS\t\t\tVideoTS\t\t\tPhase\t\tTrackerTS\t\t\tECGRate\n");
    ((this->Triggering) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Triggering: ON\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Triggering: OFF\n"));
    ((this->Retrospective) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Gating: RETRO\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Gating: PROSP\n"));
    ((this->CheckHeartRate) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Checking Heart Rate: ON\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Checking Heart Rate: OFF\n"));
  }

  //this->SaveInsertedTimestampsCounter = 0;

  // turning updating with desired timestamp on for retrospective
  if (this->VideoSource)
  {
    if (this->Triggering && this->Retrospective)
    {
      this->VideoSource->UpdateWithDesiredTimestampOn();
    }
    else
    {
      this->VideoSource->UpdateWithDesiredTimestampOff();
    }
  }

  return 1;
}

//----------------------------------------------------------------------------
// Things to do before real-time reconstruction
int vtkFreehandUltrasound2Dynamic::InitializeRealTimeReconstruction()
{

  this->Superclass::InitializeRealTimeReconstruction();

  // if we are triggering, then make sure that the signal box is setup
  // properly
  if (this->Triggering)
  {
    if (!this->TestBeforeReconstructingWithTriggering())
    {
      return 0;
    }
  }

  // Calculate the heart rate parameters if we are checking heart rate
  if (this->CheckHeartRate)
  {
    if (!this->CalculateHeartRateParameters())
    {
      LOG_WARNING("Could not calculate mean heart rate - the patient's heart rate is fluctuating too much");
      return 0;
    }
  }

  for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
  {
    if (this->GetOutputFromPort(port))
    {
      this->GetOutputFromPort(port)->Update();
    }
  }

  // setup more parameters for the beginning of a new reconstruction
  this->CurrentPhase = -1;
  this->PreviousPhase = -1;
  this->PhaseShift = 0;
  //this->FanRotation = 0;
  this->NumberInsertedIntoSliceBuffers = 0;
  this->PreviousCyclePassed = 0;
  this->InsertOnNextVideoFrame = 0;
  this->PhaseToInsert = -1;
  this->PreviousVideoTime = -1;
  this->VideoTime = -1;

  this->ClearBuffers();

  if (this->VideoSource)
  {
    this->HalfVideoPeriod = 1.0 / this->VideoSource->GetFrameRate() / 2.0;
  }
  else
  {
    this->HalfVideoPeriod = -1;
  }

  if (this->SaveInsertedTimestampsOutputFilename)
  {
    this->SaveInsertedTimestampsOutputFile = fopen(this->SaveInsertedTimestampsOutputFilename,"a");
  }

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fprintf(this->SaveInsertedTimestampsOutputFile, "Slice#\tDesiredTS\t\t\tVideoTS\t\t\tPhase\t\tTrackerTS\t\t\tECGRate\n");
    ((this->Triggering) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Triggering: ON\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Triggering: OFF\n"));
    ((this->Retrospective) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Gating: RETRO\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Gating: PROSP\n"));
    ((this->CheckHeartRate) ? fprintf(this->SaveInsertedTimestampsOutputFile, "Checking Heart Rate: ON\n") : fprintf(this->SaveInsertedTimestampsOutputFile, "Checking Heart Rate: OFF\n"));
  }

  //this->SaveInsertedTimestampsCounter = 0;

  // turning updating with desired timestamp on for retrospective
  if (this->VideoSource)
  {
    if (this->Triggering && this->Retrospective)
    {
      this->VideoSource->UpdateWithDesiredTimestampOn();
    }
    else
    {
      this->VideoSource->UpdateWithDesiredTimestampOff();
    }
  }

  return 1;
}

//----------------------------------------------------------------------------
// Things to do after non-real-time reconstruction
void vtkFreehandUltrasound2Dynamic::UninitializeReconstruction()
{

  this->Superclass::UninitializeReconstruction();

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fclose(this->SaveInsertedTimestampsOutputFile);
    this->SaveInsertedTimestampsOutputFile = NULL;
  }

  /*if (this->SaveInsertedDirectory)
  {
  delete [] this->SaveInsertedDirectory;
  this->SaveInsertedDirectory = NULL;
  }*/
}

//----------------------------------------------------------------------------
// Things to do after real-time reconstruction
void vtkFreehandUltrasound2Dynamic::UninitializeRealTimeReconstruction()
{
  this->Superclass::UninitializeRealTimeReconstruction();

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fclose(this->SaveInsertedTimestampsOutputFile);
    this->SaveInsertedTimestampsOutputFile = NULL;
  }

  /*if (this->SaveInsertedDirectory)
  {
  delete [] this->SaveInsertedDirectory;
  this->SaveInsertedDirectory = NULL;
  }*/

  // turning updating with desired timestamp off
  if (this->VideoSource)
  {
    if (this->VideoSource->GetUpdateWithDesiredTimestamp())
    {
      this->VideoSource->UpdateWithDesiredTimestampOff();
    }
  }
}

//----------------------------------------------------------------------------
// Makes sure that the signal box is running before starting the reconstruction
// (will start it for you if it's not already running)
int vtkFreehandUltrasound2Dynamic::TestBeforeReconstructingWithTriggering()
{

  if (!this->Triggering)
  {
    return 1;
  }

  if (!this->SignalBox)
  {
    LOG_ERROR("Triggering is set on but there is no signal box");
    return 0;
  }

  // want to make sure the user hasn't switched all signal boxes off
  if (this->NumberOfOutputVolumes < 1)
  {
    LOG_ERROR("The number of output volumes must be at least one");
    return 0;
  }

  // want to make sure that the user hasn't change the parameters of the signal box since setting it
  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    LOG_ERROR("The number of phases in the signal box has changed since it was set");
    return 0;
  }

  // start the signal box if necessary
  if (!this->SignalBox->GetIsStarted())
  {
    LOG_WARNING("Signal box was not started - starting now");
    this->SignalBox->Initialize();
    this->SignalBox->Start();
  }

  // if we still couldn't start...
  if (!this->SignalBox->GetIsStarted())
  {
    LOG_ERROR("Signal box could not be started");
    return 0;
  }


  /*// makes sure we have the videosource setup properly for retrospective gating
  if (this->VideoSource)
  {
  if (this->Retrospective)
  {
  this->VideoSource->UpdateWithDesiredTimestampOn();
  }
  else
  {
  this->VideoSource->UpdateWithDesiredTimestampOff();
  }
  }*/

  return 1;
}

//----------------------------------------------------------------------------
// Setup the output volume (default 1 output volume)
void vtkFreehandUltrasound2Dynamic::SetupOutputVolumes()
{
  this->SetupOutputVolumes(1);
}

//----------------------------------------------------------------------------
// Setup the output volumes, for a specified number of volumes (should
// correspond to the number of phases we have), and sets the slice buffers
// If we are checking heart rates, has the side effect of clearing the old
// slice buffers and recreating them to match the new number of output volumes
// (since the number of phases is changing, it doesn't make any sense to keep
// the old ones and then add more or delete some - so we'll just clear them
// all and recreate them)
void vtkFreehandUltrasound2Dynamic::SetupOutputVolumes(int num)
{

  // sanity check
  if (num <= 0)
  {
    return;
  }

  // there's no use in doing anything
  if (num == this->NumberOfOutputVolumes)
  {
    return;
  }

  // create the output objects via the VTK 5 pipeline
  this->SetNumberOfOutputPorts(num);

  // if we are checking the heart rate, then clear the old slice buffers and
  // recreate them
  if (this->CheckHeartRate)
  {
    this->DeleteSliceBuffers();
    this->CreateSliceBuffers(num);
  }

  // if saving insertion times, then clear the old buffers and recreate,
  // as before
  /*if (this->SaveInsertedTimestamps)
  {
  this->DeleteSliceTimestampBuffers();
  this->CreateSliceTimestampBuffers(num);
  }*/
}

//----------------------------------------------------------------------------
// Setup the accumulation buffer (for 1 accumulation buffer)
void vtkFreehandUltrasound2Dynamic::SetupAccumulationBuffers()
{
  this->SetupAccumulationBuffers(1);
}

//----------------------------------------------------------------------------
// Setup the accumulation buffers, for a specified number of accumuation
// buffers
// Has the side effect of clearing the old accumulation buffers and recreating
// them to match the new number of output volumes
// (since the number of phases is changing, it doesn't make any sense to keep
// the old ones and then add more or delete some - so we'll just clear them
// all and recreate them)
void vtkFreehandUltrasound2Dynamic::SetupAccumulationBuffers(int num)
{
  // sanity check
  if (num <= 0)
  {
    return;
  }

  if (!this->Compounding)
  {
    return;
  }

  // if we are creating the accumulation buffers for the first time
  // This has to go before the check for num == this->NumberOfOutputVolumes
  // so that the accumulation buffer(s) is created when compounding is
  // toggled on
  if (this->AccumulationBuffers == NULL)
  {
    this->AccumulationBuffers = new vtkImageData*[num];
    for (int i = 0; i < num; i++)
    {
      this->AccumulationBuffers[i] = vtkImageData::New();
    }
    return;
  }

  // there's no point in doing anything if...
  if (num == this->NumberOfOutputVolumes)
  {
    return;
  }

  // delete the old accumulation buffer
  this->DeleteAccumulationBuffers();

  // and create the new ones
  this->AccumulationBuffers = new vtkImageData*[num];
  for (int i = 0; i < num; i++)
  {
    this->AccumulationBuffers[i] = vtkImageData::New();
  }
}

//----------------------------------------------------------------------------
// Deletes all of the accumulation buffers (not just clear/setting intensitites
// to zero, this actually deletes the objects!)
void vtkFreehandUltrasound2Dynamic::DeleteAccumulationBuffers()
{

  if (this->AccumulationBuffers)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->AccumulationBuffers[port])
      {
        this->AccumulationBuffers[port]->Delete();
      }
    }
    delete [] this->AccumulationBuffers;
    this->AccumulationBuffers = NULL;
  }
}

//****************************************************************************
// RECONSTRUCTION - OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice, for phase 0
// Optimized by splitting into x,y,z components or with integer math
void vtkFreehandUltrasound2Dynamic::OptimizedInsertSlice(vtkImageData *outData, vtkImageData *accData)
{
  this->OptimizedInsertSlice(outData, accData, 0);
}

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice, for a specified phase
// Optimized by splitting into x,y,z components or with integer math
void vtkFreehandUltrasound2Dynamic::OptimizedInsertSlice(vtkImageData *outData, vtkImageData* accData, int phase)
{
  // for non-triggering and retrospective gating, we are inserting the current slice
  if (!this->Triggering || this->Retrospective)
  {
    this->OptimizedInsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
  }
  // for prospective gating, we are inserting the current slice if we are not checking heart rate
  else
  {
    if (!this->CheckHeartRate)
    {
      this->OptimizedInsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
    }
    // for prospective gating, we are inserting from the buffer if we are checking heart rate
    else
    {
      /*if (!this->SliceBuffer)
      {
      LOG_ERROR("InsertSlice(phase) doesn't have a slice buffer");
      return;
      }
      if (!this->GetSliceBuffer(phase))
      {
      LOG_ERROR("InsertSlice(phase) doesn't have a slice buffer at this phase");
      return;
      }*/
      vtkImageData* slice = this->GetSliceBuffer(phase);
      vtkMatrix4x4* indexMatrix = this->GetIndexMatrix(phase);
      if (slice && indexMatrix)
      {
        this->OptimizedInsertSliceHelper(outData, accData, slice, indexMatrix);
      }
    }
  }

}

//****************************************************************************
// RECONSTRUCTION - NOT OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice, for phase 0
// Non-optimized version

void vtkFreehandUltrasound2Dynamic::InsertSlice(vtkImageData* outData, vtkImageData* accData)
{
  this->InsertSlice(outData, accData, 0);
}

//----------------------------------------------------------------------------
// Given the output volume and the accumulation buffer (if compounding, otherwise
// can be null), execute the filter algorithm to fill the output with the input
// slice, for a specified phase
// Non-optimized version

void vtkFreehandUltrasound2Dynamic::InsertSlice(vtkImageData* outData, vtkImageData* accData, int phase)
{

  // if we are optimizing by either splitting into x, y, z components or with
  // integer math, then run the optimized insert slice function instead
  if (this->GetOptimization())
  {
    this->OptimizedInsertSlice(outData, accData, phase);
    return;
  }

  // for non-triggering and retrospective gating, we are inserting the current slice
  if (!this->Triggering || this->Retrospective)
  {
    this->InsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
  }
  // for prospective gating, we are inserting the current slice if we are not checking heart rate
  else
  {
    if (!this->CheckHeartRate)
    {
      this->InsertSliceHelper(outData, accData, this->GetSlice(), this->GetIndexMatrix());
    }
    // for prospective gating, we are inserting from the buffer if we are checking heart rate
    else
    {
      /*if (!this->SliceBuffer)
      {
      LOG_ERROR("InsertSlice(phase) doesn't have a slice buffer");
      return;
      }
      if (!this->GetSliceBuffer(phase))
      {
      LOG_ERROR("InsertSlice(phase) doesn't have a slice buffer at this phase");
      return;
      }*/
      vtkImageData* slice = this->GetSliceBuffer(phase);
      vtkMatrix4x4* indexMatrix = this->GetIndexMatrix(phase);
      if (slice && indexMatrix)
      {
        this->InsertSliceHelper(outData, accData, slice, indexMatrix);
      }
    }
  }
}

//****************************************************************************
// USED BY THE MAIN RECONSTRUCTION LOOP
//****************************************************************************

//----------------------------------------------------------------------------
// Calculate how much time to sleep for the reconstruction thread
// Rely on the video source for timing if we are not triggering, otherwise
// rely on the signal box time for timing if we are triggering
double vtkFreehandUltrasound2Dynamic::GetSleepTime()
{
  if (!this->Triggering)
  {
    return (1.0 / this->VideoSource->GetFrameRate()); // 0.033
  }
  else
  {
    return (this->SignalBox->GetSleepInterval() / 1000.0); // 0.005
  }
}

//----------------------------------------------------------------------------
// Returns the timestamp for the frame we're interested in
double vtkFreehandUltrasound2Dynamic::CalculateCurrentVideoTime(vtkImageData* inData)
{

  // if we are not triggering, then return the video time that we've calculated in
  // UpdateSlice
  if (!this->Triggering)
  {
    return this->VideoTime;
  }
  // if we are using retrospective gating, then return the video time that we've calculated
  // in UpdateSlice - corresponds to the timestamp within the video buffer that's closest
  // to the desired timestamp calculated with retrospective gating
  else if (this->Retrospective)
  {
    return this->VideoTime;
  }
  // if we are using prospective gating, then return the current video time - will determine
  // if it is at a phase transition later on
  else
  {

    // if the current time will be closer to the videotime for the next video image,
    // then we will wait for the next video image instead of this one
    // (but we will still use the current transformation!)
    // Note all times are in seconds, so we don't have to scale anything
    // for units here
    if (this->HalfVideoPeriod != -1 && this->VideoTime != -1.0 && ((this->SignalBox->GetTimestamp() - this->VideoTime) > this->HalfVideoPeriod))
    {
      this->InsertOnNextVideoFrame = 1;
    }
    else
    {
      this->InsertOnNextVideoFrame = 0;
    }

    return this->VideoTime;
  }

}

//----------------------------------------------------------------------------
// Update the input slice
PlusStatus vtkFreehandUltrasound2Dynamic::UpdateSlice(vtkImageData* inData, int& insertNow)
{
  int phase, prevPhase;
  double currtime;
  insertNow = 0;

  // Note, this is the only place for real-time reconstruction where we update
  // the output from the video source - here, we are actually copying from the
  // video source's buffer into inData

  // not triggering
  if (!this->Triggering)
  {
    if ( this->RealTimeReconstruction )
    {
      inData->Update();
      this->PreviousVideoTime = this->VideoTime;
      this->VideoTime = this->VideoSource->GetFrameTimeStamp();
    }
    else
    {
      if ( this->VideoBufferUid <= this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() )
      {
        VideoBufferItem bufferItem; 
        if ( this->VideoSource->GetBuffer()->GetVideoBufferItem( this->VideoBufferUid, &bufferItem) != ITEM_OK )
        {
          LOG_ERROR("Failed to get video item from buffer with UID: " << this->VideoBufferUid); 
          return PLUS_FAIL; 
        }

        this->PreviousVideoTime = this->VideoTime;
        this->VideoTime = bufferItem.GetTimestamp(0); 

        UsImageConverterCommon::ConvertItkImageToVtkImage(bufferItem.GetFrame(), inData); 
        inData->Modified(); 

        this->VideoBufferUid++; 
      }
      else
      {
        LOG_WARNING("We've reached the last frame in the buffer (Latest Uid: " << this->VideoSource->GetBuffer()->GetLatestItemUidInBuffer() << ")!"); 
        return PLUS_FAIL; 
      }
    }
  }

  // we are using retrospective gating, so set the time that we want to the
  // video source, and get the frame closest to that time
  else if (this->Retrospective)
  {
    phase = this->SignalBox->GetPhase();
    prevPhase = this->GetCurrentPhase();
    this->PreviousPhase = prevPhase;
    this->CurrentPhase = phase;
    this->ECGRate = this->SignalBox->GetBPMRate();

    if (phase != prevPhase)
    {
      this->PhaseShift = 1;
    }

    // note - may change desired timestamp to -1 on failure (i.e. overflowed
    // buffer in either direction
    currtime = this->SignalBox->CalculateRetrospectiveTimestamp(phase);
    this->DesiredTime = currtime;
    this->VideoSource->SetDesiredTimestamp(currtime);

    inData->Update();
    this->PreviousVideoTime = this->VideoTime;
    this->VideoTime = this->VideoSource->GetTimestampClosestToDesired();
  }

  // we are using prospective gating, so get the most current slice from
  // the video source
  else
  {
    phase = this->SignalBox->GetPhase();
    prevPhase = this->GetCurrentPhase();
    this->PreviousPhase = prevPhase;
    this->CurrentPhase = phase;
    this->ECGRate = this->SignalBox->GetBPMRate();

    if (phase != prevPhase)
    {
      this->PhaseShift = 1;
      insertNow = 1;
    }

    inData->Update();
    this->PreviousVideoTime = this->VideoTime;
    this->VideoTime = this->VideoSource->GetFrameTimeStamp();
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
// Update the accumulation buffers (goes through and updates all of them)
void vtkFreehandUltrasound2Dynamic::UpdateAccumulationBuffers()
{
  if (this->Compounding)
  {
    vtkImageData* accData;
    for (int port = 0; port < this->GetNumberOfOutputVolumes(); port++)
    {
      accData = this->GetAccumulationBufferFromPort(port);
      accData->SetUpdateExtentToWholeExtent();
      accData->Update();
    }
  }
}

//----------------------------------------------------------------------------
// Inserts a single slice into the volume
void vtkFreehandUltrasound2Dynamic::ReconstructSlice(double timestamp, vtkImageData* inData)
{

  ////////////////////////////////////////////////////////////////////////
  // we are not triggering at all
  ////////////////////////////////////////////////////////////////////////
  if (!this->Triggering)
  {
    this->InsertSlice(this->GetOutput(), this->GetAccumulationBuffer());

    // save timestamps to buffer
    if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
    {
      this->SaveSlice(inData,0,timestamp);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // we are using retrospective gating
  ////////////////////////////////////////////////////////////////////////
  else if (this->Retrospective)
  {
    // these were set previously in CalculateCurrentVideoTime(),
    // so grab them again here
    int phase = this->GetCurrentPhase();
    int prevPhase = this->GetPreviousPhase();
    double ecgRate = this->GetECGRate();

    // if we are the beginning of a new phase, we will insert this slice
    //if (ecgRate > 0 && prevPhase != -1 && phase != -1 && phase != prevPhase)
    if (ecgRate > 0 && prevPhase != -1 && phase != -1 && this->PhaseShift)
    {

      this->PhaseShift = 0;

      if (this->CheckHeartRate)
      {
        // if we are at the start of a new phase, then insert the slices
        // from the previous cycle into the output if the ECG rate is valid
        if (phase == 0)
        {
          if (ecgRate >= this->MinAllowedHeartRate && ecgRate <= this->MaxAllowedHeartRate)
          {
            this->PreviousCyclePassed = 1;
          }
          else
          {
            this->PreviousCyclePassed = 0;
          }
        }
      }

      // insert the slice, if we are using retrospective gating or if we're using prospective
      // gating and we're using the current video frame instead of waiting for the next one
      if (!this->CheckHeartRate || this->PreviousCyclePassed)
      {

        if (this->VideoSource->GetDesiredTimestamp() == -1)
        {
          return;
        }

        vtkImageData* outData = this->GetOutputFromPhase(phase);
        vtkImageData* accData = this->GetAccumulationBufferFromPhase(phase);
        // This effectively implements the use of selected phases, because outData and accData
        // will be null if phase is not one of the phases we are reconstructing
        if (outData && (accData || !this->Compounding))
        {
          this->InsertSlice(outData, accData, phase);

          // save timestamps to buffer
          if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
          {
            this->SaveSlice(inData,phase, timestamp);
          }

        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // we are using prospective gating
  ////////////////////////////////////////////////////////////////////////
  else
  {
    // these were set previously in CalculateCurrentVideoTime(),
    // so grab them again here
    int phase = this->GetCurrentPhase();
    int prevPhase = this->GetPreviousPhase();
    double ecgRate = this->GetECGRate();

    // if we are the beginning of a new phase, we will insert this slice
    //if (ecgRate > 0 && prevPhase != -1 && phase != -1 && phase != prevPhase)
    if (ecgRate > 0 && prevPhase != -1 && phase != -1 && this->PhaseShift)
    {

      this->PhaseShift = 0;

      if (this->CheckHeartRate)
      {
        // if we are at the start of a new phase, then insert the slices
        // from the previous cycle into the output if the ECG rate is valid
        // and the buffer is full
        if (phase == 0 && this->NumberInsertedIntoSliceBuffers >= this->NumberOfOutputVolumes)
        {
          if (ecgRate >= this->MinAllowedHeartRate && ecgRate <= this->MaxAllowedHeartRate)
          {
            this->PreviousCyclePassed = 1;
          }
          else
          {
            this->PreviousCyclePassed = 0;
          }
        }

        // for validation
        this->DesiredTime = this->SignalBox->CalculateRetrospectiveTimestamp(phase);

        if (this->PreviousCyclePassed)
        {
          vtkImageData* outData = this->GetOutputFromPhase(phase);
          vtkImageData* accData = this->GetAccumulationBufferFromPhase(phase);
          // This effectively implements the use of selected phases, because outData and accData
          // will be null if phase is not one of the phases we are reconstructing
          if (outData && (accData || !this->Compounding))
          {
            this->InsertSlice(outData, accData, phase);

            // save timestamps to buffer
            if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
            {
              this->SaveSlice(inData,phase, this->GetSliceTimestampsBuffer(phase));
            }
          }
        }
      }

      // copy this slice and associated transforms into the buffers
      if (!this->InsertOnNextVideoFrame)
      {
        if (!this->CheckHeartRate)
        {
          vtkImageData* outData = this->GetOutputFromPhase(phase);
          vtkImageData* accData = this->GetAccumulationBufferFromPhase(phase);
          // This effectively implements the use of selected phases, because outData and accData
          // will be null if phase is not one of the phases we are reconstructing
          if (outData && (accData || !this->Compounding))
          {
            this->InsertSlice(outData, accData, phase);

            // save timestamps to buffer
            if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
            {
              this->SaveSlice(inData,phase, timestamp);
            }
          }
        }
        else
        {
          this->SetSliceBuffer(phase, inData);
        }
      }
      else
      {
        if (this->PhaseToInsert != -1)
        {
          LOG_WARNING("warning - dropped frame - when waiting for next video frame");
        }
        this->PhaseToInsert = phase;
      }

      if (this->CheckHeartRate)
      {
        this->SetSliceAxesAndTransformsBuffers(phase, this->GetSliceAxes(), this->GetSliceTransform(), timestamp);
      }
    }
  }
}

//----------------------------------------------------------------------------
// Inserts an old slice (i.e. from memory or a buffer) before we work on the
// current slice - override in derived classes
// will only do anything in this function with prospective gating
void vtkFreehandUltrasound2Dynamic::ReconstructOldSlice(double timestamp, vtkImageData* inData)
{
  int phaseToInsert = this->PhaseToInsert;
  if (phaseToInsert != -1 && this->PreviousVideoTime != -1.0 && this->VideoTime != -1.0 && this->VideoTime != this->PreviousVideoTime)
  {

    // for sanity's sake
    if (this->Retrospective || !this->Triggering)
    {
      LOG_WARNING("Reconstructing old slice with retrospective gating or no triggering- shouldn't be here!\n");
    }

    if (!this->CheckHeartRate)
    {
      vtkImageData* outData = this->GetOutputFromPhase(phaseToInsert);
      vtkImageData* accData = this->GetAccumulationBufferFromPhase(phaseToInsert);
      // This effectively implements the use of selected phases, because outData and accData
      // will be null if phase is not one of the phases we are reconstructing
      if (outData && (accData || !this->Compounding))
      {
        this->InsertSlice(outData, accData, phaseToInsert);

        // save timestamps to buffer
        if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
        {
          this->SaveSlice(inData,phaseToInsert, timestamp);
        }
      }
    }
    else
    {
      this->SetSliceBuffer(phaseToInsert, inData);
    }

    this->PhaseToInsert = -1;
  }
}

//****************************************************************************
// TRIGGERING (ECG-GATING)
//****************************************************************************

//--------------------------------------------------------------------------------
// Toggle triggering on and off, and setup output volumes and accumulation buffer
// accordingly
void vtkFreehandUltrasound2Dynamic::SetTriggering(int i)
{
  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  // going from triggering to no triggering
  if (this->Triggering && !i)
  {
    this->Triggering = i;
    this->SetupOutputVolumes(1);
    this->SetupAccumulationBuffers(1);
    this->NumberOfOutputVolumes = 1;
    this->NumSignalBoxPhases = 0;

    // allocate the output and the accumulation buffer(s)
    this->NeedsClear = 1;
  }

  // going from no triggering to triggering
  else if (!this->Triggering && i)
  {
    this->Triggering = i;
    if (this->SignalBox)
    {

      if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
      {
        return;
      }

      // if we are not selecting phases, then the number of output volumes is the
      // number of phases from the signal box
      if (!this->UseSelectPhases)
      {
        this->SetupOutputVolumes(this->NumSignalBoxPhases);
        this->SetupAccumulationBuffers(this->NumSignalBoxPhases);
        this->NumberOfOutputVolumes = this->NumSignalBoxPhases;
      }
      // if we are selecting phases, then the number of output volumes depends on
      // the array mapping phases to output volumes
      else
      {
        int numOutputVolumes = 0;
        for (int phase = 0; phase < this->NumSignalBoxPhases; phase++)
        {
          if (this->MapPhaseToOutputVolume[phase] != -1)
          {
            numOutputVolumes++;
          }
        }
        this->SetupOutputVolumes(numOutputVolumes);
        this->SetupAccumulationBuffers(numOutputVolumes);
        this->NumberOfOutputVolumes = numOutputVolumes;
      }

      // allocate the output and the accumulation buffer(s)
      this->NeedsClear = 1;
    }
  }
}

//----------------------------------------------------------------------------
// Sets the signal box for the triggering
// Side effect - the number of output volumes is set to the number of phases
void vtkFreehandUltrasound2Dynamic::SetSignalBox(vtkSignalBox *signalBox)
{

  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  if (signalBox == NULL)
  {
    if (this->MapPhaseToOutputVolume)
    {
      delete [] this->MapPhaseToOutputVolume;
    }
    this->NumSignalBoxPhases = 0;

    if ( this->SignalBox != NULL )
    {
      this->SignalBox->Delete(); 
      this->SignalBox = NULL;
    }

    return;
  }

  if (signalBox->GetNumberOfPhases() <= 1)
  {
    LOG_ERROR("The signal box must have at least 2 phases");
    return;
  }

  // changing signal box, but not changing number of output volumes
  if (this->SignalBox && signalBox->GetNumberOfPhases() == this->NumSignalBoxPhases)
  {
    this->SignalBox = signalBox;
    return;
  }

  this->SignalBox = signalBox;
  this->NumSignalBoxPhases = signalBox->GetNumberOfPhases();

  if (this->Triggering)
  {
    this->SetupOutputVolumes(this->NumSignalBoxPhases);
    this->SetupAccumulationBuffers(this->NumSignalBoxPhases);
    this->NumberOfOutputVolumes = this->NumSignalBoxPhases;
  }

  // setup the array mapping phases to output volume indices
  // - by default every phase is recontructed
  if (this->MapPhaseToOutputVolume)
  {
    delete [] this->MapPhaseToOutputVolume;
  }
  this->MapPhaseToOutputVolume = new int[this->NumSignalBoxPhases];
  for (int phase = 0; phase < this->NumSignalBoxPhases; phase++)
  {
    this->MapPhaseToOutputVolume[phase] = phase;
  }

  // allocate the output and the accumulation buffer
  if (this->Triggering)
  {
    this->NeedsClear = 1;
  }

}

//----------------------------------------------------------------------------
// If retrospective is false, then we're using prospective gating
void vtkFreehandUltrasound2Dynamic::SetRetrospective(int retro)
{

  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  this->Retrospective = retro;

  /*// turning retrospective gating on
  if (retro)
  {
  if (this->VideoSource)
  {
  this->VideoSource->UpdateWithDesiredTimestampOn();
  }
  }
  // turning retrospective gating off
  else
  {
  if (this->VideoSource)
  {
  this->VideoSource->UpdateWithDesiredTimestampOff();
  this->VideoSource->SetDesiredTimestamp(-1);
  }
  }*/
}

//****************************************************************************
// CHECKING HEART RATE
//****************************************************************************

//----------------------------------------------------------------------------
// Sets whether or not we are discarding outlier heart rates, and creates/
// deletes the slice buffers as necessary
void vtkFreehandUltrasound2Dynamic::SetCheckHeartRate(int discard)
{

  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  // sanity check - don't do anything if settings haven't changed
  if ((discard && this->CheckHeartRate) || (!discard && !this->CheckHeartRate))
  {
    return;
  }

  // create the buffers if we are discarding, and delete them if not
  if (discard)
  {
    this->CreateSliceBuffers(this->NumberOfOutputVolumes);
  }
  if (!discard)
  {
    this->DeleteSliceBuffers();
  }

  this->CheckHeartRate = discard;

}

//----------------------------------------------------------------------------
// Inserts an image into the buffer, for when checking heart rate
void vtkFreehandUltrasound2Dynamic::SetSliceBuffer(int phase, vtkImageData* inData)
{

  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return;
  }

  this->SliceBuffer[port]->SetUpdateExtent(inData->GetWholeExtent()); // TODO clip extent implementation - clip extent?
  this->SliceBuffer[port]->DeepCopy(inData);
}

//----------------------------------------------------------------------------
// Gets an image from the slice buffer, for when checking heart rate
vtkImageData* vtkFreehandUltrasound2Dynamic::GetSliceBuffer(int phase)
{

  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return NULL;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return NULL;
  }

  return this->SliceBuffer[port];
}

//----------------------------------------------------------------------------
// Inserts a matrix into the slice axes buffer and a transform into the slice
// transform buffer, for when checking heart rate
void vtkFreehandUltrasound2Dynamic::SetSliceAxesAndTransformsBuffers(int phase, vtkMatrix4x4* sliceAxesMatrix, vtkLinearTransform *sliceTransform, double timestamp)
{

  // sanity check
  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return;
  }

  this->SliceAxesBuffer[port]->DeepCopy(sliceAxesMatrix);
  this->SliceTransformBuffer[port]->DeepCopy(sliceTransform);
  this->SliceTimestampsBuffer[port] = timestamp;
  this->NumberInsertedIntoSliceBuffers++;
}

//----------------------------------------------------------------------------
// Gets a slice axes matrix from the buffer, for when checking heart rate
vtkMatrix4x4* vtkFreehandUltrasound2Dynamic::GetSliceAxesBuffer(int phase)
{

  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return NULL;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return NULL;
  }

  return this->SliceAxesBuffer[port];
}

//----------------------------------------------------------------------------
// Gets a slice transform from the buffer, for when checking heart rate
vtkLinearTransform* vtkFreehandUltrasound2Dynamic::GetSliceTransformBuffer(int phase)
{

  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return NULL;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return NULL;
  }

  return this->SliceTransformBuffer[port];
}

//----------------------------------------------------------------------------
// Gets a slice timestamp from the buffer, for when checking heart rate
double vtkFreehandUltrasound2Dynamic::GetSliceTimestampsBuffer(int phase)
{
  if (!this->CheckHeartRate || !this->SliceBuffer)
  {
    return -1;
  }

  int port = this->CalculatePortFromPhase(phase);
  if (port == -1)
  {
    return -1;
  }

  return this->SliceTimestampsBuffer[port];
}

//----------------------------------------------------------------------------
// Creates buffers for slice images, slice axes and slice transforms
void vtkFreehandUltrasound2Dynamic::CreateSliceBuffers(int num)
{
  this->SliceBuffer = new vtkImageData*[num];
  this->SliceAxesBuffer = new vtkMatrix4x4*[num];
  this->SliceTransformBuffer = new vtkLinearTransform*[num];
  this->SliceTimestampsBuffer = new double[num];
  for (int i = 0; i < num; i++)
  {
    this->SliceBuffer[i] = vtkImageData::New();
    this->SliceAxesBuffer[i] = vtkMatrix4x4::New();
    this->SliceTransformBuffer[i] = vtkTransform::New();
  }
}

//----------------------------------------------------------------------------
// Clears buffers for slice images, slice axes and slice transforms, and for
// inserting timestamps
void vtkFreehandUltrasound2Dynamic::ClearBuffers()
{

  if (this->SliceBuffer)
  {
    int *inExtent = this->GetSlice()->GetExtent(); // TODO clip extent implementation - clip extent?
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->SliceBuffer[port])
      {
        this->SliceBuffer[port]->SetExtent(inExtent);
        this->SliceBuffer[port]->AllocateScalars();
        void *slicePtr = this->SliceBuffer[port]->GetScalarPointerForExtent(inExtent);
        memset(slicePtr,0,((inExtent[1]-inExtent[0]+1)*
          (inExtent[3]-inExtent[2]+1)*
          (inExtent[5]-inExtent[4]+1)*
          this->SliceBuffer[port]->GetScalarSize()*this->SliceBuffer[port]->GetNumberOfScalarComponents()));
      }
    }
  }

  if (this->SliceAxesBuffer)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->SliceAxesBuffer[port])
      {
        this->SliceAxesBuffer[port]->Identity();
      }
    }
  }

  /*if (this->SliceTransformBuffer)
  {
  for (int port = 0; port < this->NumberOfOutputVolumes; port++)
  {
  if (this->SliceTransformBuffer[port])
  { 
  this->SliceTransformBuffer[port]->GetMatrix()->Identity(); // buggy
  }
  }
  }*/

  if (this->SliceTimestampsBuffer)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      this->SliceTimestampsBuffer[port] = 0;
    }
  }

  this->NumberInsertedIntoSliceBuffers = 0;
}

//----------------------------------------------------------------------------
// Actually deletes buffers for slice images, slice axes and slice transforms
void vtkFreehandUltrasound2Dynamic::DeleteSliceBuffers()
{
  if (this->SliceBuffer)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->SliceBuffer[port])
      {
        this->SliceBuffer[port]->Delete();
      }
    }
    delete [] this->SliceBuffer;
    this->SliceBuffer = NULL;
  }

  if (this->SliceAxesBuffer)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->SliceAxesBuffer[port])
      {
        this->SliceAxesBuffer[port]->Delete();
      }
    }
    delete [] this->SliceAxesBuffer;
    this->SliceAxesBuffer = NULL;
  }

  if (this->SliceTransformBuffer)
  {
    for (int port = 0; port < this->NumberOfOutputVolumes; port++)
    {
      if (this->SliceTransformBuffer[port])
      {
        this->SliceTransformBuffer[port]->Delete();
      }
    }
    delete [] this->SliceTransformBuffer;
    this->SliceTransformBuffer = NULL;
  }

  if (this->SliceTimestampsBuffer)
  {
    delete [] this->SliceTimestampsBuffer;
    this->SliceTimestampsBuffer = NULL;
  }
}

//----------------------------------------------------------------------------
// Calculate the expected heart rate, for when checking heart rates
// Expected heart rate = mean heart rate over the time specified by 
// ECGMonitoringTime, as long as the heart rate never goes beyond the bounds
// specified by PercentageIncreasedHeartRateAllowed and
// PercentageDecreasedHeartRateAllowed over the scan time.  If it does,
// then it tries to calculate the expected heart rate again, for a maximum
// number of trials specified by NumECGTrials
int vtkFreehandUltrasound2Dynamic::CalculateHeartRateParameters()
{

  if (!this->CheckHeartRate || !this->Triggering || !this->SignalBox)
  {
    return 0;
  }

  int haveConsistentHeartRateMeasurement = 0;
  double valsPerSecond = 10.0;
  int numVals = valsPerSecond * this->ECGMonitoringTime;
  double sleepTime = 1.0 / valsPerSecond;
  int i;
  double thisHR, meanHR, maxHR, minHR;
  double maxAllowedHR, minAllowedHR;
  int numTrials = 0;

  // Calculate the mean heart rate and make sure it's consistent enough before
  // accepting it
  while (!haveConsistentHeartRateMeasurement && numTrials < this->NumECGTrials)
  {
    printf("starting trial %d\n", numTrials);
    meanHR = 0;
    minHR = 1000.0; // something unbelievably high so that the first test will pass
    maxHR = 0.0; // something unbelievably low so that the first test will pass
    for (i = 0; i < numVals; i++)
    {
      thisHR = this->SignalBox->GetBPMRate();
      if (thisHR != -1)
      {
        meanHR += thisHR;
        if (thisHR < minHR)
        {
          minHR = thisHR;
        }
        if (thisHR > maxHR)
        {
          maxHR = thisHR;
        }
      }
      vtkSleep(sleepTime);
    }
    meanHR = meanHR / numVals;

    maxAllowedHR = meanHR + (meanHR * this->PercentageIncreasedHeartRateAllowed / 100.0);
    minAllowedHR = meanHR - (meanHR * this->PercentageDecreasedHeartRateAllowed / 100.0);
    if (maxHR <= maxAllowedHR && minHR >= minAllowedHR && meanHR > 1)
    {
      haveConsistentHeartRateMeasurement = 1;
    }
    else
    {
      printf("trial %d is bad, trying again (mean %f, max %f, min %f)\n", numTrials, meanHR, maxHR, minHR);
    }

    numTrials++;
  }

  if (haveConsistentHeartRateMeasurement)
  {
    // set the heart rate parameters
    this->MeanHeartRate = meanHR;
    this->MaxAllowedHeartRate = maxAllowedHR;
    this->MinAllowedHeartRate = minAllowedHR;

    printf("mean heart rate = %f\n", meanHR);
    printf("min heart rate allowed = %f\n", minAllowedHR);
    printf("max heart rate allowed = %f\n", maxAllowedHR);
  }

  return haveConsistentHeartRateMeasurement;
}

//****************************************************************************
// SELECTING ONLY CERTAIN PHASES TO RECONSTRUCT
//****************************************************************************

//----------------------------------------------------------------------------
// Set whether or not to use select phases
void vtkFreehandUltrasound2Dynamic::SetUseSelectPhases(int select)
{
  this->UseSelectPhases = select;

  if (!this->SignalBox || !this->Triggering)
  {
    return;
  }

  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    return;
  }

  // selecting phases from off to on
  if (select)
  {
    int numOutputVolumes = 0;
    for (int phase = 0; phase < this->NumSignalBoxPhases; phase++)
    {
      if (this->MapPhaseToOutputVolume[phase] != -1)
      {
        numOutputVolumes++;
      }
    }
    //increment the number of output volumes and setup
    if (numOutputVolumes != this->NumberOfOutputVolumes)
    {
      this->SetupOutputVolumes(numOutputVolumes);
      this->SetupAccumulationBuffers(numOutputVolumes);
      this->NumberOfOutputVolumes = numOutputVolumes;

      // allocate the output and the accumulation buffer(s)
      this->NeedsClear = 1;
    }
  }
  // selecting phases from on to off
  else
  {
    int numNotReconstructed = 0;
    for (int phase = 0; phase < this->NumSignalBoxPhases; phase++)
    {
      if (this->MapPhaseToOutputVolume[phase] == -1)
      {
        numNotReconstructed++;
      }
    }
    // reset the number of output volumes if something wasn't reconstructed before and now it is
    if (numNotReconstructed != 0)
    {
      this->SetupOutputVolumes(this->NumSignalBoxPhases);
      this->SetupAccumulationBuffers(this->NumSignalBoxPhases);
      this->NumberOfOutputVolumes = this->NumSignalBoxPhases;

      // allocate the output and the accumulation buffer(s)
      this->NeedsClear = 1;
    }
  }

}

//----------------------------------------------------------------------------
// Set whether or not to use a specific phase
void vtkFreehandUltrasound2Dynamic::SetPhaseReconstruction(int phase, int onoff)
{
  // if we don't have the mapping array then we can't do anything - the signal box
  // hasn't been set yet so we don't know how many phases there are
  if (!this->MapPhaseToOutputVolume || !this->SignalBox)
  {
    return;
  }

  // check that the number of phases hasn't changed
  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    return;
  }

  // check to make sure phase is within range
  if (phase < 0 || phase >= this->NumSignalBoxPhases)
  {
    return;
  }

  // toggling from off to on
  if (onoff)
  {
    // we were already on, so do nothing
    if (this->MapPhaseToOutputVolume[phase] != -1)
    {
      return;
    }
    // find what the output volume index corresponding to the phase should be set to
    int volumeIndex = -1;
    int i = 0;
    for (; i < phase; i++)
    {
      if (this->MapPhaseToOutputVolume[i] != -1)
      {
        volumeIndex = this->MapPhaseToOutputVolume[i];
      }
    }
    // change the output volume index at phase
    volumeIndex++;
    this->MapPhaseToOutputVolume[phase] = volumeIndex;
    i = phase + 1;
    // increment the output volume indices for the remaining phases
    for (; i < this->NumSignalBoxPhases; i++)
    {
      if (this->MapPhaseToOutputVolume[i] != -1)
      {
        this->MapPhaseToOutputVolume[i]++;
      }
    }
    if (this->Triggering && this->UseSelectPhases)
    {

      // increment the number of output volumes and setup
      this->SetupOutputVolumes(this->NumberOfOutputVolumes + 1);
      this->SetupAccumulationBuffers(this->NumberOfOutputVolumes + 1);
      this->NumberOfOutputVolumes++;

      // allocate the output and the accumulation buffer(s)
      this->NeedsClear = 1;
    }
  }

  // toggling from on to off
  else
  {
    // if we were already off, then do nothing
    if (this->MapPhaseToOutputVolume[phase] == -1)
    {
      return;
    }
    // change the output volume index at phase
    this->MapPhaseToOutputVolume[phase] = -1;
    // decrement the output volume indices for the remaining phases
    for (int i = phase+1; i < this->NumSignalBoxPhases; i++)
    {
      if (this->MapPhaseToOutputVolume[i] != -1)
      {
        this->MapPhaseToOutputVolume[i]--;
      }
    }

    if (this->Triggering && this->UseSelectPhases)
    {
      // decrement the number of output volumes and setup
      this->SetupOutputVolumes(this->NumberOfOutputVolumes - 1);
      this->SetupAccumulationBuffers(this->NumberOfOutputVolumes - 1);
      this->NumberOfOutputVolumes--;

      // allocate the output and the accumulation buffer(s)
      this->NeedsClear = 1;
    }
  }
}

//----------------------------------------------------------------------------
// Get whether or not a specific phase will be reconstructed
int vtkFreehandUltrasound2Dynamic::GetPhaseReconstruction(int phase)
{

  // if we are not triggering or don't have a signal box, then everything is
  // reconstructed
  if (!this->Triggering || !this->SignalBox)
  {
    return 1;
  }

  // check to make sure that the number of phases hasn't changed in the signal box
  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    return 0;
  }

  // check to make sure phase is within range
  if (phase < 0 || phase >= this->NumSignalBoxPhases)
  {
    return 0;
  }

  // if we are not selecting phases, then we will reconstruct each (valid) phase
  if (!this->UseSelectPhases)
  {
    return 1;
  }

  // look into the array to see if we are reconstructing the specific phase
  if (this->MapPhaseToOutputVolume[phase] == -1)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//----------------------------------------------------------------------------
int vtkFreehandUltrasound2Dynamic::CalculatePortFromPhase(int phase)
{

  // if we are not triggering or don't have a signal box yet, then the output
  // for multiple phases hasn't been allocated
  if (!this->Triggering || !this->SignalBox)
  {
    return 0;
  }

  // if the number of phases of the signal box has changed
  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    return -1;
  }

  // if the phase is invalid...
  if (phase < 0 || phase >= this->NumSignalBoxPhases)
  {
    return -1;
  }

  // if we are not selecting phases, then the index of the output volume is the
  // phase
  if (!this->UseSelectPhases)
  {
    return phase;
  }
  // if we are selecting phases, then determine the index of the output volume
  // from the mapping array, and return if we are not reconstructing this phase
  // will return -1 on error
  else
  {
    return this->MapPhaseToOutputVolume[phase];
  }
}

//----------------------------------------------------------------------------
int vtkFreehandUltrasound2Dynamic::CalculatePhaseFromPort(int port)
{

  // if we are not triggering or don't have a signal box yet, then the output
  // for multiple phases hasn't been allocated
  if (!this->Triggering || !this->SignalBox)
  {
    return 0;
  }

  // if the number of phases of the signal box has changed
  if (this->NumSignalBoxPhases != this->SignalBox->GetNumberOfPhases())
  {
    return -1;
  }

  // if the phase is invalid...
  if (port < 0 || port >= this->NumberOfOutputVolumes)
  {
    return -1;
  }

  // if we are selecting phases, then the phase is the index of the output volume
  if (!this->UseSelectPhases)
  {
    return port;
  }
  // if we are selecting phases, then determine the phase corresponding to a port
  // by looking through the mapping array
  else
  {
    for (int phase = 0; phase < this->NumSignalBoxPhases; phase++)
    {
      if (this->MapPhaseToOutputVolume[phase] == port)
      {
        return phase;
      }
    }
    return -1;
  }

}

//****************************************************************************
// SAVING INSERTED TIMESTAMPS / FRAMES
//****************************************************************************

//----------------------------------------------------------------------------
// Sets the (relative!) directory for saving inserted timestamps and/or slices, and creates
// it if it doesn't exist
void vtkFreehandUltrasound2Dynamic::SetSaveInsertedDirectory(const char* directory)
{
  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  // if we are switching directories
  if (this->SaveInsertedDirectory)
  {
    delete [] this->SaveInsertedDirectory;
  }

  // create the directory for the timestamps and/or slices
  int res;
#ifdef _WIN32
  res = _mkdir(directory);
#else
  int mode = 0777;
  res = mkdir(directory, mode);
#endif

  this->SaveInsertedDirectory = new char[512];
  sprintf(this->SaveInsertedDirectory, "%s", directory);

  // reset this to make the timestamps file for the new directory
  if (this->SaveInsertedTimestamps)
  {
    this->SaveInsertedTimestampsOn();
  }

}


//----------------------------------------------------------------------------
// If turned on, keeps a record of the timestamps used to insert slices
// Save the frame timestamps in the directory previously specified
// frameTimestamps.txt - a file with the frame timestamps inserted
void vtkFreehandUltrasound2Dynamic::SaveInsertedTimestampsOn()
{
  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  /*if (!this->SaveInsertedDirectory) // annoying with AV setup
  {
  LOG_WARNING(<< "Can't set saving inserted timestamps if we don't have a directory yet");
  return;
  }*/

  // in case we're switching directories
  if (this->SaveInsertedTimestampsOutputFile)
  {
    fclose(this->SaveInsertedTimestampsOutputFile);
  }
  if (this->SaveInsertedTimestampsOutputFilename)
  {
    delete [] this->SaveInsertedTimestampsOutputFilename;
  }

  // create the timestamps file

  char path[512];
#ifdef _WIN32
  sprintf(path, "%s\\%s", this->SaveInsertedDirectory, "frameTimestamps.txt");
#else
  sprintf(path, "%s/%s", this->SaveInsertedDirectory, "frameTimestamps.txt");
#endif

  this->SaveInsertedTimestampsOutputFilename = new char[512];
  sprintf(this->SaveInsertedTimestampsOutputFilename, "%s", path);
  this->SaveInsertedTimestampsOutputFile = fopen(path,"a");

  // if it failed to open...
  if (this->SaveInsertedTimestampsOutputFile == NULL)
  {
    this->SaveInsertedTimestamps = 0;
    this->SaveInsertedTimestampsCounter = 0;
    //delete [] this->SaveInsertedDirectory;
    delete [] this->SaveInsertedTimestampsOutputFile;
    delete [] this->SaveInsertedTimestampsOutputFilename;
    //this->SaveInsertedDirectory = NULL;
    this->SaveInsertedTimestampsOutputFilename = NULL;
    return;
  }

  //fprintf(this->SaveInsertedTimestampsOutputFile, "Slice#\tTimestamp\t\t\tPhase\t\tVideoTime\t\t\tECGRate\n");
  this->SaveInsertedTimestamps = 1;
  //this->SaveInsertedTimestampsCounter = 0;
  fclose(this->SaveInsertedTimestampsOutputFile); // will open again in initialize reconstruction

}

//----------------------------------------------------------------------------
// If turned on, saves the inserted 2D slices as .bmps in the directory previously specified.
// Will also automatically turn saving inserted timestamps on
void vtkFreehandUltrasound2Dynamic::SaveInsertedSlicesOn()
{
  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  /*if (!this->SaveInsertedDirectory) // annoying with AV setup
  {
  LOG_WARNING(<< "Can't set saving inserted timestamps if we don't have a directory yet");
  return;
  }*/

  // create the image writer
  this->SaveInsertedSlicesWriter = vtkBMPWriter::New();
  this->SaveInsertedSlicesWriter->SetFileDimensionality(2);

  this->SaveInsertedSlices = 1;

  // set saving timestamps on by default, because otherwise the slices are useless
  // for offline reconstruction
  if (!this->SaveInsertedTimestamps)
  {
    this->SaveInsertedTimestampsOn();
  }
}

//----------------------------------------------------------------------------
void vtkFreehandUltrasound2Dynamic::SaveInsertedTimestampsOff()
{

  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  // need to keep saving inserted timestamps if we are saving inserted slicse
  if (this->SaveInsertedSlices)
  {
    return;
  }

  // sanity check - make sure we are changing settings
  if (!this->SaveInsertedTimestamps)
  {
    return;
  }

  if (this->SaveInsertedTimestampsOutputFile)
  {
    fclose(this->SaveInsertedTimestampsOutputFile);
    this->SaveInsertedTimestampsOutputFile = NULL;
  }

  /*if (this->SaveInsertedDirectory)
  {
  delete [] this->SaveInsertedDirectory;
  this->SaveInsertedDirectory = NULL;
  }*/

  if (this->SaveInsertedTimestampsOutputFilename)
  {
    delete [] this->SaveInsertedTimestampsOutputFilename;
    this->SaveInsertedTimestampsOutputFilename = NULL;
  }

  this->SaveInsertedTimestamps = 0;
  //this->SaveInsertedTimestampsCounter = 0;
}

//----------------------------------------------------------------------------
void vtkFreehandUltrasound2Dynamic::SaveInsertedSlicesOff()
{

  if (this->ReconstructionThreadId != -1)
  {
    return;
  }

  // sanity check - make sure we are changing settings
  if (!this->SaveInsertedSlices)
  {
    return;
  }

  this->SaveInsertedSlices = 0;

  if (this->SaveInsertedSlicesWriter)
  {
    this->SaveInsertedSlicesWriter->Delete();
    this->SaveInsertedSlicesWriter = NULL;
  }

}

//----------------------------------------------------------------------------
// Insert a timestamp into the buffer, if we are saving timestamps (note that
// the timestamp counter is handled whenever a slice is actually inserted, not here)
// Give the phase, the timestamp we are reconstructing, and the timestamp that
// we calculated before we applied temporal calibration
void vtkFreehandUltrasound2Dynamic::SaveSlice(vtkImageData* inData, int phase, double timestamp)
{

  // Needed for valid filenames!
  if (!this->SaveInsertedDirectory)
  {
    return;
  }

  // save timestamps
  if (this->SaveInsertedTimestampsOutputFile)
  {
    fprintf(this->SaveInsertedTimestampsOutputFile, "%d\t\t%f\t\t%f\t\t%d\t\t%f\t\t%f\n", this->SaveInsertedTimestampsCounter, this->DesiredTime, this->VideoTime, phase, timestamp, this->ECGRate);
    //this->SaveInsertedTimestampsCounter++;
  }

  // save .bmp of 2D image
  if (this->SaveInsertedSlices)
  {
    char path[512];
#ifdef _WIN32
    sprintf(path, "%s\\z%d.bmp", this->SaveInsertedDirectory, this->SaveInsertedTimestampsCounter);
#else
    sprintf(path, "%s/z%d.bmp", this->SaveInsertedDirectory, "frameTimestamps.txt");
#endif

    this->SaveInsertedSlicesWriter->SetFileName(path);
    this->SaveInsertedSlicesWriter->SetInput(inData);
    this->SaveInsertedSlicesWriter->Update();
    this->SaveInsertedSlicesWriter->Write();
  }

  this->SaveInsertedTimestampsCounter++;

}

//****************************************************************************
// VTK 5 PIPELINE
//****************************************************************************

//----------------------------------------------------------------------------
// Gets the output ready to receive data, so we need to call it before the
// reconstruction starts.  Updates the information for the output and the
// accumulation buffer
// Whole extent, spacing and origin comes from user specifications for the
// output volume
// Scalar type and number of scalar components come from the input slice
// Also updates NeedsClear if anything has changed
void vtkFreehandUltrasound2Dynamic::InternalExecuteInformation() 
{

  for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
  {
    if (this->Compounding)
    {
      this->InternalExecuteInformationHelper(this->GetOutputFromPort(port), this->GetAccumulationBufferFromPort(port));
    }
    else
    {
      this->InternalExecuteInformationHelper(this->GetOutputFromPort(port), NULL);
    }
  }
}

//----------------------------------------------------------------------------
// Specify what the output data will look like.
// Whole extent, spacing and origin comes from user specifications for the
// output volume
// Scalar type and number of scalar components come from the input slice
// Also updates NeedsClear if anything has changed
int vtkFreehandUltrasound2Dynamic::RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inInfo),
  vtkInformationVector* outInfoVector)
{

  // to avoid conflict between the main application thread and the
  // realtime reconstruction thread
  if (this->ReconstructionThreadId == -1)
  {
    for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
    {
      if (this->Compounding)
      {
        this->RequestInformationHelper(outInfoVector->GetInformationObject(port), this->GetOutputFromPort(port), this->GetAccumulationBufferFromPort(port));
      }
      else
      {
        this->RequestInformationHelper(outInfoVector->GetInformationObject(port), this->GetOutputFromPort(port), NULL);
      }
    }
  }
  return 1;
}

//****************************************************************************
// FILLING HOLES
//****************************************************************************

//----------------------------------------------------------------------------
// Fills holes in the output by using the weighted average of the surrounding
// voxels (see David Gobbi's thesis)
// Basically, just calls MultiThreadFill()
void vtkFreehandUltrasound2Dynamic::FillHolesInOutput()
{
  this->UpdateInformation();
  if (this->NeedsClear)
  {
    this->InternalClearOutput();
  }

  vtkImageData *outData;
  vtkImageData *accData;
  for (int port = 0; port < this->GetNumberOfOutputPorts(); port++)
  {
    outData = this->GetOutputFromPort(port);
    accData = this->GetAccumulationBufferFromPort(port);
    this->MultiThreadFill(outData, accData);
  }

  this->Modified(); 
}

//****************************************************************************
// I/O
//****************************************************************************

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkFreehandUltrasound2Dynamic::MakeXMLElement()
{
  //TODO fix saving according to the unified configuration file
  vtkXMLDataElement* elem;
  elem = this->Superclass::MakeXMLElement();

  // triggering parameters
  vtkXMLDataElement* triggeringParams = vtkXMLDataElement::New();
  triggeringParams->SetName("TriggeringParameters");
  (this->Triggering ? triggeringParams->SetAttribute("Triggering", "On") : triggeringParams->SetAttribute("Triggering", "Off"));
  (this->Retrospective ? triggeringParams->SetAttribute("Gating", "Retrospective") : triggeringParams->SetAttribute("Gating", "Prospective"));
  triggeringParams->SetIntAttribute("NumOutputVolumes", this->NumberOfOutputVolumes);
  elem->AddNestedElement(triggeringParams);

  // signal box parameters
  vtkXMLDataElement *signalBoxParams = vtkXMLDataElement::New();
  signalBoxParams->SetName("SignalBox");
  if (this->SignalBox)
  {
    signalBoxParams->SetIntAttribute("NumPhases", this->SignalBox->GetNumberOfPhases());
  }
  else
  {
    signalBoxParams->SetAttribute("SignalBox", "NULL");
  }
  elem->AddNestedElement(signalBoxParams);

  // check heart rate parameters
  vtkXMLDataElement* checkParams = vtkXMLDataElement::New();
  checkParams->SetName("CheckHeartRateParameters");
  (this->CheckHeartRate ? checkParams->SetAttribute("CheckHeartRate", "On") : checkParams->SetAttribute("CheckHeartRate", "Off"));
  checkParams->SetDoubleAttribute("ECGMonitoringTime", this->ECGMonitoringTime);
  checkParams->SetIntAttribute("NumECGTrials", this->NumECGTrials);
  checkParams->SetDoubleAttribute("PercentIncreaseHRAllowed", this->PercentageIncreasedHeartRateAllowed);
  checkParams->SetDoubleAttribute("PercentDecreasedHRAllowed", this->PercentageDecreasedHeartRateAllowed);
  checkParams->SetDoubleAttribute("MeanHR", this->MeanHeartRate);
  elem->AddNestedElement(checkParams);

  // selecting phases
  vtkXMLDataElement* selectingParams = vtkXMLDataElement::New();
  selectingParams->SetName("UseSelectPhasesParameters");
  (this->UseSelectPhases ? selectingParams->SetAttribute("UseSelectPhases", "On") : selectingParams->SetAttribute("UseSelectPhases", "Off"));
  selectingParams->SetIntAttribute("NumSignalBoxPhases", this->NumSignalBoxPhases);
  selectingParams->SetVectorAttribute("MapPhaseToOutputVolume", this->NumSignalBoxPhases, this->MapPhaseToOutputVolume);
  elem->AddNestedElement(selectingParams);

  // save inserted timestamps
  //vtkXMLDataElement *saveParams = vtkXMLDataElement::New();
  //saveParams->SetName("SaveInserted");
  //(this->SaveInsertedTimestamps ? saveParams->SetAttribute("SaveInsertedTimestamps", "On") : saveParams->SetAttribute("SaveInsertedTimestamps", "Off"));
  //(this->SaveInsertedSlices ? saveParams->SetAttribute("SaveInsertedSlices", "On") : saveParams->SetAttribute("SaveInsertedSlices", "Off"));

  // buffering options
  vtkXMLDataElement *bufferOptions = elem->FindNestedElementWithName("BufferOptions");
  if (bufferOptions && this->SignalBox)
  {
    bufferOptions->SetDoubleAttribute("SignalBoxSleepInterval", this->SignalBox->GetSleepInterval());
  }

  // clean up
  triggeringParams->Delete();
  signalBoxParams->Delete();
  checkParams->Delete();
  selectingParams->Delete();
  bufferOptions->Delete(); 
  //  saveParams->Delete();

  return elem;
}



//----------------------------------------------------------------------------
// Read the freehand parameters from the filename specified in the (relative!)
// directory
// File should have been created using SaveSummaryFile()
PlusStatus vtkFreehandUltrasound2Dynamic::ReadSummary(vtkXMLDataElement* aConfig)
{
	LOG_TRACE("vtkFreehandUltrasound2Dynamic::ReadSummary"); 

  if (this->ReconstructionThreadId != -1)
  {
    LOG_ERROR("Reconstruction is already active");
    return PLUS_FAIL;
  }

	vtkSmartPointer<vtkXMLDataElement> volumeReconstruction = aConfig->FindNestedElementWithName("VolumeReconstruction");
	if (volumeReconstruction == NULL)
  {
		LOG_ERROR("No volume reconstruction is found in the XML tree!");
		return PLUS_FAIL;
	}

  // get the base information
  this->Superclass::ReadSummary(aConfig);

  // triggering options
  vtkXMLDataElement* triggeringParams = volumeReconstruction->FindNestedElementWithName("TriggeringParameters");
  if (triggeringParams)
  {
    if (triggeringParams->GetAttribute("Triggering"))
    {
      ((strcmp(triggeringParams->GetAttribute("Triggering"), "On") == 0) ? this->TriggeringOn() : this->TriggeringOff());
    }
    if (triggeringParams->GetAttribute("Gating"))
    {
      if (strcmp(triggeringParams->GetAttribute("Gating"), "Retrospective") == 0)
      {
        this->RetrospectiveOn();
      }
      else if (strcmp(triggeringParams->GetAttribute("Gating"), "Prospective") == 0)
      {
        this->RetrospectiveOff();
      }
    }
  }

  // signal box options
  vtkXMLDataElement* signalBoxParams = volumeReconstruction->FindNestedElementWithName("SignalBox");
  if (signalBoxParams)
  {
    if (signalBoxParams->GetAttribute("SignalBox"))
    {
      // this is set to NULL, meaning we didn't have a signal box
      // when saving to file, but if we have one here we can just
      // leave it because the triggering flags will let us decide
      // wheteher or not to trigger
    }
    else
    {
      int temp;
      // we should have a signal box, so make one if we don't have one
      if (this->SignalBox == NULL)
      {
        vtkSignalBox* signalBox = vtkSignalBox::New();
        if (signalBoxParams->GetScalarAttribute("NumPhases", temp))
        {
          signalBox->SetNumberOfPhases(temp);
        }
        this->SetSignalBox(signalBox);
      }
      // change the number of phases if we do have a signal box
      else
      {
        if (signalBoxParams->GetScalarAttribute("NumPhases", temp))
        {
          if (temp != this->SignalBox->GetNumberOfPhases())
          {
            this->SignalBox->SetNumberOfPhases(temp);
            this->SetSignalBox(this->SignalBox);
          }
        }
      }
    }
  }

  vtkXMLDataElement* bufferOptions = volumeReconstruction->FindNestedElementWithName("BufferOptions");
  if (bufferOptions)
  {
    if (this->SignalBox)
    {
      double tempd = this->SignalBox->GetSleepInterval();
      bufferOptions->GetScalarAttribute("SignalBoxSleepInterval", tempd);
      this->SignalBox->SetSleepInterval(tempd);
    }
  }

  //check heart rate parameters
  vtkXMLDataElement* checkParams = volumeReconstruction->FindNestedElementWithName("CheckHeartRateParameters");
  if (checkParams)
  {
    if (checkParams->GetAttribute("CheckHeartRate"))
    {
      ((strcmp(checkParams->GetAttribute("CheckHeartRate"), "On") == 0) ? this->CheckHeartRateOn() : this->CheckHeartRateOff());  
    }
    checkParams->GetScalarAttribute("ECGMonitoringTime", this->ECGMonitoringTime);
    checkParams->GetScalarAttribute("NumECGTrials", this->NumECGTrials);
    checkParams->GetScalarAttribute("PercentIncreaseHRAllowed", this->PercentageIncreasedHeartRateAllowed);
    checkParams->GetScalarAttribute("PercentDecreaseHRAllowed", this->PercentageDecreasedHeartRateAllowed);
    checkParams->GetScalarAttribute("MeanHR", this->MeanHeartRate);

    this->MaxAllowedHeartRate = this->MeanHeartRate + (this->MeanHeartRate * this->PercentageIncreasedHeartRateAllowed / 100.0);
    this->MinAllowedHeartRate = this->MeanHeartRate - (this->MeanHeartRate * this->PercentageDecreasedHeartRateAllowed / 100.0);
  }

  // saving inserted timestamps/slices
  //vtkXMLDataElement *saveParams = volumeReconstruction->FindNestedElementWithName("SaveInserted");
  //if (saveParams)
  //  {
  //  if (saveParams->GetAttribute("SaveInsertedTimestamps"))
  //    {
  //    ((strcmp(saveParams->GetAttribute("SaveInsertedTimestamps"), "On") == 0) ? this->SaveInsertedTimestampsOn() : this->SaveInsertedTimestampsOff());
  //    }
  //  if (saveParams->GetAttribute("SaveInsertedSlices"))
  //    {
  //    ((strcmp(saveParams->GetAttribute("SaveInsertedSlices"), "On") == 0) ? this->SaveInsertedSlicesOn() : this->SaveInsertedSlicesOff());
  //    }
  //  }

  // using select phases
  vtkXMLDataElement* selectingParams = volumeReconstruction->FindNestedElementWithName("UseSelectPhasesParameters");
  if (selectingParams)
  {
    selectingParams->GetScalarAttribute("NumSignalBoxPhases", this->NumSignalBoxPhases);
    selectingParams->GetVectorAttribute("MapPhaseToOutputVolume", this->NumSignalBoxPhases, this->MapPhaseToOutputVolume);
    if (selectingParams->GetAttribute("UseSelectPhases"))
    {
      ((strcmp(selectingParams->GetAttribute("UseSelectPhases"), "On") == 0) ? this->UseSelectPhasesOn() : this->UseSelectPhasesOff());
    }
  }

  return PLUS_SUCCESS;

}
