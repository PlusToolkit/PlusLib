/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <string>
#include <stdlib.h>
#include "itkUlteriusImageIO.h"
#include "itkExceptionObject.h"
//#include "itkSpatialOrientation.h"
//#include "itkSpatialOrientationAdapter.h"
#include "itkIOCommon.h"
#include <itksys/SystemTools.hxx>

#include "ulterius_def.h"

namespace itk
{

  UlteriusImageIO::UlteriusImageIO()
  {
    m_FileType = Binary;
    m_FileHeaderPtr = new uFileHeader;

    // :TODO: check all the supported file formats (extensions) on the Ultrasonix GUI and add them here

    /* From Ulterius1.2.1 SDK: 
    udtScreen->.avi
    udtBPre->.bpr
    udtBPost->.b8
    udtBPost32->.b32
    udtRF->.rf
    udtMPre->.mpr
    udtMPost->.m
    udtPWRF->.drf
    udtPWSpectrum->.pw
    udtColorRF->.crf
    udtColorPost->.col
    udtColorVelocityVariance->.cvv
    udtElastoCombined->.el
    udtElastoOverlay->.elo
    udtElastoPre->.epr
    udtECG->.ecg
    */

    this->AddSupportedReadExtension(".b8");
    this->AddSupportedReadExtension(".b32");
  } 

  UlteriusImageIO::~UlteriusImageIO()
  {
    delete m_FileHeaderPtr;
    m_FileHeaderPtr = NULL;
  }

  void UlteriusImageIO::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Type: " << m_FileHeaderPtr->type << "\n";
    os << indent << "Number of frames: " << m_FileHeaderPtr->frames << "\n";  
  }

  // This method will only test if the header looks like a
  // UlteriusImage.
  bool UlteriusImageIO::CanReadFile( const char* filename ) 
  { 
    // First check the extension
    std::string fname = filename;
    if(  fname == "" )
    {
      itkDebugMacro(<<"No filename specified.");
      return false;
    }

    if (!ReadHeader(fname.c_str()))
    {
      return false;
    }

    switch (m_FileHeaderPtr->type)
    {
    case udtBPost:
    case udtBPost32:
      //supported data type
      break;
    default:
      // unknown data type
      return false;
    }

    // :TODO: add some more checks (e.g., size is >0, sample size is not too big, not too small, etc.

    return true;
  }


  void UlteriusImageIO::ReadImageInformation()
  { 
    if (!ReadHeader(m_FileName.c_str()))
    {
      itkExceptionMacro("File cannot be read: "
        << this->GetFileName() << " for reading."
        << std::endl
        << "Reason: cannot read header, "
        << itksys::SystemTools::GetLastSystemError());
    }


    switch (m_FileHeaderPtr->type)
    {
    case udtBPost:
      this->SetNumberOfComponents(1);
      this->SetPixelType( SCALAR );
      this->SetComponentType( CHAR );
      break;
    case udtBPost32:
      this->SetNumberOfComponents(3);
      this->SetPixelType( SCALAR );
      this->SetComponentType( CHAR );
      break;
    default:
      // unknown data type
      itkExceptionMacro("File cannot be read: "
        << this->GetFileName() << " for reading."
        << std::endl
        << "Reason: unknown data type " << m_FileHeaderPtr->type);
    }

    if (m_FileHeaderPtr->frames<=0)
    {
      itkExceptionMacro("File cannot be read: "
        << this->GetFileName() << " for reading."
        << std::endl
        << "Reason: invalid number of frames: " << m_FileHeaderPtr->frames);
    }

    this->SetNumberOfDimensions(3);

    this->SetDimensions(0, m_FileHeaderPtr->w);
    this->SetDimensions(1, m_FileHeaderPtr->h);
    this->SetDimensions(2, m_FileHeaderPtr->frames);

    // :TODO: get spacing info, if possible
    // it might be computed from line density (if pitch and native # elements is known)
    this->SetSpacing(0, 1.0);
    this->SetSpacing(1, 1.0);
    this->SetSpacing(2, 1.0);

    this->SetOrigin(0, 0);
    this->SetOrigin(1, 0);
    this->SetOrigin(2, 0);

    vnl_vector< double > directionAxis( this->GetNumberOfDimensions() );

    directionAxis[0]=1.0;
    directionAxis[1]=0.0;
    directionAxis[2]=0.0;
    this->SetDirection( 0, directionAxis );

    directionAxis[0]=0.0;
    directionAxis[1]=1.0;
    directionAxis[2]=0.0;
    this->SetDirection( 1, directionAxis );

    directionAxis[0]=0.0;
    directionAxis[1]=0.0;
    directionAxis[2]=1.0;
    this->SetDirection( 2, directionAxis );

  } 


  void UlteriusImageIO::Read(void* buffer)
  { 
    std::ifstream file;
    file.open( m_FileName.c_str(), std::ios::in | std::ios::binary );

    // Set the file read pointer to the start of the pixel data
    file.seekg(sizeof(uFileHeader), std::ios::beg );

    file.read(static_cast<char*>(buffer), static_cast<std::streamsize>(this->GetImageSizeInBytes()));    
    // :TODO: add error handling here
  } 

  bool UlteriusImageIO::CanWriteFile( const char * name )
  {
    return false;
  }


  void 
    UlteriusImageIO
    ::WriteImageInformation(void)
  {
  }

  void 
    UlteriusImageIO
    ::Write( const void* buffer) 
  {
  }

  bool UlteriusImageIO::ReadHeader(const char* filename)
  {
    std::ifstream   local_InputStream;
    local_InputStream.open( filename, std::ios::in | std::ios::binary );
    if( local_InputStream.fail() )
    {
      return false;
    }
    if( ! this->ReadBufferAsBinary( local_InputStream,
      (void *)this->m_FileHeaderPtr,
      sizeof(uFileHeader) ) )
    {
      local_InputStream.close();
      return false;
    }
    local_InputStream.close(); 

    return true;
  }

} // end namespace itk


