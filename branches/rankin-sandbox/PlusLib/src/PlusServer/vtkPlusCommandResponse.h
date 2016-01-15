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

  vtkGetMacro(DeviceName,std::string);
  vtkSetMacro(DeviceName,std::string);
  vtkGetMacro(ClientId,unsigned int);
  vtkSetMacro(ClientId,unsigned int);
  vtkGetMacro(Status,PlusStatus);
  vtkSetMacro(Status,PlusStatus);
protected:
  vtkPlusCommandResponse()
  : ClientId(0)
  , Id(0)
  , Status(PLUS_SUCCESS)
  {
  }
  std::string DeviceName;
  unsigned int ClientId;
  uint32_t Id;
  PlusStatus Status; // indicates if the command is succeeded or failed
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

  vtkGetMacro(Message,std::string);
  vtkSetMacro(Message,std::string);
protected:
  vtkPlusCommandStringResponse()
  {
  }
  std::string Message;
private:
  vtkPlusCommandStringResponse( const vtkPlusCommandStringResponse& );
  void operator=( const vtkPlusCommandStringResponse& );
};

//----------------------------------------------------------------------------
class vtkPlusCommandCommandResponse : public vtkPlusCommandResponse
{
public:
  static vtkPlusCommandCommandResponse *New();
  vtkTypeMacro(vtkPlusCommandCommandResponse, vtkPlusCommandResponse);

  vtkGetMacro(Version,uint16_t);
  vtkSetMacro(Version,uint16_t);
  vtkGetMacro(OriginalId,uint32_t);
  vtkSetMacro(OriginalId,uint32_t);
  vtkGetMacro(ErrorString,std::string);
  vtkSetMacro(ErrorString,std::string);

protected:
  vtkPlusCommandCommandResponse()
  {
  }
  uint16_t Version;
  uint32_t OriginalId;
  std::string ErrorString;
private:
  vtkPlusCommandCommandResponse( const vtkPlusCommandCommandResponse& );
  void operator=( const vtkPlusCommandCommandResponse& );
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
