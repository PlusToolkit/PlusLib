
#include "igtlStringMessage1.h"


#include "igtlutil/igtl_header.h"
#include "igtlutil/igtl_util.h"



namespace igtl {


StringMessage1
::StringMessage1()
  : MessageBase()
{
  this->m_DefaultBodyType = "STRING1";
  this->m_String.clear();
}



StringMessage1
::~StringMessage1()
{

}



int
StringMessage1
::SetString( const char *string )
{
  this->m_String = string;
  return (int) this->m_String.length();
}



int
StringMessage1
::SetString( std::string &string )
{
  this->m_String = string;
  return (int) this->m_String.length();
}



const char*
StringMessage1
::GetString()
{
  return this->m_String.c_str();
}



int
StringMessage1
::GetBodyPackSize()
{
  return sizeof( igtl_uint16 ) + this->m_String.length();
}



int
StringMessage1
::PackBody()
{
  AllocatePack();
  
  igtl_uint16* p_header;
  char* p_string;
  
  
  p_header = (igtl_uint16*)( this->m_Body );
  p_string = (char*)( this->m_Body + sizeof( igtl_uint16 ) );
  
  
    // Copy data.
  
  (*p_header) = static_cast< igtl_uint16 >( this->m_String.length() );
  strncpy( p_string, this->m_String.c_str(), this->m_String.length() );
  
  return 1;
}



int
StringMessage1
::UnpackBody()
{
  
  igtl_uint16* p_header;
  char* p_string;
  
  p_header = (igtl_uint16*)( this->m_Body );
  p_string = (char*)( this->m_Body + sizeof( igtl_uint16 ) );
  
  
    // Copy data.
  
  this->m_String.clear();
  this->m_String.append( p_string, (*p_header) );
  
  return 1;
}


} // namespace igtl

