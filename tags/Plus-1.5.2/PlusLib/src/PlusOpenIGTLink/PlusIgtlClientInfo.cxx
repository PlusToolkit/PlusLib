/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusIgtlClientInfo.h"

//----------------------------------------------------------------------------
PlusIgtlClientInfo::PlusIgtlClientInfo()
{
  this->ClientSocket = NULL; 
}

//----------------------------------------------------------------------------
PlusIgtlClientInfo::~PlusIgtlClientInfo()
{

}

//----------------------------------------------------------------------------
PlusIgtlClientInfo::PlusIgtlClientInfo(const PlusIgtlClientInfo& clientInfo)
{
  this->ClientSocket = NULL; 
  *this = clientInfo; 
}

//----------------------------------------------------------------------------
PlusIgtlClientInfo& PlusIgtlClientInfo::operator=(PlusIgtlClientInfo const& clientInfo)
{
  // Handle self-assignment
  if (this == &clientInfo)
  {
    return *this;
  }
  
  if ( this->ClientSocket.IsNull() )
  {
    this->ClientSocket = igtl::ClientSocket::New(); 
  }

  this->ClientSocket = clientInfo.ClientSocket; 
  this->ShallowCopy(clientInfo); 
  return *this;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::ShallowCopy(const PlusIgtlClientInfo& clientInfo)
{
  this->IgtlMessageTypes = clientInfo.IgtlMessageTypes; 
  this->ImageTransformName = clientInfo.ImageTransformName; 
  this->TransformNames = clientInfo.TransformNames; 
}

//----------------------------------------------------------------------------
PlusStatus PlusIgtlClientInfo::SetClientInfoFromXmlData( const char* strXmlData )
{
  if ( strXmlData == NULL )
  {
    LOG_ERROR("Failed to set ClientInfo - input xml data string is NULL!" ); 
    return PLUS_FAIL; 
  }

  PlusIgtlClientInfo clientInfo; 

  vtkSmartPointer<vtkXMLDataElement> xmldata = vtkSmartPointer<vtkXMLDataElement>::Take( vtkXMLUtilities::ReadElementFromString(strXmlData) );

  if ( xmldata == NULL )
  {
    LOG_ERROR("Failed to set ClientInfo - invalid xml data string!"); 
    return PLUS_FAIL; 
  }

  // Get message types
  vtkXMLDataElement* messageTypes = xmldata->FindNestedElementWithName("MessageTypes"); 
  if ( messageTypes != NULL )
  {
    for ( int i = 0; i < messageTypes->GetNumberOfNestedElements(); ++i )
    {
      const char* name = messageTypes->GetNestedElement(i)->GetName(); 
      if ( name == NULL || STRCASECMP( name, "Message") != 0 )
      {
        continue; 
      }
      const char* type = messageTypes->GetNestedElement(i)->GetAttribute("Type"); 
      if ( type != NULL )
      {
        clientInfo.IgtlMessageTypes.push_back(type); 
      }
    } 
  }

  // Get transform names
  vtkXMLDataElement* transformNames = xmldata->FindNestedElementWithName("TransformNames"); 
  if ( transformNames != NULL )
  {
    for ( int i = 0; i < transformNames->GetNumberOfNestedElements(); ++i )
    {
      const char* transform = transformNames->GetNestedElement(i)->GetName(); 
      if ( transform == NULL || STRCASECMP( transform, "Transform") != 0 )
      {
        continue; 
      }
      const char* name = transformNames->GetNestedElement(i)->GetAttribute("Name"); 
      if ( name != NULL )
      {
        PlusTransformName tName; 
        if ( tName.SetTransformName(name) != PLUS_SUCCESS )
        {
          LOG_WARNING( "Invalid transform name: " << name ); 
          continue; 
        }
        clientInfo.TransformNames.push_back(tName); 
      }
    } // transformNames
  }

  // Get image transform name
  vtkXMLDataElement* imageTransform = xmldata->FindNestedElementWithName("ImageTransform"); 
  if ( imageTransform != NULL )
  {
    const char* name = imageTransform->GetAttribute("Name"); 
    if ( name != NULL )
    {
      PlusTransformName tName; 
      if ( tName.SetTransformName(name) != PLUS_SUCCESS )
      {
        LOG_WARNING( "Invalid transform name: " << name ); 
      }
      else
      {
        clientInfo.ImageTransformName = tName; 
      }
    }
  }

  // Copy over the new client info 
  (*this) = clientInfo; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::GetClientInfoInXmlData( std::string& strXmlData )
{
  vtkSmartPointer<vtkXMLDataElement> xmldata = vtkSmartPointer<vtkXMLDataElement>::New(); 
  xmldata->SetName("ClientInfo"); 

  vtkSmartPointer<vtkXMLDataElement> messageTypes = vtkSmartPointer<vtkXMLDataElement>::New(); 
  messageTypes->SetName("MessageTypes"); 
  for ( int i = 0; i < IgtlMessageTypes.size(); ++i )
  {
    vtkSmartPointer<vtkXMLDataElement> message = vtkSmartPointer<vtkXMLDataElement>::New(); 
    message->SetName("Message"); 
    message->SetAttribute("Type", IgtlMessageTypes[i].c_str() ); 
    messageTypes->AddNestedElement(message); 
  }
  xmldata->AddNestedElement( messageTypes ); 

  vtkSmartPointer<vtkXMLDataElement> transformNames = vtkSmartPointer<vtkXMLDataElement>::New(); 
  transformNames->SetName("TransformNames"); 
  for ( int i = 0; i < TransformNames.size(); ++i )
  {
    if ( ! TransformNames[i].IsValid() )
    {
      std::string transformName; 
      TransformNames[i].GetTransformName(transformName); 
      LOG_ERROR("Failed to add transform name to client info - transform name is invalid (" << transformName << ")." ); 
      continue; 
    }
    vtkSmartPointer<vtkXMLDataElement> transform = vtkSmartPointer<vtkXMLDataElement>::New(); 
    transform->SetName("Transform"); 
    std::string tname; 
    TransformNames[i].GetTransformName(tname); 
    transform->SetAttribute("Name", tname.c_str() ); 
    transformNames->AddNestedElement(transform);
  }
  xmldata->AddNestedElement( transformNames ); 

  vtkSmartPointer<vtkXMLDataElement> imageTransformName = vtkSmartPointer<vtkXMLDataElement>::New(); 
  imageTransformName->SetName("ImageTransform"); 
  if ( ImageTransformName.IsValid() )
  {
    std::string tname; 
    ImageTransformName.GetTransformName(tname); 
    imageTransformName->SetAttribute("Name", tname.c_str() ); 
  }
  // Report error if the transform name is not empty (empty image transform name means no image transform needed)
  else if ( !ImageTransformName.From().empty() && !ImageTransformName.To().empty() )
  {
    std::string transformName; 
    ImageTransformName.GetTransformName(transformName); 
    LOG_ERROR("Failed to add image transform name to client info - transform name is invalid (" << transformName << ")." ); 
  }
  xmldata->AddNestedElement( imageTransformName ); 

  std::ostringstream os; 
  xmldata->PrintXML( os, vtkIndent(0) ); 
  strXmlData = os.str(); 
}
