/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStealthLinkCommand_h
#define __vtkPlusStealthLinkCommand_h

#include "vtkPlusServerExport.h"

#include "vtkPlusCommand.h"
#include "vtkTransformRepository.h"

class vtkStealthLinkTracker;

//class vtkStealthLinkTracker;
/*!
  \class vtkPlusStealthLinkCommand 
  \brief This command reconstructs a volume from an image sequence and saves it to disk or sends it to the client in an IMAGE message. 
  \ingroup PlusLibPlusServer
 */ 
class vtkPlusServerExport vtkPlusStealthLinkCommand : public vtkPlusCommand
{
public:
  
  static vtkPlusStealthLinkCommand *New();
  vtkTypeMacro(vtkPlusStealthLinkCommand, vtkPlusCommand);
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

  /*! Id of the stealthlink device */
  vtkGetStringMacro(StealthLinkDeviceId);
  vtkSetStringMacro(StealthLinkDeviceId);
  
  /*! The folder in which the dicom images will be stored. The default value is: PlusOutputDirectory\StealthLinkDicomOutput  */
  vtkSetStringMacro(DicomImagesOutputDirectory);
  vtkGetStringMacro(DicomImagesOutputDirectory);

  /*!
    The frame reference in  which the image will be represented. Example if this is RAS then image will be defined
    in RAS coordinate system, if Reference, the image will be in reference coordinate system
  */
  vtkSetStringMacro(VolumeEmbeddedTransformToFrame);
  vtkGetStringMacro(VolumeEmbeddedTransformToFrame);

  /*!
    If enabled then the DICOM files received through StealthLink will be preserved in the DicomImagesOutputDirectory.
    If disabled then the DICOM files are deleted after the volume is sent through OpenIGTLink.
  */
  void SetKeepReceivedDicomFiles(bool keepReceivedDicomFiles);
  bool GetKeepReceivedDicomFiles();

  /*!
    Set the command to get the exam data (image and Ras to VolumeEmbeddedTransformToFrame coordinate system transform).
    In the future there may be other command names.
  */
  void SetNameToGetExam(); 

protected:

  /*! Saves image to disk (if requested) and prepare sending image as a response (if requested) */
  PlusStatus ProcessImageReply(const std::string& imageId,vtkImageData* volumeToSend, vtkMatrix4x4* imageToReferenceOrientationMatrixWithSpacing,std::string& resultMessage);

  vtkStealthLinkTracker* GetStealthLinkDevice();

  vtkPlusStealthLinkCommand();
  virtual ~vtkPlusStealthLinkCommand();  

private:

  char* StealthLinkDeviceId;
  char* DicomImagesOutputDirectory;
  char* VolumeEmbeddedTransformToFrame;
  bool KeepReceivedDicomFiles;

  vtkPlusStealthLinkCommand( const vtkPlusStealthLinkCommand& );
  void operator=( const vtkPlusStealthLinkCommand& );
  
};

#endif
