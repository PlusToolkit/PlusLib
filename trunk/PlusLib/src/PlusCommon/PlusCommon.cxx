#include "PlusCommon.h"
#include "vtksys/SystemTools.hxx"
#include "vtksys/RegularExpression.hxx"


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
bool PlusTransformName::IsValid()
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

  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus PlusTransformName::GetTransformName(std::string& aTransformName)
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
std::string PlusTransformName::From()
{
  this->Capitalize(this->m_From); 
  return this->m_From; 
}

//-------------------------------------------------------
std::string PlusTransformName::To()
{
  this->Capitalize(this->m_To); 
  return this->m_To; 
}

//-------------------------------------------------------
void PlusTransformName::Capitalize(std::string& aString )
{
  // Check starting character 
  vtksys::RegularExpression capitalize("^[a-z].*"); 
  if ( capitalize.find(aString) )
  {
    aString = vtksys::SystemTools::Capitalized(aString);
    LOG_WARNING("Coordinate frame name should be capitalized, changed to " << aString ); 
  }
}


