/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSCOMMANDRESPONSE_H
#define __VTKPLUSCOMMANDRESPONSE_H

#include "PlusConfigure.h"

#include "vtkMatrix4x4.h"
#include "vtkPlusDevice.h"

/*!
  \class PlusCommandResponse 
  \brief Structure to store command responses that Plus should send through OpenIGTLink
  \ingroup PlusLibPlusServer
*/
class vtkPlusCommandResponse : public vtkObject
{
public:
  static vtkPlusCommandResponse *New();
  vtkTypeMacro(vtkPlusCommandResponse, vtkObject);

  vtkGetMacro(ClientId,unsigned int);
  vtkSetMacro(ClientId,unsigned int);
protected:
  vtkPlusCommandResponse()
  : ClientId(0)
  {
  }
  unsigned int ClientId;
private:
  vtkPlusCommandResponse( const vtkPlusCommandResponse& );
  void operator=( const vtkPlusCommandResponse& );
};

typedef std::list< vtkSmartPointer<vtkPlusCommandResponse> > PlusCommandResponseList;

//----------------------------------------------------------------------------
class vtkPlusCommandStringResponse : public vtkPlusCommandResponse
{
public:
  static vtkPlusCommandStringResponse *New();
  vtkTypeMacro(vtkPlusCommandStringResponse, vtkPlusCommandResponse);

  vtkGetMacro(DeviceName,std::string);
  vtkSetMacro(DeviceName,std::string);
  vtkGetMacro(Message,std::string);
  vtkSetMacro(Message,std::string);
  vtkGetMacro(Status,PlusStatus);
  vtkSetMacro(Status,PlusStatus);
protected:
  vtkPlusCommandStringResponse()
  : Status(PLUS_SUCCESS)
  {
  }
  std::string DeviceName;
  std::string Message;
  PlusStatus Status; // indicates if the command is succeeded or failed
private:
  vtkPlusCommandStringResponse( const vtkPlusCommandStringResponse& );
  void operator=( const vtkPlusCommandStringResponse& );
};

//----------------------------------------------------------------------------
class vtkPlusCommandImageResponse : public vtkPlusCommandResponse
{
public:
  static vtkPlusCommandImageResponse *New();
  vtkTypeMacro(vtkPlusCommandImageResponse, vtkPlusCommandResponse);

  vtkGetMacro(ImageName,std::string);
  vtkSetMacro(ImageName,std::string);
  vtkSetObjectMacro(ImageData,vtkImageData);
  vtkGetMacro(ImageData,vtkImageData*);
  vtkSetObjectMacro(ImageToReferenceTransform,vtkMatrix4x4);
  vtkGetMacro(ImageToReferenceTransform,vtkMatrix4x4*);
protected:
  vtkPlusCommandImageResponse()
  : ImageData(NULL)
  , ImageToReferenceTransform(NULL)
  {
  }
  virtual ~vtkPlusCommandImageResponse()
  {
    SetImageData(NULL);
    SetImageToReferenceTransform(NULL);
  }
  std::string ImageName;
  vtkImageData* ImageData;
  vtkMatrix4x4* ImageToReferenceTransform;
private:
  // We have pointers in this class, so make sure we don't try to accidentally copy it
  vtkPlusCommandImageResponse( const vtkPlusCommandImageResponse& );
  void operator=( const vtkPlusCommandImageResponse& );
};

//----------------------------------------------------------------------------
class vtkPlusCommandImageMetaDataResponse : public vtkPlusCommandResponse
{
public:
  static vtkPlusCommandImageMetaDataResponse *New();
  vtkTypeMacro(vtkPlusCommandImageMetaDataResponse, vtkPlusCommandResponse);

  void GetImageMetaDataItems(PlusCommon::ImageMetaDataList &list)
  {
    list=this->ImageMetaDataItems;
  }  
  void SetImageMetaDataItems(const PlusCommon::ImageMetaDataList &list)
  {
    this->ImageMetaDataItems=list;
  }
protected:
  vtkPlusCommandImageMetaDataResponse()
  {
  }
  PlusCommon::ImageMetaDataList ImageMetaDataItems;
private:
  vtkPlusCommandImageMetaDataResponse( const vtkPlusCommandImageMetaDataResponse& );
  void operator=( const vtkPlusCommandImageMetaDataResponse& );  
};

#endif
