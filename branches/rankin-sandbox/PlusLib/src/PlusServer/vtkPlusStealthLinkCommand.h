/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPlusStealthLinkCommand_h
#define __vtkPlusStealthLinkCommand_h

#include "vtkPlusCommand.h"

class vtkStealthLinkTracker;

//class vtkStealthLinkTracker;
/*!
  \class vtkPlusStealthLinkCommand 
  \brief This command reconstructs a volume from an image sequence and saves it to disk or sends it to the client in an IMAGE message. 
  \ingroup PlusLibPlusServer
 */ 
class VTK_EXPORT vtkPlusStealthLinkCommand : public vtkPlusCommand
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
  
  /*! Gets the transformation matrix f.... TODO !*/
   vtkSmartPointer<vtkMatrix4x4> GetFrameToExamTransform();
  
  /*! If specified, the reconstructed volume will sent to the client through OpenIGTLink, using this device name */
  vtkSetStringMacro(PatientName);
  vtkGetStringMacro(PatientName);

  /*! Id of the live reconstruction command to be stopped, suspended, or resumed at the next Execute */
  vtkGetStringMacro(StealthLinkDeviceId);
  vtkSetStringMacro(StealthLinkDeviceId);
  
  /*! Id of the live reconstruction command to be stopped, suspended, or resumed at the next Execute */
  vtkGetStringMacro(PatientId);
  vtkSetStringMacro(PatientId);
  
  /*! If specified, the reconstructed volume will sent to the client through OpenIGTLink, using this device name */
  vtkSetStringMacro(DicomImagesOutputDirectory);
  vtkGetStringMacro(DicomImagesOutputDirectory);

  void SetNameToGetExam(); // TODO patientName, patientID , description do similiar for patientId and description
  void SetNameToGetRegistration();

  bool IsMatrixIdentityMatrix(vtkMatrix4x4*);

protected:

  /*! Saves image to disk (if requested) and prepare sending image as a response (if requested) */
  PlusStatus ProcessImageReply(vtkImageData* volumeToSend, vtkMatrix4x4* imageToReferenceOrientationMatrixWithSpacing);

  vtkStealthLinkTracker* GetStealthLinkDevice(); //TODO replace the name with vtkStealthLink

  vtkPlusStealthLinkCommand();
  virtual ~vtkPlusStealthLinkCommand();  
  
private:

  vtkSmartPointer<vtkMatrix4x4> FrameToExamTransform;
  char* PatientName;
  char* PatientId;
  char* StealthLinkDeviceId;
  char* DicomImagesOutputDirectory;

  vtkPlusStealthLinkCommand( const vtkPlusStealthLinkCommand& );
  void operator=( const vtkPlusStealthLinkCommand& );
  
};

#endif
