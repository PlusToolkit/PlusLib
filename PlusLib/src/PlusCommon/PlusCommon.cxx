#include "PlusCommon.h"
#include "vtksys/SystemTools.hxx"
#include "vtksys/RegularExpression.hxx"
#include "vtkXMLDataElement.h"

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

  // Check if the string has only one valid 'To' phrase 
  vtksys::RegularExpression isValid("To([A-Z].*)"); 
  int numOfMatch(0); 
  if ( isValid.find(aTransformName) )
  {
    numOfMatch++; 
    std::string subString = isValid.match(1); 
    while ( isValid.find(subString) )
    {
      numOfMatch++;
      subString = isValid.match(1); 
    }
  }

  if ( numOfMatch != 1 )
  {
    LOG_ERROR("Unable to parse transform name, there are " << numOfMatch 
      << " matching 'To' phrases in the transform name '" << aTransformName << "', while exactly one allowed."); 
    return PLUS_FAIL; 
  }

  // Find <FrameFrom>To<FrameTo> matches 
  vtksys::RegularExpression regexp("(^[A-Z].*)To([A-Z].*)"); 
  if ( !regexp.find(aTransformName) )
  {
    LOG_ERROR("Failed to set transform name - unable to match '" << aTransformName << "' to '[FrameFrom]To[FrameTo]' expression!");
    return PLUS_FAIL; 
  }

  // Set coordinate frame names 
  this->m_From = regexp.match(1); 
  this->m_To = regexp.match(2); 
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
void PlusCommon::Trim(std::string &str)
{
  str.erase(str.find_last_not_of(" \t\r\n")+1);
  str.erase(0,str.find_first_not_of(" \t\r\n"));
}

//-------------------------------------------------------
std::string PlusCommon::Trim(const char* c)
{
  std::string str = c; 
  str.erase(str.find_last_not_of(" \t\r\n")+1);
  str.erase(0,str.find_first_not_of(" \t\r\n"));
  return str; 
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
    const unsigned int MATRIX_ELEM_COUNT=16;
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
      os << matrixIndent << matrixValues[12] << "\t" << matrixValues[13] << "\t" << matrixValues[14] << "\t" << matrixValues[15];
      os << "\"";
      if (!printEachAttributeInNewLine)
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
    os << "/>\n";
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
  of.imbue(vtkstd::locale::classic());
  return PlusCommon::PrintXML(of, vtkIndent(), elem);
}
