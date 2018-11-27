/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusCommon_h
#define __PlusCommon_h

// PLUS includes
#include "vtkPlusCommonExport.h"
#include "itkImageIOBase.h"
#include "vtkIGSIOLogger.h"
#include "vtkPlusMacro.h"
#include "vtkIGSIORecursiveCriticalSection.h"

// VTK includes
#include <vtkImageData.h>
#include <vtksys/SystemTools.hxx>

// ITK includes
#include <itkImage.h>

// System includes
#include <float.h>

// STL includes
#include <array>
#include <list>
#include <locale>
#include <sstream>

// IGSIO includes
#include <igsioCommon.h>
#include <vtkIGSIOTrackedFrameList.h>
#include <igsioTrackedFrame.h>
#include <igsioVideoFrame.h>
#include <vtkIGSIOTransformRepository.h>

class vtkPlusUsScanConvert;

typedef igsioStatus PlusStatus;
#define PLUS_FAIL IGSIO_FAIL
#define PLUS_SUCCESS IGSIO_SUCCESS

class vtkXMLDataElement;

namespace PlusCommon
{
  typedef itk::ImageIOBase::IOComponentType ITKScalarPixelType;
  typedef int VTKScalarPixelType;
  typedef int IGTLScalarPixelType;

  vtkPlusCommonExport std::string GetPlusLibVersionString();

  vtkPlusCommonExport PlusStatus WriteToFile(igsioTrackedFrame* frame, const std::string& filename, vtkMatrix4x4* imageToTracker);
  
#ifdef PLUS_USE_OpenIGTLink
  /*! Convert between ITK and IGTL scalar pixel types */
  vtkPlusCommonExport IGTLScalarPixelType GetIGTLScalarPixelTypeFromVTK(igsioCommon::VTKScalarPixelType vtkScalarPixelType);

  /*! Convert between IGTL and ITK scalar pixel types */
  vtkPlusCommonExport VTKScalarPixelType GetVTKScalarPixelTypeFromIGTL(igsioCommon::IGTLScalarPixelType igtlPixelType);
#endif

  /*! Convert 3D vtkImageData to 3D itkImage */
  template<typename ScalarType> vtkPlusCommonExport PlusStatus DeepCopyVtkVolumeToItkVolume(vtkImageData* inFrame, typename itk::Image< ScalarType, 3 >::Pointer outFrame);
  /*! Convert 2D/3D vtkImageData to 2D itkImage (take only first slice if 3D) */
  template<typename ScalarType> vtkPlusCommonExport PlusStatus DeepCopyVtkVolumeToItkImage(vtkImageData* inFrame, typename itk::Image< ScalarType, 2 >::Pointer outFrame);

};

#include "vtkPlusConfig.h"
#include "PlusXmlUtils.h"
#include "PlusCommon.txx"

#endif //__PlusCommon_h
