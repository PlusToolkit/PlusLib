/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __itkFcsvReader_h
#define __itkFcsvReader_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"

#include <vector>
#include <fstream>
#include "itkProcessObject.h"
#include "itkSpatialObject.h"
#include "itkMetaEvent.h"
#include <itkDefaultStaticMeshTraits.h>
#include "itkSpatialObjectReader.h"

#include "itkImageIOBase.h"

namespace itk
{
/*  \class FcsvData
 *  \stores Fiducial information
 *  \ingroup IOFilters
 *  \Stored data: Fiducial label , (x,y,z) , selected ,  visibility
 *  \Limitations: 2D or 3D??? (if 2D, Z is not needed)
 */

////////////////////////////////////////////////////////////////////////////////
/// Fcsv Data definition.
////////////////////////////////////////////////////////////////////////////////
class FcsvPoint 
{
	public:
		std::string label;
		double position[3];
		int selected;  
		int visibility;	
};

////////////////////////////////////////////////////////////////////////////////
/// FCSV data file header.
////////////////////////////////////////////////////////////////////////////////
struct FcsvData
{
		std::string filePath;	// C:/Documents and Settings/peikarih/L.fcsv
		std::string name;		//= L
		int numPoints;			//= 16
		int symbolScale;		// = 5
		int symbolType;			// = 11
		int visibility;			//= 1
		float version;			//= 2
		float textScale;		//= 4.5
		float color[3];			//= 0.4,1,1 
		float selectedColor[3]; //= 1,0.5,0.5  
		float opacity;			//= 1 
		float ambient;			//= 0
		float diffuse;			//= 1  
		float specular;			//= 0  
		float power;			//= 1
		int   locked;			//= 0
		// :TODO: use std<std::string,int> instead
		std::vector<std::string> columns;			//= label,x,y,z,sel,vis
		int numberingScheme;	//= 0
		std::vector<FcsvPoint> points;				
};

static const int FcsvNDimensions=3;

class PLUS_EXPORT FcsvReader : public Object
{
public:

  // SmartPointer typedef support 
  typedef FcsvReader                Self;									// correct????
  typedef SmartPointer<Self>                  Pointer;

  // Method for creation through the object factory 
  itkNewMacro(Self);

  // Run-time type information (and related methods). 
  typedef Object Superclass;
  itkTypeMacro(FcsvReader, Object);

  //Load a FCSV file.
  void Update(void);

  // Set the filename  
  itkSetStringMacro(FileName);

  // Get the filename 
  itkGetStringMacro(FileName);

  FcsvData* GetFcsvDataObject() { return &m_FcsvDataObject; };
protected:
  FcsvReader(const Self&);		//purposely not implemented
  Self& operator=(const Self&);	//purposely not implemented

  std::string m_FileName;

  FcsvReader();
  virtual ~FcsvReader();

private:
  FcsvData m_FcsvDataObject;
};


} // namespace itk

#endif // __FcsvReader_h
