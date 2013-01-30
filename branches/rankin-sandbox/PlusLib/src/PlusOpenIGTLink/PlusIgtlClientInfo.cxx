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
  this->ImageStreams = clientInfo.ImageStreams; 
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

  // Get image streams
  vtkXMLDataElement* imageNames = xmldata->FindNestedElementWithName("ImageNames"); 
  if ( imageNames != NULL )
  {
    for ( int i = 0; i < imageNames->GetNumberOfNestedElements(); ++i )
    {
      const char* image = imageNames->GetNestedElement(i)->GetName(); 
      if ( image == NULL || STRCASECMP( image, "Image") != 0 )
      {
        continue; 
      }
      const char* embeddedTransformToFrame = imageNames->GetNestedElement(i)->GetAttribute("EmbeddedTransformToFrame"); 
      if ( embeddedTransformToFrame == NULL)
      {
        continue; 
      }
      const char* name = imageNames->GetNestedElement(i)->GetAttribute("Name"); 
      if ( name == NULL )
      {
        continue; 
      }

      ImageStream stream; 
      stream.EmbeddedTransformToFrame = embeddedTransformToFrame; 
      stream.Name = name; 
      clientInfo.ImageStreams.push_back(stream); 
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
  for ( unsigned int i = 0; i < IgtlMessageTypes.size(); ++i )
  {
    vtkSmartPointer<vtkXMLDataElement> message = vtkSmartPointer<vtkXMLDataElement>::New(); 
    message->SetName("Message"); 
    message->SetAttribute("Type", IgtlMessageTypes[i].c_str() ); 
    messageTypes->AddNestedElement(message); 
  }
  xmldata->AddNestedElement( messageTypes ); 

  vtkSmartPointer<vtkXMLDataElement> transformNames = vtkSmartPointer<vtkXMLDataElement>::New(); 
  transformNames->SetName("TransformNames"); 
  for ( unsigned int i = 0; i < TransformNames.size(); ++i )
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

  vtkSmartPointer<vtkXMLDataElement> imageNames = vtkSmartPointer<vtkXMLDataElement>::New(); 
  imageNames->SetName("ImageNames"); 
  for ( unsigned int i = 0; i < ImageStreams.size(); ++i )
  {
    vtkSmartPointer<vtkXMLDataElement> image = vtkSmartPointer<vtkXMLDataElement>::New(); 
    image->SetName("Image"); 
    image->SetAttribute("Name", ImageStreams[i].Name.c_str()); 
    image->SetAttribute("EmbeddedTransformToFrame", ImageStreams[i].EmbeddedTransformToFrame.c_str()); 
    imageNames->AddNestedElement(image); 
  }
  xmldata->AddNestedElement( imageNames ); 

  std::ostringstream os; 
  PlusCommon::PrintXML(os, vtkIndent(0), xmldata); 
  strXmlData = os.str(); 
}
