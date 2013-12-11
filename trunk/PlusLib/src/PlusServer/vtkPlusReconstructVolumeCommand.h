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
class vtkVirtualVolumeReconstructor;
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

  /*! Id of the live reconstruction command to be stopped, suspended, or resumed at the next Execute */
  vtkGetStringMacro(VolumeReconstructorDeviceId);
  vtkSetStringMacro(VolumeReconstructorDeviceId);

  void SetNameToReconstruct();
  void SetNameToStart();
  void SetNameToStop();
  void SetNameToSuspend();
  void SetNameToResume();
  void SetNameToGetSnapshot();

protected:

  PlusStatus SetCommandCompletedWithImage(vtkImageData* volumeToSend, const std::string &message);

  vtkVirtualVolumeReconstructor* GetVolumeReconstructorDevice();

  vtkPlusReconstructVolumeCommand();
  virtual ~vtkPlusReconstructVolumeCommand();  
  
private:

  char* InputSeqFilename;
  char* OutputVolFilename;
  char* OutputVolDeviceName;
  char* VolumeReconstructorDeviceId;
  
  vtkPlusReconstructVolumeCommand( const vtkPlusReconstructVolumeCommand& );
  void operator=( const vtkPlusReconstructVolumeCommand& );
  
};

#endif
