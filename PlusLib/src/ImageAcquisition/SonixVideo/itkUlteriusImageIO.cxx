/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkUlteriusImageIO.cxx,v $
Language:  C++
Date:      $Date: 2009-12-15 14:28:19 $
Version:   $Revision: 1.104 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
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

namespace itk
{

	UlteriusImageIO::UlteriusImageIO()
	{
		m_FileType = Binary;

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
	}

	void UlteriusImageIO::PrintSelf(std::ostream& os, Indent indent) const
	{
		Superclass::PrintSelf(os, indent);
		os << indent << "Type: " << m_FileHeader.type << "\n";
		os << indent << "Number of frames: " << m_FileHeader.frames << "\n";  
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

		switch (m_FileHeader.type)
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


		switch (m_FileHeader.type)
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
				<< "Reason: unknown data type " << m_FileHeader.type);
		}

		if (m_FileHeader.frames<=0)
		{
			itkExceptionMacro("File cannot be read: "
				<< this->GetFileName() << " for reading."
				<< std::endl
				<< "Reason: invalid number of frames: " << m_FileHeader.frames);
		}

		this->SetNumberOfDimensions(3);

		this->SetDimensions(0, m_FileHeader.w);
		this->SetDimensions(1, m_FileHeader.h);
		this->SetDimensions(2, m_FileHeader.frames);

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
			(void *)&(this->m_FileHeader),
			sizeof(uFileHeader) ) )
		{
			local_InputStream.close();
			return false;
		}
		local_InputStream.close(); 

		return true;
	}

} // end namespace itk


