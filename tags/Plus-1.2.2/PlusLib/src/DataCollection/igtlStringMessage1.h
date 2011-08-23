
#ifndef __igtlStringMessage1_h
#define __igtlStringMessage1_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"


namespace igtl
{


/**
 * This class enables string messages in programs that still use
 * OpenIGTLink protocol 1.
 */
class IGTLCommon_EXPORT StringMessage1: public MessageBase
{
public:
  typedef StringMessage1                 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro( igtl::StringMessage1, igtl::MessageBase );
  igtlNewMacro( igtl::StringMessage1 );

public:

  int        SetString( const char* string );
  int        SetString( std::string & string );
  
  const char* GetString();
  
  
protected:
  StringMessage1();
  ~StringMessage1();
  
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::string  m_String;
  
};


} // namespace igtl

#endif // _igtlStringMessage_h
