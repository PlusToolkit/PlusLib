/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusCommon.h"
#include "PlusRevision.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkXMLDataElement.h>
#include <vtksys/SystemTools.hxx>

// MetaIO includes
#include "metaImage.h"

// STL includes
#include <algorithm>
#include <string>

#ifdef PLUS_USE_OpenIGTLink
// IGTL includes
#include <igtlImageMessage.h>
#endif

//----------------------------------------------------------------------------
std::string PlusCommon::GetPlusLibVersionString()
{
  std::string plusLibVersion = std::string("Plus-") + std::string(PLUSLIB_VERSION) + "." + std::string(PLUSLIB_SHORT_REVISION);
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


//----------------------------------------------------------------------------
PlusStatus PlusCommon::WriteToFile(igsioTrackedFrame* frame, const std::string& filename, vtkMatrix4x4* imageToTracker)
{
  vtkImageData* volumeToSave = frame->GetImageData()->GetImage();

  MET_ValueEnumType scalarType = MET_NONE;
  switch (volumeToSave->GetScalarType())
  {
  case VTK_UNSIGNED_CHAR:
    scalarType = MET_UCHAR;
    break;
  case VTK_FLOAT:
    scalarType = MET_FLOAT;
    break;
  default:
    LOG_ERROR("Scalar type is not supported!");
    return PLUS_FAIL;
  }

  MetaImage metaImage(volumeToSave->GetDimensions()[0], volumeToSave->GetDimensions()[1], volumeToSave->GetDimensions()[2],
                      volumeToSave->GetSpacing()[0], volumeToSave->GetSpacing()[1], volumeToSave->GetSpacing()[2],
                      scalarType, volumeToSave->GetNumberOfScalarComponents(), volumeToSave->GetScalarPointer());
  double origin[3];
  origin[0] = imageToTracker->Element[0][3];
  origin[1] = imageToTracker->Element[1][3];
  origin[2] = imageToTracker->Element[2][3];
  metaImage.Origin(origin);
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      metaImage.Orientation(i, j, imageToTracker->Element[i][j]);
    }
  }
  // By definition, LPS orientation in DICOM sense = RAI orientation in MetaIO. See details at:
  // http://www.itk.org/Wiki/Proposals:Orientation#Some_notes_on_the_DICOM_convention_and_current_ITK_usage
  metaImage.AnatomicalOrientation("RAI");
  metaImage.BinaryData(true);
  metaImage.CompressedData(true);
  metaImage.ElementDataFileName("LOCAL");
  if (metaImage.Write(filename.c_str()) == false)
  {
    LOG_ERROR("Failed to save reconstructed volume in sequence metafile!");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

#ifdef PLUS_USE_OpenIGTLink
//----------------------------------------------------------------------------
// static
PlusCommon::VTKScalarPixelType PlusCommon::GetVTKScalarPixelTypeFromIGTL(PlusCommon::IGTLScalarPixelType igtlPixelType)
{
  switch (igtlPixelType)
  {
  case igtl::ImageMessage::TYPE_INT8:
    return VTK_CHAR;
  case igtl::ImageMessage::TYPE_UINT8:
    return VTK_UNSIGNED_CHAR;
  case igtl::ImageMessage::TYPE_INT16:
    return VTK_SHORT;
  case igtl::ImageMessage::TYPE_UINT16:
    return VTK_UNSIGNED_SHORT;
  case igtl::ImageMessage::TYPE_INT32:
    return VTK_INT;
  case igtl::ImageMessage::TYPE_UINT32:
    return VTK_UNSIGNED_INT;
  case igtl::ImageMessage::TYPE_FLOAT32:
    return VTK_FLOAT;
  case igtl::ImageMessage::TYPE_FLOAT64:
    return VTK_DOUBLE;
  default:
    return VTK_VOID;
  }
}

//----------------------------------------------------------------------------
// static
PlusCommon::IGTLScalarPixelType PlusCommon::GetIGTLScalarPixelTypeFromVTK(PlusCommon::VTKScalarPixelType pixelType)
{
  switch (pixelType)
  {
  case VTK_CHAR:
    return igtl::ImageMessage::TYPE_INT8;
  case VTK_UNSIGNED_CHAR:
    return igtl::ImageMessage::TYPE_UINT8;
  case VTK_SHORT:
    return igtl::ImageMessage::TYPE_INT16;
  case VTK_UNSIGNED_SHORT:
    return igtl::ImageMessage::TYPE_UINT16;
  case VTK_INT:
    return igtl::ImageMessage::TYPE_INT32;
  case VTK_UNSIGNED_INT:
    return igtl::ImageMessage::TYPE_UINT32;
  case VTK_FLOAT:
    return igtl::ImageMessage::TYPE_FLOAT32;
  case VTK_DOUBLE:
    return igtl::ImageMessage::TYPE_FLOAT64;
  default:
    // There is no unknown IGT scalar pixel type, so display an error message
    //**LOG_ERROR("Unknown conversion between VTK scalar pixel type (" << pixelType << ") and IGT pixel type - return igtl::ImageMessage::TYPE_INT8 by default!");
    return igtl::ImageMessage::TYPE_INT8;
  }
}

#endif
