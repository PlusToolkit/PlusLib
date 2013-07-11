/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusReconstructVolumeCommand_h
#define __vtkPlusReconstructVolumeCommand_h

#include "vtkPlusCommand.h"

class vtkVolumeReconstructor;
class vtkTrackedFrameList;
class vtkTransformRepository;

/*!
  \class vtkPlusReconstructVolumeCommand 
  \brief This command reconstructs a volume from an image sequence and saves it to disk or sends it to the client in an IMAGE message. 
  \ingroup PlusLibDataCollection
 */ 
class VTK_EXPORT vtkPlusReconstructVolumeCommand : public vtkPlusCommand
{
public:
  
  static vtkPlusReconstructVolumeCommand *New();
  vtkTypeMacro(vtkPlusReconstructVolumeCommand, vtkPlusCommand);
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  virtual vtkPlusCommand* Clone() { return New(); }

  /*! Executes the command  */
  virtual PlusStatus Execute();

  /*! Read command parameters from XML */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Write command parameters to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

  /*! Get all the command names that this class can execute */
  virtual void GetCommandNames(std::list<std::string> &cmdNames);

  /*! Gets the description for the specified command name. */
  virtual std::string GetDescription(const char* commandName);
  
  /*! File name of the sequence metafile that contains the image frames */
  vtkSetStringMacro(InputSeqFilename);
  vtkGetStringMacro(InputSeqFilename);

  /*! If specified, the reconstructed volume will be saved into this filename */
  vtkSetStringMacro(OutputVolFilename);
  vtkGetStringMacro(OutputVolFilename);

  /*! If specified, the reconstructed volume will sent to the client through OpenIGTLink, using this device name */
  vtkSetStringMacro(OutputVolDeviceName);
  vtkGetStringMacro(OutputVolDeviceName);

  /*! Id of the device that provides the desired channel */
  vtkSetStringMacro(ChannelId);
  vtkGetStringMacro(ChannelId);

  /*! Enables capturing frames. It can be used for pausing the live reconstruction. */
  vtkGetMacro(EnableAddingFrames, bool);
  vtkSetMacro(EnableAddingFrames, bool);

  /*! If this flag is set then the live reconstruction will stop at the next Execute. */
  vtkGetMacro(StopReconstructionRequested, bool);
  vtkSetMacro(StopReconstructionRequested, bool);

  /*! If this flag is set then a snapshot of the live reconstruction will be send/saved at the next Execute */
  vtkGetMacro(ReconstructionSnapshotRequested, bool);
  vtkSetMacro(ReconstructionSnapshotRequested, bool);

  /*! Id of the live reconstruction command to be stopped, suspended, or resumed at the next Execute */
  vtkGetStringMacro(ReferencedCommandId);
  vtkSetStringMacro(ReferencedCommandId);

  void SetNameToReconstruct();
  void SetNameToStart();
  void SetNameToStop();
  void SetNameToSuspend();
  void SetNameToResume();
  void SetNameToGetSnapshot();

protected:

  PlusStatus InitializeReconstruction();
  PlusStatus AddFrames(vtkTrackedFrameList* trackedFrameList);
  PlusStatus SendReconstructionResults();

  vtkPlusReconstructVolumeCommand();
  virtual ~vtkPlusReconstructVolumeCommand();  
  
private:

  char* InputSeqFilename;
  char* OutputVolFilename;
  char* OutputVolDeviceName;
  char* ChannelId;

  vtkSmartPointer<vtkVolumeReconstructor> VolumeReconstructor;
  vtkSmartPointer<vtkTransformRepository> TransformRepository;
  /*! Timestamp of last added frame (the tracked frames acquired since this timestamp will be added to the volume on the next Execute) */
  double LastRecordedFrameTimestamp;

  bool LiveReconstructionInProgress;
  bool EnableAddingFrames;
  bool StopReconstructionRequested;
  bool ReconstructionSnapshotRequested;

  char* ReferencedCommandId;

  vtkPlusReconstructVolumeCommand( const vtkPlusReconstructVolumeCommand& );
  void operator=( const vtkPlusReconstructVolumeCommand& );
  
};

#endif
