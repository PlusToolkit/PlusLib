/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusIgtlClientInfo.h"

// IGTL includes
#include <igtl_header.h>

//----------------------------------------------------------------------------
PlusIgtlClientInfo::PlusIgtlClientInfo()
  : ClientHeaderVersion(IGTL_HEADER_VERSION_1)
  , TDATAResolution(0)
  , TDATARequested(false)
  , LastTDATASentTimeStamp(-1)
{

}

//----------------------------------------------------------------------------
PlusStatus PlusIgtlClientInfo::SetClientInfoFromXmlData(const char* strXmlData)
{
  if (strXmlData == NULL)
  {
    LOG_ERROR("Failed to set ClientInfo - input xml data string is invalid");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> xmldata = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromString(strXmlData));
  if (xmldata == NULL)
  {
    LOG_ERROR("Failed to set ClientInfo - invalid xml data string: " << strXmlData);
    return PLUS_FAIL;
  }

  return SetClientInfoFromXmlData(xmldata);
}


//----------------------------------------------------------------------------
PlusStatus PlusIgtlClientInfo::SetClientInfoFromXmlData(vtkXMLDataElement* xmldata)
{
  if (xmldata == NULL)
  {
    LOG_ERROR("Failed to set ClientInfo - invalid xml data string!");
    return PLUS_FAIL;
  }

  PlusIgtlClientInfo clientInfo;

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, ClientHeaderVersion, xmldata);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(TDATARequested, xmldata);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, TDATAResolution, xmldata);
  if (xmldata->GetAttribute("Resolution") != NULL)
  {
    int resolution;
    XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, Resolution, resolution, xmldata);
    clientInfo.SetTDATAResolution(resolution);

    LOG_WARNING("Old parameter name \"Resolution\" detected. Please replace with \"TDATAResolution\".");
    xmldata->RemoveAttribute("Resolution");
    xmldata->SetIntAttribute("TDATAResolution", resolution);
  }

  // Get message types
  vtkXMLDataElement* messageTypes = xmldata->FindNestedElementWithName("MessageTypes");
  if (messageTypes != NULL)
  {
    for (int i = 0; i < messageTypes->GetNumberOfNestedElements(); ++i)
    {
      const char* name = messageTypes->GetNestedElement(i)->GetName();
      if (name == NULL || STRCASECMP(name, "Message") != 0)
      {
        continue;
      }
      vtkXMLDataElement* typeElem = messageTypes->GetNestedElement(i);
      std::string type;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(Type, type, typeElem);
      clientInfo.IgtlMessageTypes.push_back(type);
    }
  }

  // Get transform names
  vtkXMLDataElement* transformNames = xmldata->FindNestedElementWithName("TransformNames");
  if (transformNames != NULL)
  {
    for (int i = 0; i < transformNames->GetNumberOfNestedElements(); ++i)
    {
      const char* transform = transformNames->GetNestedElement(i)->GetName();
      if (transform == NULL || STRCASECMP(transform, "Transform") != 0)
      {
        continue;
      }
      vtkXMLDataElement* transformElem = transformNames->GetNestedElement(i);
      std::string name;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Name, name, transformElem);
      if (name.empty())
      {
        LOG_WARNING("In TransformNames child transform #" << i << " definition is incomplete: required Name attribute is missing.");
        continue;
      }

      igsioTransformName tName;
      if (tName.SetTransformName(name) != PLUS_SUCCESS)
      {
        LOG_WARNING("Invalid transform name: " << name);
        continue;
      }
      clientInfo.TransformNames.push_back(tName);
    }
  }

  // Get image streams
  vtkXMLDataElement* imageNames = xmldata->FindNestedElementWithName("ImageNames");
  if (imageNames != NULL)
  {
    for (int i = 0; i < imageNames->GetNumberOfNestedElements(); ++i)
    {
      const char* image = imageNames->GetNestedElement(i)->GetName();
      if (image == NULL || STRCASECMP(image, "Image") != 0)
      {
        continue;
      }
      vtkXMLDataElement* imageElem = imageNames->GetNestedElement(i);
      std::string embeddedTransformToFrame;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(EmbeddedTransformToFrame, embeddedTransformToFrame, imageElem);
      if (embeddedTransformToFrame.empty())
      {
        LOG_WARNING("EmbeddedTransformToFrame attribute of ImageNames/Image element #" << i << " is missing. This element will be ignored.");
        continue;
      }

      std::string name;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Name, name, imageElem);
      if (name.empty())
      {
        LOG_WARNING("Name attribute of ImageNames/Image element # " << i << " is missing. This element will be ignored.");
        continue;
      }

      ImageStream stream;
      stream.EmbeddedTransformToFrame = embeddedTransformToFrame;
      stream.Name = name;

      XML_FIND_NESTED_ELEMENT_OPTIONAL(encodingElem, imageElem, "Encoding");
      if (encodingElem)
      {
        XML_READ_STRING_ATTRIBUTE_NONMEMBER_REQUIRED(FourCC, stream.EncodeVideoParameters.FourCC, encodingElem);
        XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(RateControl, stream.EncodeVideoParameters.RateControl, encodingElem);
        XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(DeadlineMode, stream.EncodeVideoParameters.DeadlineMode, encodingElem);
        XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(Lossless, stream.EncodeVideoParameters.Lossless, encodingElem);
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, MinKeyframeDistance, stream.EncodeVideoParameters.MinKeyframeDistance, encodingElem);
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, MaxKeyframeDistance, stream.EncodeVideoParameters.MaxKeyframeDistance, encodingElem);
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, Speed, stream.EncodeVideoParameters.Speed, encodingElem);
        XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_OPTIONAL(int, TargetBitrate, stream.EncodeVideoParameters.TargetBitrate, encodingElem);
      }

      clientInfo.ImageStreams.push_back(stream);
    }
  }

  // Get string names
  vtkXMLDataElement* stringNames = xmldata->FindNestedElementWithName("StringNames");
  if (stringNames != NULL)
  {
    for (int i = 0; i < stringNames->GetNumberOfNestedElements(); ++i)
    {
      const char* string = stringNames->GetNestedElement(i)->GetName();
      if (string == NULL || STRCASECMP(string, "String") != 0)
      {
        continue;
      }
      vtkXMLDataElement* stringElem = stringNames->GetNestedElement(i);
      std::string name;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Name, name, stringElem);
      if (name.empty())
      {
        LOG_WARNING("In StringNames child element #" << i << " definition is incomplete: required Name attribute is missing");
        continue;
      }

      clientInfo.StringNames.push_back(name);
    }
  }

  // Copy over the new client info
  (*this) = clientInfo;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::GetClientInfoInXmlData(std::string& strXmlData)
{
  vtkSmartPointer<vtkXMLDataElement> xmldata = vtkSmartPointer<vtkXMLDataElement>::New();
  xmldata->SetName("ClientInfo");
  xmldata->SetAttribute("TDATARequested", (this->GetTDATARequested() ? "TRUE" : "FALSE"));
  xmldata->SetIntAttribute("TDATAResolution", this->GetTDATAResolution());

  vtkSmartPointer<vtkXMLDataElement> messageTypes = vtkSmartPointer<vtkXMLDataElement>::New();
  messageTypes->SetName("MessageTypes");
  for (unsigned int i = 0; i < IgtlMessageTypes.size(); ++i)
  {
    vtkSmartPointer<vtkXMLDataElement> message = vtkSmartPointer<vtkXMLDataElement>::New();
    message->SetName("Message");
    message->SetAttribute("Type", IgtlMessageTypes[i].c_str());
    messageTypes->AddNestedElement(message);
  }
  xmldata->AddNestedElement(messageTypes);

  vtkSmartPointer<vtkXMLDataElement> transformNames = vtkSmartPointer<vtkXMLDataElement>::New();
  transformNames->SetName("TransformNames");
  for (unsigned int i = 0; i < TransformNames.size(); ++i)
  {
    if (!TransformNames[i].IsValid())
    {
      std::string transformName;
      TransformNames[i].GetTransformName(transformName);
      LOG_ERROR("Failed to add transform name to client info - transform name is invalid (" << transformName << ").");
      continue;
    }
    vtkSmartPointer<vtkXMLDataElement> transform = vtkSmartPointer<vtkXMLDataElement>::New();
    transform->SetName("Transform");
    std::string tname;
    TransformNames[i].GetTransformName(tname);
    transform->SetAttribute("Name", tname.c_str());
    transformNames->AddNestedElement(transform);
  }
  xmldata->AddNestedElement(transformNames);

  vtkSmartPointer<vtkXMLDataElement> stringNames = vtkSmartPointer<vtkXMLDataElement>::New();
  stringNames->SetName("StringNames");
  for (unsigned int i = 0; i < StringNames.size(); ++i)
  {
    if (StringNames[i].empty())
    {
      LOG_ERROR("Failed to add string name to client info - string name is empty");
      continue;
    }
    vtkSmartPointer<vtkXMLDataElement> stringElem = vtkSmartPointer<vtkXMLDataElement>::New();
    stringElem->SetName("String");
    stringElem->SetAttribute("Name", StringNames[i].c_str());
    stringNames->AddNestedElement(stringElem);
  }
  xmldata->AddNestedElement(stringNames);

  vtkSmartPointer<vtkXMLDataElement> imageNames = vtkSmartPointer<vtkXMLDataElement>::New();
  imageNames->SetName("ImageNames");
  for (unsigned int i = 0; i < ImageStreams.size(); ++i)
  {
    vtkSmartPointer<vtkXMLDataElement> image = vtkSmartPointer<vtkXMLDataElement>::New();
    image->SetName("Image");
    image->SetAttribute("Name", ImageStreams[i].Name.c_str());
    image->SetAttribute("EmbeddedTransformToFrame", ImageStreams[i].EmbeddedTransformToFrame.c_str());
    imageNames->AddNestedElement(image);
  }
  xmldata->AddNestedElement(imageNames);

  std::ostringstream os;
  igsioCommon::XML::PrintXML(os, vtkIndent(0), xmldata);
  strXmlData = os.str();
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "IGTL version: " << this->ClientHeaderVersion;
  os << indent << "Message types: ";
  if (!this->IgtlMessageTypes.empty())
  {
    for (unsigned int i = 0; i < this->IgtlMessageTypes.size(); ++i)
    {
      if (i > 0)
      {
        os << ", ";
      }
      os << this->IgtlMessageTypes[i];
    }
  }
  else
  {
    os << "(none)";
  }
  os << indent << ". ";

  os << indent << "TDATARequested: " << (this->GetTDATARequested() ? "TRUE" : "FALSE") << ". ";
  os << indent << "LastTDATASentTimeStamp: " << this->GetLastTDATASentTimeStamp() << ". ";
  os << indent << "TDATAResolution: " << this->GetTDATAResolution() << ". ";

  os << ". Transforms: ";
  if (!this->TransformNames.empty())
  {
    for (unsigned int i = 0; i < this->TransformNames.size(); ++i)
    {
      if (i > 0)
      {
        os << ", ";
      }
      std::string tn;
      this->TransformNames[i].GetTransformName(tn);
      os << tn;
    }
  }
  else
  {
    os << "(none)";
  }

  os << ". Strings: ";
  if (!this->StringNames.empty())
  {
    std::ostringstream stringNames;
    for (unsigned int i = 0; i < this->StringNames.size(); ++i)
    {
      if (i > 0)
      {
        os << ", ";
      }
      os << this->StringNames[i];
    }
  }
  else
  {
    os << "(none)";
  }

  os << ". Images: ";
  if (!this->ImageStreams.empty())
  {
    std::ostringstream imageNames;
    for (unsigned int i = 0; i < this->ImageStreams.size(); ++i)
    {
      if (i > 0)
      {
        os << ", ";
      }
      os << this->ImageStreams[i].Name << " (EmbeddedTransformToFrame: " << this->ImageStreams[i].EmbeddedTransformToFrame << ")";
    }
  }
  else
  {
    os << "(none)";
  }
}

//----------------------------------------------------------------------------
int PlusIgtlClientInfo::GetClientHeaderVersion() const
{
  return this->ClientHeaderVersion;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::SetClientHeaderVersion(int version)
{
  this->ClientHeaderVersion = version;
}

//----------------------------------------------------------------------------
int PlusIgtlClientInfo::GetTDATAResolution() const
{
  return this->TDATAResolution;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::SetTDATAResolution(int val)
{
  this->TDATAResolution = val;
}

//----------------------------------------------------------------------------
bool PlusIgtlClientInfo::GetTDATARequested() const
{
  return this->TDATARequested;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::SetTDATARequested(bool val)
{
  this->TDATARequested = val;
}

//----------------------------------------------------------------------------
double PlusIgtlClientInfo::GetLastTDATASentTimeStamp() const
{
  return this->LastTDATASentTimeStamp;
}

//----------------------------------------------------------------------------
void PlusIgtlClientInfo::SetLastTDATASentTimeStamp(double val)
{
  this->LastTDATASentTimeStamp = val;
}
