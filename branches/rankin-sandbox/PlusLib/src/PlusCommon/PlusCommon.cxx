#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLDataElement.h"
#include "PlusRevision.h"

//-------------------------------------------------------
PlusTransformName::PlusTransformName()
{
}

//-------------------------------------------------------
PlusTransformName::~PlusTransformName()
{
}

//-------------------------------------------------------
PlusTransformName::PlusTransformName(std::string aFrom, std::string aTo )
{
  this->Capitalize(aFrom); 
  this->m_From = aFrom; 

  this->Capitalize(aTo); 
  this->m_To = aTo; 
}

//-------------------------------------------------------
PlusTransformName::PlusTransformName(const std::string& transformName )
{
  this->SetTransformName(transformName.c_str());
}

//-------------------------------------------------------
bool PlusTransformName::IsValid() const
{
  if ( this->m_From.empty() )
  {
    return false;
  }

  if ( this->m_To.empty() )
  {
    return false;
  }

  return true; 
}

//-------------------------------------------------------
PlusStatus PlusTransformName::SetTransformName(const char* aTransformName)
{
  this->m_From.clear(); 
  this->m_To.clear();

  if ( aTransformName == NULL )
  {
    LOG_ERROR("Failed to set transform name if it's NULL");
    return PLUS_FAIL;
  }

  std::string transformNameStr(aTransformName);
  size_t posTo=std::string::npos;

  // Check if the string has only one valid 'To' phrase 
  int numOfMatch=0;
  std::string subString = transformNameStr;
  size_t posToTested=std::string::npos;
  size_t numberOfRemovedChars=0;
  while ( ((posToTested = subString.find("To")) != std::string::npos) && (subString.length() > posToTested+2) )
  {
    if (toupper(subString[posToTested+2]) == subString[posToTested+2])
    {
      // there is a "To", and after that the next letter is uppercase, so it's really a match (e.g., the first To in TestToolToTracker would not be a real match)
      numOfMatch++;
      posTo=numberOfRemovedChars+posToTested;
    }
    // search in the rest of the string
    subString = subString.substr(posToTested+2);
    numberOfRemovedChars+=posToTested+2;
  }

  if ( numOfMatch != 1 )
  {
    LOG_ERROR("Unable to parse transform name, there are " << numOfMatch
              << " matching 'To' phrases in the transform name '" << aTransformName << "', while exactly one allowed.");
    return PLUS_FAIL;
  }

  // Find <FrameFrom>To<FrameTo> matches 
  if ( posTo == std::string::npos )
  {
    LOG_ERROR("Failed to set transform name - unable to find 'To' in '" << aTransformName << "'!");
    return PLUS_FAIL;
  }
  else if ( posTo == 0 )
  {
    LOG_ERROR("Failed to set transform name - no coordinate frame name before 'To' in '" << aTransformName << "'!");
    return PLUS_FAIL;
  }
  else if ( posTo == transformNameStr.length()-2 )
  {
    LOG_ERROR("Failed to set transform name - no coordinate frame name after 'To' in '" << aTransformName << "'!");
    return PLUS_FAIL;
  }

  // Set From coordinate frame name
  this->m_From = transformNameStr.substr(0, posTo);

  // Allow handling of To coordinate frame containing "Transform"
  std::string postFrom(transformNameStr.substr(posTo+2));
  if( postFrom.find("Transform") != std::string::npos )
  {
    postFrom = postFrom.substr(0, postFrom.find("Transform"));
  }

  this->m_To = postFrom;
  this->Capitalize(this->m_From); 
  this->Capitalize(this->m_To); 

  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus PlusTransformName::GetTransformName(std::string& aTransformName) const
{
  if ( this->m_From.empty() )
  {
    LOG_ERROR("Failed to get transform name - 'From' transform name is empty");
    return PLUS_FAIL;
  }

  if ( this->m_To.empty() )
  {
    LOG_ERROR("Failed to get transform name - 'To' transform name is empty");
    return PLUS_FAIL;
  }

  aTransformName =( this->m_From + std::string("To") + this->m_To); 
  return PLUS_SUCCESS; 
}

//-------------------------------------------------------
std::string PlusTransformName::GetTransformName() const
{
  return ( this->m_From + std::string("To") + this->m_To); 
}

//-------------------------------------------------------
std::string PlusTransformName::From() const
{
  return this->m_From; 
}

//-------------------------------------------------------
std::string PlusTransformName::To() const
{
  return this->m_To; 
}

//-------------------------------------------------------
void PlusTransformName::Capitalize(std::string& aString )
{
  // Change first character to uppercase
  if (aString.length()<1)
  {
    return;
  }
  aString[0] = toupper( aString[0] );
}

//-------------------------------------------------------
void PlusTransformName::Clear()
{
  this->m_From = "";
  this->m_To = "";
}

//----------------------------------------------------------------------------
PlusStatus PlusCommon::CreateTemporaryFilename( std::string& aString, const std::string& anOutputDirectory )
{
  aString = "";
  int maxRetryCount = 50;
  int tryCount = 0;
  while( tryCount < maxRetryCount)
  {
    tryCount++;

#ifdef _WIN32
    // Get output directory
    char candidateFilename[MAX_PATH]="";
    std::string path;
    if( !anOutputDirectory.empty() )
    {
      path = vtksys::SystemTools::GetRealPath(anOutputDirectory.c_str());
    }
    else
    {
      char tempPath[MAX_PATH]="";
      if( GetTempPath(MAX_PATH, tempPath) == 0 )
      {
        LOG_ERROR("Unable to retrieve temp path: " << GetLastError() );
        return PLUS_FAIL;
      }
      path = tempPath;
    }

    // Get full output file path
    UINT uRetVal = GetTempFileName(path.c_str(), "tmp", 0, candidateFilename);  // buffer for name
    if( uRetVal == ERROR_BUFFER_OVERFLOW )
    {
      if( vtksys::SystemTools::FileExists(candidateFilename) )
      {
        vtksys::SystemTools::RemoveFile(candidateFilename);
      }
      LOG_ERROR("Path too long to generate temporary filename ("<<path<<"). Consider moving output directory to shorter path.");
      continue;
    }
    else if (uRetVal==0)
    {
      LOG_ERROR("Failed to generate temporary filename. Error code:" << GetLastError());
      continue;
    }

    aString = candidateFilename;
    return PLUS_SUCCESS;
#else
    std::string path;
    if( !anOutputDirectory.empty() )
    {
      path = vtksys::SystemTools::GetRealPath(anOutputDirectory.c_str());
    }
    else
    {
      path = std::string(P_tmpdir);
    }

    char candidateFilenameBuffer[PATH_MAX];
    memset(candidateFilenameBuffer, 0, PATH_MAX);
    strncpy(candidateFilenameBuffer, path.c_str(), path.length());
    strcat(candidateFilenameBuffer, "/plusTmpFile-XXXXXX");
    char* candidateFilename = mktemp(candidateFilenameBuffer);

    if( vtksys::SystemTools::FileExists(candidateFilename) )
    {
      continue;
    }
    ofstream aFile(candidateFilename);
    if( !aFile.is_open() )
    {
      LOG_WARNING("Cannot write to temp file " << candidateFilename << " check write permissions of output directory.");
      continue;
    }

    aFile.close();
    vtksys::SystemTools::RemoveFile(candidateFilename);
    aString = candidateFilename;
    return PLUS_SUCCESS;
#endif
  }

  LOG_ERROR("PlusCommon::CreateTemporaryFilename failed to generate a temporary file name");
  return PLUS_FAIL;
}

//-------------------------------------------------------
std::string PlusCommon::Trim(std::string &str)
{
  str.erase(str.find_last_not_of(" \t\r\n")+1);
  str.erase(0,str.find_first_not_of(" \t\r\n"));

  return str;
}

//-------------------------------------------------------
std::string PlusCommon::Trim(const char* c)
{
  std::string str(c);
  return PlusCommon::Trim(str);
}

//----------------------------------------------------------------------------
// print out data while replacing XML special characters <, >, &, ", ' with
// &lt;, &gt;, &amp;, &quot;, &apos;, respectively.
void PrintWithEscapedData(ostream& os, const char* data)
{
  for(size_t i=0;i<strlen(data);i++)
  {
    switch(data[i])
    {
      case '&': os << "&amp;"; break;
      case '<': os << "&lt;"; break;
      case '>': os << "&gt;"; break;
      case '"': os << "&quot;"; break;
      case '\'': os << "&apos;"; break;
      default: os << data[i];
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus PlusCommon::PrintXML(ostream& os, vtkIndent indent, vtkXMLDataElement* elem)
{
  if (elem==NULL)
  {
    LOG_ERROR("PlusCommon::PrintXML failed, input element is invalid");
    return PLUS_FAIL;
  }
  vtkIndent nextIndent=indent.GetNextIndent();

  os << indent << "<" << elem->GetName();

  // If there are many attributes then print each of them in separate lines to improve readability
  bool printEachAttributeInNewLine=elem->GetNumberOfAttributes()>5;

  for(int i=0;i < elem->GetNumberOfAttributes();++i)
  {
    std::string attName=elem->GetAttributeName(i);

    // Find out if it's a matrix element, because we format them somewhat differently
    bool matrixElement=false;
    const int MATRIX_ELEM_COUNT=16;
    double matrixValues[MATRIX_ELEM_COUNT]={0};
    if (attName.find("Matrix")!=std::string::npos || attName.find("Transform")!=std::string::npos)
    {
      if (elem->GetVectorAttribute(attName.c_str(),MATRIX_ELEM_COUNT, matrixValues) == MATRIX_ELEM_COUNT)
      {
        // This seems to be a vector
        matrixElement=true;
      }
    }

    if (matrixElement)
    {
      os << std::endl;
      os << nextIndent << attName << "=\"" << std::endl;
      vtkIndent matrixIndent=nextIndent.GetNextIndent();
      os << matrixIndent << matrixValues[0] << "\t" << matrixValues[1] << "\t" << matrixValues[2] << "\t" << matrixValues[3] << std::endl;
      os << matrixIndent << matrixValues[4] << "\t" << matrixValues[5] << "\t" << matrixValues[6] << "\t" << matrixValues[7] << std::endl;
      os << matrixIndent << matrixValues[8] << "\t" << matrixValues[9] << "\t" << matrixValues[10] << "\t" << matrixValues[11] << std::endl;
      os << matrixIndent << matrixValues[12] << "\t" << matrixValues[13] << "\t" << matrixValues[14] << "\t" << matrixValues[15] << "\"";
      // Prevent writing additional attributes right after the last line of the matrix
      // If this is the last attribute then we don't have to start a new line, just append the closing to the matrix line.
      // If each attribute is written to a separate line anyway then we don't have to start a new line, it'll be added before adding the next element.
      bool isLastAttribute = (i+1 == elem->GetNumberOfAttributes());
      if (!isLastAttribute && !printEachAttributeInNewLine)
      {
        os << std::endl << nextIndent;
      }
    }
    else
    {
      if (printEachAttributeInNewLine)
      {
        os << std::endl << nextIndent;
      }
      else
      {
        os << " ";
      }
      os << attName << "=\"";
      PrintWithEscapedData(os, elem->GetAttributeValue(i));
      os << "\"";
    }
  }
  // Long format tag is needed if either or both 
  // nested elements or inline data are present.  
  std::string charData;
  char *charDataPtr=elem->GetCharacterData();
  if (charDataPtr!=NULL)
  {
    charData=charDataPtr;
    Trim(charData);
  }

  if (elem->GetNumberOfNestedElements()>0 || !charData.empty())
  {
    os << ">\n";
    // nested elements
    for(int i=0;i < elem->GetNumberOfNestedElements();++i)
    {
      PrintXML(os,nextIndent,elem->GetNestedElement(i));
    }
    // inline data
    if (!charData.empty())
    {
      os << nextIndent;
      PrintWithEscapedData(os, charData.c_str());
      os << endl;
    }
    // close tag
    os << indent << "</" << elem->GetName() << ">\n";
  }
  // We can get away with short format tag.
  else
  {
    os << " />\n";
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus PlusCommon::PrintXML(const char* fname, vtkXMLDataElement* elem)
{
  ofstream of(fname);
  if (!of.is_open())
  {
    LOG_ERROR("Failed to open "<<fname<<" for writing");
    return PLUS_FAIL;
  }
  of.imbue(std::locale::classic());
  return PlusCommon::PrintXML(of, vtkIndent(), elem);
}

#if (VTK_MAJOR_VERSION < 6)
//----------------------------------------------------------------------------
// A reimplementation of the vtkXMLDataElement::RemoveAttribute method
// using public APIs. This method works correctly, while the VTK 5.x
// version causes memory corruption.
// TODO: remove this method when Plus uses VTK 6.x, see more detail at
// https://www.assembla.com/spaces/plus/tickets/859
void PlusCommon::RemoveAttribute(vtkXMLDataElement* elem, const char *name)
{
  if (elem==NULL)
  {
    LOG_ERROR("PlusCommon::RemoveAttribute elem is invalid");
    return;
  }
  if (name==NULL)
  {
    LOG_WARNING("PlusCommon::RemoveAttribute name is invalid");
    return;
  }
  std::vector< std::string > attNames;
  std::vector< std::string > attValues;
  int numberOfAttributes=elem->GetNumberOfAttributes();
  for(int i=0; i<numberOfAttributes; ++i)
  {
    attNames.push_back(elem->GetAttributeName(i));
    attValues.push_back(elem->GetAttributeValue(i));
  }
  elem->RemoveAllAttributes();
  for (int i=0; i<numberOfAttributes; ++i)
  {
    if (attNames[i].compare(name)==0)
    {
      continue;
    }
    elem->SetAttribute(attNames[i].c_str(),attValues[i].c_str());
  }
}
#endif

//----------------------------------------------------------------------------
std::string PlusCommon::GetPlusLibVersionString()
{
  std::string plusLibVersion = std::string("Plus-") + std::string(PLUSLIB_VERSION) + "." + std::string(PLUSLIB_REVISION); 
#ifdef _DEBUG
  plusLibVersion += " (debug build)";
#endif
#if defined _WIN64
  plusLibVersion += " - Win64";
#elif defined _WIN32
  plusLibVersion += " - Win32";
#elif defined __APPLE__
  plusLibVersion += " - Mac";
#else
  plusLibVersion += " - Linux";
#endif 
  return plusLibVersion;
}

//-------------------------------------------------------
void PlusCommon::SplitStringIntoTokens(const std::string &s, char delim, std::vector<std::string> &elems, bool keepEmptyParts)
{
  std::istringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim))
  {
    if( keepEmptyParts || !item.empty() )
    {
      elems.push_back(item);
    }
  }
}

//----------------------------------------------------------------------------
vtkPlusCommonExport void PlusCommon::JoinTokensIntoString(const std::vector<std::string>& elems, std::string& output)
{
  typedef std::vector<std::string> StringList;

  for( StringList::const_iterator it = elems.begin(); it != elems.end(); ++it )
  {
    output += *it;
  }
}

//----------------------------------------------------------------------------
vtkPlusCommonExport void PlusCommon::JoinTokensIntoString(const std::vector<std::string>& elems, std::string& output, char separator)
{
  typedef std::vector<std::string> StringList;

  for( StringList::const_iterator it = elems.begin(); it != elems.end(); ++it )
  {
    output += *it;
    if( it != elems.end()-1 )
    {
      output += separator;
    }
  }
}

//-------------------------------------------------------
bool PlusCommon::IsClippingRequested(const int clipOrigin[3], const int clipSize[3])
{
  return ( 
      clipOrigin[0] != PlusCommon::NO_CLIP &&
      clipOrigin[1] != PlusCommon::NO_CLIP &&
      clipOrigin[2] != PlusCommon::NO_CLIP &&
      clipSize[0] != PlusCommon::NO_CLIP &&
      clipSize[1] != PlusCommon::NO_CLIP &&
      clipSize[2] != PlusCommon::NO_CLIP
  );
}

//-------------------------------------------------------
bool PlusCommon::IsClippingWithinExtents(const int clipOrigin[3], const int clipSize[3], const int extents[6])
{
  return (clipOrigin[0] >= extents[0] && clipOrigin[0] <= extents[1]) &&
      (clipOrigin[1] >= extents[2] && clipOrigin[1] <= extents[3]) &&  // Verify that the origin is within the image
      (clipOrigin[2] >= extents[4] && clipOrigin[2] <= extents[5]) &&

      (clipOrigin[0]+clipSize[0]-1 <= extents[1]) &&
      (clipOrigin[1]+clipSize[1]-1 <= extents[3]) && // Verify that the extent of the clipping falls within the image
      (clipOrigin[2]+clipSize[2]-1 <= extents[5]);
}

//-------------------------------------------------------
PlusStatus PlusCommon::RobustFwrite(FILE* fileHandle, void* data, size_t dataSize, size_t &writtenSize)
{
  // on some systems fwrite cannot write all data in one chunk, therefore we have to write
  // chunks until all bytes are written or failed to write any bytes
  unsigned char* writePointer = static_cast<unsigned char*>(data);
  size_t remainingBytes = dataSize;
  size_t writtenBytesForCurrentBlock = 0;
  while( (writtenBytesForCurrentBlock = fwrite(writePointer, 1, remainingBytes, fileHandle)) > 0 && !ferror(fileHandle))
  {
    remainingBytes -= writtenBytesForCurrentBlock;
    writePointer += writtenBytesForCurrentBlock;
    if (remainingBytes==0)
    {
      // completed
      break;
    }
  }
  
  writtenSize = dataSize-remainingBytes;
  if (remainingBytes>0)
  {
    LOG_ERROR("Filed to write data to file. Data size: " << dataSize << ", successfully written: " << writtenSize << " bytes");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}
