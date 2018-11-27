/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "PlusFidSegmentation.h"
#include "vtkMath.h"

#include <limits.h>
#include <iostream>
#include <algorithm>

#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPNGImageIO.h"

static const short BLACK            = 0;
static const short MIN_WINDOW_DIST  = 8;
static const short MAX_CLUSTER_VALS = 16384;

const double PlusFidSegmentation::DEFAULT_APPROXIMATE_SPACING_MM_PER_PIXEL = 0.078;
const double PlusFidSegmentation::DEFAULT_MORPHOLOGICAL_OPENING_CIRCLE_RADIUS_MM = 0.27;
const double PlusFidSegmentation::DEFAULT_MORPHOLOGICAL_OPENING_BAR_SIZE_MM = 2.0;
const int PlusFidSegmentation::DEFAULT_CLIP_ORIGIN[2] = { 27, 27 };
const int PlusFidSegmentation::DEFAULT_CLIP_SIZE[2] = { 766, 562 };
const double PlusFidSegmentation::DEFAULT_MAX_LINE_PAIR_DISTANCE_ERROR_PERCENT = 10.0;
const double PlusFidSegmentation::DEFAULT_ANGLE_TOLERANCE_DEGREES = 10.0;
const double PlusFidSegmentation::DEFAULT_MAX_ANGLE_DIFFERENCE_DEGREES = 10.0;
const double PlusFidSegmentation::DEFAULT_MIN_THETA_DEGREES = -70.0;
const double PlusFidSegmentation::DEFAULT_MAX_THETA_DEGREES = 70.0;
const double PlusFidSegmentation::DEFAULT_MAX_LINE_SHIFT_MM = 10.0;
const double PlusFidSegmentation::DEFAULT_THRESHOLD_IMAGE_PERCENT = 10.0;
const double PlusFidSegmentation::DEFAULT_COLLINEAR_POINTS_MAX_DISTANCE_FROM_LINE_MM = 0.6;
const char* PlusFidSegmentation::DEFAULT_USE_ORIGINAL_IMAGE_INTENSITY_FOR_DOT_INTENSITY_SCORE = "FALSE";
const int PlusFidSegmentation::DEFAULT_NUMBER_OF_MAXIMUM_FIDUCIAL_POINT_CANDIDATES = 20;

//-----------------------------------------------------------------------------

PlusFidSegmentation::PlusFidSegmentation()
  : m_UseOriginalImageIntensityForDotIntensityScore(false)
  , m_NumberOfMaximumFiducialPointCandidates(DEFAULT_NUMBER_OF_MAXIMUM_FIDUCIAL_POINT_CANDIDATES)
  , m_ThresholdImagePercent(-1)
  , m_MorphologicalOpeningBarSizeMm(-1)
  , m_MorphologicalOpeningCircleRadiusMm(-1)
  , m_PossibleFiducialsImageFilename("")
  , m_FiducialGeometry(CALIBRATION_PHANTOM_6_POINT)
  , m_ApproximateSpacingMmPerPixel(-1)
  , m_DotsFound(false)
  , m_NumDots(-1.0)
  , m_Working(new PlusFidSegmentation::PixelType[1])
  , m_Dilated(new PlusFidSegmentation::PixelType[1])
  , m_Eroded(new PlusFidSegmentation::PixelType[1])
  , m_UnalteredImage(new PlusFidSegmentation::PixelType[1])
  , m_DebugOutput(false)
{
  //Initialization of member variables
  m_FrameSize[0] = 0;
  m_FrameSize[1] = 0;
  m_FrameSize[2] = 0;

  m_RegionOfInterest[0] = 0;
  m_RegionOfInterest[1] = 0;
  m_RegionOfInterest[2] = 0;
  m_RegionOfInterest[3] = 0;

  for (int i = 0 ; i < 4 ; i++)
  {
    m_ImageScalingTolerancePercent[i] = -1.0;
  }

  for (int i = 0 ; i < 3 ; i++)
  {
    m_ImageNormalVectorInPhantomFrameEstimation[i] = -1.0;
  }

  for (int i = 0 ; i < 6 ; i++)
  {
    m_ImageNormalVectorInPhantomFrameMaximumRotationAngleDeg[i] = -1.0;
  }

  for (int i = 0 ; i < 16 ; i++)
  {
    m_ImageToPhantomTransform[i] = -1.0;
  }
}

//-----------------------------------------------------------------------------

PlusFidSegmentation::~PlusFidSegmentation()
{
  delete[] m_Dilated;
  delete[] m_Eroded;
  delete[] m_Working;
  delete[] m_UnalteredImage;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::UpdateParameters()
{
  LOG_TRACE("FidSegmentation::UpdateParameters");

  // Create morphological circle
  m_MorphologicalCircle.clear();
  int radiuspx = floor((m_MorphologicalOpeningCircleRadiusMm / m_ApproximateSpacingMmPerPixel) + 0.5);
  for (int x = -radiuspx; x <= radiuspx; x++)
  {
    for (int y = -radiuspx; y <= radiuspx; y++)
    {
      if (sqrt(pow(x, 2.0) + pow(y, 2.0)) <= radiuspx)
      {
        PlusCoordinate2D dot;
        dot.X = y;
        dot.Y = x;
        m_MorphologicalCircle.push_back(dot);
      }
    }
  }
}

//-----------------------------------------------------------------------------

PlusStatus PlusFidSegmentation::ReadConfiguration(vtkXMLDataElement* configData)
{
  LOG_TRACE("FidSegmentation::ReadConfiguration");

  XML_FIND_NESTED_ELEMENT_REQUIRED(phantomDefinition, configData, "PhantomDefinition");
  XML_FIND_NESTED_ELEMENT_REQUIRED(description, phantomDefinition, "Description");
  XML_READ_ENUM3_ATTRIBUTE_OPTIONAL(FiducialGeometry, description, "Multi-N", CALIBRATION_PHANTOM_MULTI_NWIRE, "CIRS", CIRS_PHANTOM_13_POINT, "Double-N", CALIBRATION_PHANTOM_6_POINT);

  XML_FIND_NESTED_ELEMENT_OPTIONAL(segmentationParameters, configData, "Segmentation");
  if (!segmentationParameters)
  {
    segmentationParameters = igsioXmlUtils::GetNestedElementWithName(configData, "Segmentation");
    PlusFidSegmentation::SetDefaultSegmentationParameters(segmentationParameters);
  }

  XML_READ_SCALAR_ATTRIBUTE_WARNING(double, ApproximateSpacingMmPerPixel, segmentationParameters);
  XML_READ_SCALAR_ATTRIBUTE_WARNING(double, MorphologicalOpeningCircleRadiusMm, segmentationParameters);
  XML_READ_SCALAR_ATTRIBUTE_WARNING(double, MorphologicalOpeningBarSizeMm, segmentationParameters);

  // Segmentation search region Y direction
  int clipOrigin[2] = {0};
  int clipSize[2] = {0};
  if (segmentationParameters->GetVectorAttribute("ClipRectangleOrigin", 2, clipOrigin) &&
      segmentationParameters->GetVectorAttribute("ClipRectangleSize", 2, clipSize))
  {
    m_RegionOfInterest[0] = clipOrigin[0];
    m_RegionOfInterest[1] = clipOrigin[1];
    m_RegionOfInterest[2] = clipOrigin[0] + clipSize[0];
    m_RegionOfInterest[3] = clipOrigin[1] + clipSize[1];
  }
  else
  {
    LOG_INFO("Cannot find ClipRectangleOrigin or ClipRectangleSize attribute in the SegmentationParameters configuration file; Using the largest ROI possible.");
  }

  XML_READ_SCALAR_ATTRIBUTE_WARNING(double, ThresholdImagePercent, segmentationParameters);

  int intUseOriginalImageIntensityForDotIntensityScore = 0;
  if (segmentationParameters->GetScalarAttribute("UseOriginalImageIntensityForDotIntensityScore", intUseOriginalImageIntensityForDotIntensityScore))
  {
    LOG_WARNING("UseOriginalImageIntensityForDotIntensityScore attribute expected to have 'TRUE' or 'FALSE' value. Numerical values ('1' or '0') are deprecated.");
    SetUseOriginalImageIntensityForDotIntensityScore(intUseOriginalImageIntensityForDotIntensityScore != 0);
  }
  else
  {
    XML_READ_BOOL_ATTRIBUTE_OPTIONAL(UseOriginalImageIntensityForDotIntensityScore, segmentationParameters);
  }


  // If the tolerance parameters are computed automatically
  int computeSegmentationParametersFromPhantomDefinition(0);
  if (segmentationParameters->GetScalarAttribute("ComputeSegmentationParametersFromPhantomDefinition", computeSegmentationParametersFromPhantomDefinition)
      && computeSegmentationParametersFromPhantomDefinition != 0)
  {
    double imageScalingTolerancePercent[4] = {0};
    if (segmentationParameters->GetVectorAttribute("ImageScalingTolerancePercent", 4, imageScalingTolerancePercent))
    {
      for (int i = 0; i < 4 ; i++)
      {
        m_ImageScalingTolerancePercent[i] = imageScalingTolerancePercent[i];
      }
    }
    else
    {
      LOG_WARNING("Could not read imageScalingTolerancePercent from configuration file.");
    }

    double imageNormalVectorInPhantomFrameEstimation[3] = {0};
    if (segmentationParameters->GetVectorAttribute("ImageNormalVectorInPhantomFrameEstimation", 3, imageNormalVectorInPhantomFrameEstimation))
    {
      m_ImageNormalVectorInPhantomFrameEstimation[0] = imageNormalVectorInPhantomFrameEstimation[0];
      m_ImageNormalVectorInPhantomFrameEstimation[1] = imageNormalVectorInPhantomFrameEstimation[1];
      m_ImageNormalVectorInPhantomFrameEstimation[2] = imageNormalVectorInPhantomFrameEstimation[2];
    }
    else
    {
      LOG_WARNING("Could not read imageNormalVectorInPhantomFrameEstimation from configuration file.");
    }
  }

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NumberOfMaximumFiducialPointCandidates, segmentationParameters);

  UpdateParameters();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::SetFrameSize(const FrameSizeType& frameSize)
{
  LOG_TRACE("FidSegmentation::SetFrameSize(" << frameSize[0] << ", " << frameSize[1] << ")");

  if ((m_FrameSize[0] == frameSize[0]) && (m_FrameSize[1] == frameSize[1]))
  {
    return;
  }

  if ((m_FrameSize[0] != 0) && (m_FrameSize[1] != 0))
  {
    delete[] m_Dilated;
    delete[] m_Eroded;
    delete[] m_Working;
    delete[] m_UnalteredImage;
  }

  m_FrameSize[0] = frameSize[0];
  m_FrameSize[1] = frameSize[1];
  m_FrameSize[2] = 1;

  // Create working images (after deleting them in case they were already created)
  long size = m_FrameSize[0] * m_FrameSize[1];
  m_Dilated = new PlusFidSegmentation::PixelType[size];
  m_Eroded = new PlusFidSegmentation::PixelType[size];
  m_Working = new PlusFidSegmentation::PixelType[size];
  m_UnalteredImage = new PlusFidSegmentation::PixelType[size];

  // Set ROI to the largest possible if not already set
  if ((m_RegionOfInterest[0] == 0) || (m_RegionOfInterest[1] == 0) || (m_RegionOfInterest[2] == 0) || (m_RegionOfInterest[3] == 0))
  {
    unsigned int barSize = GetMorphologicalOpeningBarSizePx();
    m_RegionOfInterest[0] = barSize + 1;
    m_RegionOfInterest[1] = barSize + 1;
    m_RegionOfInterest[2] = m_FrameSize[0] - barSize - 1;
    m_RegionOfInterest[3] = m_FrameSize[1] - barSize - 1;
  }
  else
  {
    // Check the search region in case it was set to too big (with the additional bar size it would go out of image)
    unsigned int barSize = GetMorphologicalOpeningBarSizePx();
    if (m_RegionOfInterest[0] - barSize <= 0)
    {
      m_RegionOfInterest[0] = barSize + 1;
      LOG_WARNING("The region of interest is too big, bar size is " << barSize);
    }
    if (m_RegionOfInterest[1] - barSize <= 0)
    {
      m_RegionOfInterest[1] = barSize + 1;
      LOG_WARNING("The region of interest is too big, bar size is " << barSize);
    }
    if (m_RegionOfInterest[2] + barSize >= m_FrameSize[0])
    {
      m_RegionOfInterest[2] = m_FrameSize[0] - barSize - 1;
      LOG_WARNING("The region of interest is too big, bar size is " << barSize);
    }
    if (m_RegionOfInterest[3] + barSize >= m_FrameSize[1])
    {
      m_RegionOfInterest[3] = m_FrameSize[1] - barSize - 1;
      LOG_WARNING("The region of interest is too big, bar size is " << barSize);
    }
  }
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Clear()
{
  //LOG_TRACE("FidSegmentation::Clear");

  m_DotsVector.clear();
  m_CandidateFidValues.clear();
}

//-----------------------------------------------------------------------------

unsigned int PlusFidSegmentation::GetMorphologicalOpeningBarSizePx()
{
  //LOG_TRACE("FidSegmentation::GetMorphologicalOpeningBarSizePx");

  return static_cast<unsigned int>(floor(m_MorphologicalOpeningBarSizeMm / m_ApproximateSpacingMmPerPixel + 0.5));
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::ErodePoint0(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::ErodePoint0");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = UCHAR_MAX;
  unsigned int p = ir * m_FrameSize[0] + ic - barSize; // current pixel - bar size (position of the start of the bar)
  unsigned int p_max = ir * m_FrameSize[0] + ic + barSize; // current pixel +  bar size (position of the end  of the bar)

  //find lowest intensity in bar shaped area in image
  for (; p <= p_max; p++)
  {
    if (image[p] < dval)
    {
      dval = image[p];
    }
    if (image[p] == 0)
    {
      break;
    }
  }

  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Erode0(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Erode0");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  for (unsigned int ir = m_RegionOfInterest[1]; ir < m_RegionOfInterest[3]; ir++)
  {
    unsigned int ic = m_RegionOfInterest[0];
    unsigned int p_base = ir * m_FrameSize[0];

    PlusFidSegmentation::PixelType dval = ErodePoint0(image, ir, ic);   // find lowest pixel intensity in surrounding region ( positions +/- 8 of current pixel position)
    dest[p_base + ic] = dval; // p_base+ic = current pixel

    for (ic++; ic < m_RegionOfInterest[2]; ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[p_base + ic + barSize];
      PlusFidSegmentation::PixelType del_val = image[p_base + ic - 1 - barSize];

      dval = new_val <= dval ? new_val  : // dval = new val if new val is less than or equal to dval
             del_val > dval ? std::min(dval, new_val) :   // if del val is greater than dval, dval= min of dval and new val
             ErodePoint0(image, ir, ic);   //else dval = result of erode function

      dest[ir * m_FrameSize[0] + ic] = dval; // update new "eroded" picture
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::ErodePoint45(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::ErodePoint45");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  PlusFidSegmentation::PixelType dval = UCHAR_MAX;
  unsigned int p = (ir + barSize) * m_FrameSize[0] + ic - barSize;
  unsigned int p_max = (ir - barSize) * m_FrameSize[0] + ic + barSize;

  for (; p >= p_max; p = p - m_FrameSize[0] + 1)
  {
    if (image[p] < dval)
    {
      dval = image[p];
    }
    if (image[p] == 0)
    {
      break;
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Erode45(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Erode45");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));
  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  /* Down the left side. */
  for (unsigned int sr = m_RegionOfInterest[1]; sr < m_RegionOfInterest[3]; sr++)
  {
    unsigned int ir = sr;
    unsigned int ic = m_RegionOfInterest[0];

    PlusFidSegmentation::PixelType dval = ErodePoint45(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ir--, ic++; ir >= m_RegionOfInterest[1] && ic < m_RegionOfInterest[2]; ir--, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir - barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir + 1 + barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val <= dval ? new_val :
             del_val > dval ? std::min(dval, new_val) :
             ErodePoint45(image, ir, ic);

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }

  /* Across the bottom */
  for (unsigned int sc = m_RegionOfInterest[0]; sc < m_RegionOfInterest[2]; sc++)
  {
    unsigned int ic = sc;
    unsigned int ir = m_RegionOfInterest[3] - 1;

    PlusFidSegmentation::PixelType dval = ErodePoint45(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ir--, ic++; ir >= m_RegionOfInterest[1] && ic < m_RegionOfInterest[2]; ir--, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir - barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir + 1 + barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val <= dval ? new_val :
             del_val > dval ? std::min(dval, new_val) :
             ErodePoint45(image, ir, ic);

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::ErodePoint90(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::ErodePoint90");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = UCHAR_MAX;
  unsigned int p = (ir - barSize) * m_FrameSize[0] + ic;
  unsigned int p_max = (ir + barSize) * m_FrameSize[0] + ic;

  for (; p <= p_max; p += m_FrameSize[0])
  {
    if (image[p] < dval)
    {
      dval = image[p];
    }
    if (image[p] == 0)
    {
      break;
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Erode90(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Erode90");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  for (unsigned int ic = m_RegionOfInterest[0]; ic < m_RegionOfInterest[2]; ic++)
  {
    unsigned int ir = m_RegionOfInterest[1];

    PlusFidSegmentation::PixelType dval = ErodePoint90(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ir++; ir < m_RegionOfInterest[3]; ir++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + ic];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + ic];

      dval = new_val <= dval ? new_val :
             del_val > dval ? std::min(dval, new_val) :
             ErodePoint90(image, ir, ic);

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::ErodePoint135(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::ErodePoint135");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = UCHAR_MAX;
  unsigned int p = (ir - barSize) * m_FrameSize[0] + ic - barSize;
  unsigned int p_max = (ir + barSize) * m_FrameSize[0] + ic + barSize;

  for (; p <= p_max; p = p + m_FrameSize[0] + 1)
  {
    if (image[p] < dval)
    {
      dval = image[p];
    }
    if (image[p] == 0)
    {
      break;
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Erode135(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Erode135");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  /* Up the left side. */
  for (unsigned int sr = m_RegionOfInterest[3] - 1; sr >= m_RegionOfInterest[1]; sr--)
  {
    unsigned int ir = sr;
    unsigned int ic = m_RegionOfInterest[0];

    PlusFidSegmentation::PixelType dval = ErodePoint135(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ir++, ic++; ir < m_RegionOfInterest[3] && ic < m_RegionOfInterest[2]; ir++, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val <= dval ? new_val :
             del_val > dval ? std::min(dval, new_val) :
             ErodePoint135(image, ir, ic);

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }

  /* Across the top. */
  for (unsigned int sc = m_RegionOfInterest[0]; sc < m_RegionOfInterest[2]; sc++)
  {
    unsigned int ic = sc;
    unsigned int ir = m_RegionOfInterest[1];

    PlusFidSegmentation::PixelType dval = ErodePoint135(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ir++, ic++; ir < m_RegionOfInterest[3] && ic < m_RegionOfInterest[2]; ir++, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val <= dval ? new_val :
             del_val > dval ? std::min(dval, new_val) :
             ErodePoint135(image, ir, ic);

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::ErodeCircle(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::ErodeCircle");

  unsigned int slen = m_MorphologicalCircle.size();

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));

  for (unsigned int ir = m_RegionOfInterest[1]; ir < m_RegionOfInterest[3]; ir++)
  {
    for (unsigned int ic = m_RegionOfInterest[0]; ic < m_RegionOfInterest[2]; ic++)
    {
      PlusFidSegmentation::PixelType dval = UCHAR_MAX;
      for (unsigned int sp = 0; sp < slen; sp++)
      {
        int sr = ir + m_MorphologicalCircle[sp].X;
        int sc = ic + m_MorphologicalCircle[sp].Y;
        PlusFidSegmentation::PixelType pixSrc = image[sr * m_FrameSize[0] + sc];

        if (pixSrc < dval)
        {
          dval = pixSrc;
        }

        if (pixSrc == 0)
        {
          break;
        }
      }

      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::DilatePoint0(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::DilatePoint0");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = 0;
  unsigned int p = ir * m_FrameSize[0] + ic - barSize;
  unsigned int p_max = ir * m_FrameSize[0] + ic + barSize;

  for (; p <= p_max; p++)
  {
    if (image[p] > dval)
    {
      dval = image[p];
    }
  }

  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Dilate0(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Dilate0");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  for (unsigned int ir = m_RegionOfInterest[1]; ir < m_RegionOfInterest[3]; ir++)
  {
    unsigned int ic = m_RegionOfInterest[0];
    unsigned int p_base = ir * m_FrameSize[0];

    PlusFidSegmentation::PixelType dval = DilatePoint0(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;

    for (ic++; ic < m_RegionOfInterest[2]; ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[p_base + ic + barSize];
      PlusFidSegmentation::PixelType del_val = image[p_base + ic - 1 - barSize];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint0(image, ir, ic));
      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::DilatePoint45(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::DilatePoint45");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = 0;
  unsigned int p = (ir + barSize) * m_FrameSize[0] + ic - barSize;
  unsigned int p_max = (ir - barSize) * m_FrameSize[0] + ic + barSize;

  while (p >= p_max)
  {
    if (image[p] > dval)
    {
      dval = image[p];
    }
    if (p - m_FrameSize[0] + 1 > p)
    {
      // unsigned int underflow, adjust
      p = 0;
    }
    else
    {
      p = p - m_FrameSize[0] + 1;
    }
  }

  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Dilate45(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Dilate45");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));
  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  /* Down the left side. */
  for (unsigned int sr = m_RegionOfInterest[1]; sr < m_RegionOfInterest[3]; sr++)
  {
    unsigned int ir = sr;
    unsigned int ic = m_RegionOfInterest[0];

    PlusFidSegmentation::PixelType dval = DilatePoint45(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval ;
    for (ir--, ic++; ir >= m_RegionOfInterest[1] && ic < m_RegionOfInterest[2]; ir--, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir - barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir + 1 + barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint45(image, ir, ic));
      dest[ir * m_FrameSize[0] + ic] = dval ;
    }
  }

  /* Across the bottom */
  for (unsigned int sc = m_RegionOfInterest[0]; sc < m_RegionOfInterest[2]; sc++)
  {
    unsigned int ic = sc;
    unsigned int ir = m_RegionOfInterest[3] - 1;

    PlusFidSegmentation::PixelType dval = DilatePoint45(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval ;
    for (ir--, ic++; ir >= m_RegionOfInterest[1] && ic < m_RegionOfInterest[2]; ir--, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir - barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir + 1 + barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint45(image, ir, ic));
      dest[ir * m_FrameSize[0] + ic] = dval ;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::DilatePoint90(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::DilatePoint90");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = 0;
  unsigned int p = (ir - barSize) * m_FrameSize[0] + ic;
  unsigned int p_max = (ir + barSize) * m_FrameSize[0] + ic;

  for (; p <= p_max; p += m_FrameSize[0])
  {
    if (image[p] > dval)
    {
      dval = image[p];
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Dilate90(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Dilate90");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));
  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  for (unsigned int ic = m_RegionOfInterest[0]; ic < m_RegionOfInterest[2]; ic++)
  {
    unsigned int ir = m_RegionOfInterest[1];

    PlusFidSegmentation::PixelType dval = DilatePoint90(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval ;
    for (ir++; ir < m_RegionOfInterest[3]; ir++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + ic];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + ic];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint90(image, ir, ic));

      dest[ir * m_FrameSize[0] + ic] = dval ;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::DilatePoint135(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic)
{
  //LOG_TRACE("FidSegmentation::DilatePoint135");

  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();
  PlusFidSegmentation::PixelType dval = 0;
  unsigned int p = (ir - barSize) * m_FrameSize[0] + ic - barSize;
  unsigned int p_max = (ir + barSize) * m_FrameSize[0] + ic + barSize;

  for (; p <= p_max; p = p + m_FrameSize[0] + 1)
  {
    if (image[p] > dval)
    {
      dval = image[p];
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Dilate135(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::Dilate135");

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));
  const unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  /* Up the left side. */
  for (unsigned int sr = m_RegionOfInterest[3] - 1; sr >= m_RegionOfInterest[1]; sr--)
  {
    unsigned int ir = sr;
    unsigned int ic = m_RegionOfInterest[0];

    PlusFidSegmentation::PixelType dval = DilatePoint135(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval ;
    for (ir++, ic++; ir < m_RegionOfInterest[3] && ic < m_RegionOfInterest[2]; ir++, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint135(image, ir, ic));
      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }

  /* Across the top. */
  for (unsigned int sc = m_RegionOfInterest[0]; sc < m_RegionOfInterest[2]; sc++)
  {
    unsigned int ic = sc;
    unsigned int ir = m_RegionOfInterest[1];

    PlusFidSegmentation::PixelType dval = DilatePoint135(image, ir, ic);
    dest[ir * m_FrameSize[0] + ic] = dval;
    for (ir++, ic++; ir < m_RegionOfInterest[3] && ic < m_RegionOfInterest[2]; ir++, ic++)
    {
      PlusFidSegmentation::PixelType new_val = image[(ir + barSize) * m_FrameSize[0] + (ic + barSize)];
      PlusFidSegmentation::PixelType del_val = image[(ir - 1 - barSize) * m_FrameSize[0] + (ic - 1 - barSize)];

      dval = new_val >= dval ? new_val :
             (del_val < dval ? std::max(dval, new_val) :
              DilatePoint135(image, ir, ic));
      dest[ir * m_FrameSize[0] + ic] = dval;
    }
  }
}

//-----------------------------------------------------------------------------

inline PlusFidSegmentation::PixelType PlusFidSegmentation::DilatePoint(PlusFidSegmentation::PixelType* image, unsigned int ir, unsigned int ic, PlusCoordinate2D* shape, int slen)
{
  //LOG_TRACE("FidSegmentation::DilatePoint");

  PlusFidSegmentation::PixelType dval = 0;
  for (int sp = 0; sp < slen; sp++)
  {
    unsigned int sr = ir + shape[sp].Y;
    unsigned int sc = ic + shape[sp].X;

    if (image[sr * m_FrameSize[0] + sc] > dval)
    {
      dval = image[sr * m_FrameSize[0] + sc];
    }
  }
  return dval;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::DilateCircle(PlusFidSegmentation::PixelType* dest, PlusFidSegmentation::PixelType* image)
{
  //LOG_TRACE("FidSegmentation::DilateCircle");

  unsigned int slen = m_MorphologicalCircle.size();

  PlusCoordinate2D* shape = new PlusCoordinate2D[slen];

  for (unsigned int i = 0; i < slen; i++)
  {
    shape[i] = m_MorphologicalCircle[i];
  }

  /* Which elements stick around when you shift right? */
  int n = 0;

  bool* sr_exist = new bool[slen];

  memset(sr_exist, 0, slen * sizeof(bool));
  for (unsigned int si = 0; si < slen; si++)
  {
    PlusCoordinate2D dot;
    dot.X = m_MorphologicalCircle[si].X + 1;
    dot.Y = m_MorphologicalCircle[si].Y;

    if (ShapeContains(m_MorphologicalCircle, dot))
    {
      sr_exist[si] = true, n++;
    }
  }
  //cout << "shift_exist: " << n << endl;

  PlusCoordinate2D* newDots = new PlusCoordinate2D[slen];
  PlusCoordinate2D* oldDots = new PlusCoordinate2D[slen];

  int nNewDots = 0, nOldDots = 0;
  for (unsigned int si = 0; si < slen; si++)
  {
    if (sr_exist[si])
    {
      oldDots[nOldDots++] = shape[si];
    }
    else
    {
      newDots[nNewDots++] = shape[si];
    }
  }

  delete [] sr_exist;

  memset(dest, 0, m_FrameSize[1]*m_FrameSize[0]*sizeof(PlusFidSegmentation::PixelType));
  for (unsigned int ir = m_RegionOfInterest[1]; ir < m_RegionOfInterest[3]; ir++)
  {
    unsigned int ic = m_RegionOfInterest[0];

    PlusFidSegmentation::PixelType dval = DilatePoint(image, ir, ic, shape, slen);
    PlusFidSegmentation::PixelType last = dest[ir * m_FrameSize[0] + ic] = dval;

    for (ic++; ic < m_RegionOfInterest[2]; ic++)
    {
      PlusFidSegmentation::PixelType dval = DilatePoint(image, ir, ic, newDots, nNewDots);

      if (dval < last)
      {
        for (int sp = 0; sp < nOldDots; sp++)
        {
          unsigned int sr = ir + oldDots[sp].Y;
          unsigned int sc = ic + oldDots[sp].X;
          if (image[sr * m_FrameSize[0] + sc] > dval)
          {
            dval = image[sr * m_FrameSize[0] + sc];
          }
          if (image[sr * m_FrameSize[0] + sc] == last)
          {
            break;
          }
        }
      }
      last = dest[ir * m_FrameSize[0] + ic] = dval ;
    }
  }
  delete [] newDots;
  delete [] oldDots;
}

//-----------------------------------------------------------------------------

bool PlusFidSegmentation::ShapeContains(std::vector<PlusCoordinate2D>& shape, PlusCoordinate2D point)
{
  //LOG_TRACE("FidSegmentation::ShapeContains");

  for (unsigned int si = 0; si < shape.size(); si++)
  {
    if (shape[si] == point)
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::WritePng(PlusFidSegmentation::PixelType* modifiedImage, std::string outImageName, int cols, int rows)
{
  //LOG_TRACE("FidSegmentation::WritePng");

  // output intermediate image
  const unsigned int Dimension = 2;

  typedef itk::Image< PlusFidSegmentation::PixelType, Dimension > ImageType;
  ImageType::Pointer modImage = ImageType::New();
  ImageType::SizeType size;
  size[0] = cols;
  size[1] = rows;

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  ImageType::RegionType wholeImage;
  wholeImage.SetSize(size);
  wholeImage.SetIndex(start);

  modImage->SetRegions(wholeImage);
  modImage->Allocate();

  typedef itk::ImageFileWriter< ImageType > WriterType;
  itk::PNGImageIO::Pointer pngImageIO = itk::PNGImageIO::New();
  pngImageIO->SetCompressionLevel(0);

  WriterType::Pointer writer = WriterType::New();
  writer->SetImageIO(pngImageIO);
  writer->SetFileName(outImageName);


  typedef itk::ImageRegionIterator<ImageType> IterType;
  IterType iter(modImage, modImage->GetRequestedRegion());
  iter.GoToBegin();

  int count = 0;

  while (!iter.IsAtEnd())
  {
    iter.Set(modifiedImage[count]);
    count++;
    ++iter;
  }

  writer->SetInput(modImage);    // piping output of reader into input of writer

  try
  {
    writer->Update(); // change to writing if want writing feature
  }
  catch (itk::ExceptionObject& err)
  {
    LOG_ERROR("Exception! writer did not update");   //ditto
    LOG_ERROR(err);
  }
  // end output

}

//-----------------------------------------------------------------------------
void PlusFidSegmentation::SetDefaultSegmentationParameters(vtkXMLDataElement* segmentationDataElement)
{
  if (!segmentationDataElement)
  {
    return;
  }

  segmentationDataElement->SetName("Segmentation");
  segmentationDataElement->SetDoubleAttribute("ApproximateSpacingMmPerPixel", DEFAULT_APPROXIMATE_SPACING_MM_PER_PIXEL);
  segmentationDataElement->SetDoubleAttribute("MorphologicalOpeningCircleRadiusMm", DEFAULT_MORPHOLOGICAL_OPENING_CIRCLE_RADIUS_MM);
  segmentationDataElement->SetDoubleAttribute("MorphologicalOpeningBarSizeMm", DEFAULT_MORPHOLOGICAL_OPENING_BAR_SIZE_MM);
  segmentationDataElement->SetDoubleAttribute("MaxLinePairDistanceErrorPercent", DEFAULT_MAX_LINE_PAIR_DISTANCE_ERROR_PERCENT);
  segmentationDataElement->SetDoubleAttribute("AngleToleranceDegrees", DEFAULT_ANGLE_TOLERANCE_DEGREES);
  segmentationDataElement->SetDoubleAttribute("MaxAngleDifferenceDegrees", DEFAULT_MAX_ANGLE_DIFFERENCE_DEGREES);
  segmentationDataElement->SetDoubleAttribute("MinThetaDegrees", DEFAULT_MIN_THETA_DEGREES);
  segmentationDataElement->SetDoubleAttribute("MaxThetaDegrees", DEFAULT_MAX_THETA_DEGREES);
  segmentationDataElement->SetDoubleAttribute("MaxLineShiftMm", DEFAULT_MAX_LINE_SHIFT_MM);
  segmentationDataElement->SetDoubleAttribute("ThresholdImagePercent", DEFAULT_THRESHOLD_IMAGE_PERCENT);
  segmentationDataElement->SetDoubleAttribute("CollinearPointsMaxDistanceFromLineMm", DEFAULT_COLLINEAR_POINTS_MAX_DISTANCE_FROM_LINE_MM);
  segmentationDataElement->SetAttribute("UseOriginalImageIntensityForDotIntensityScore", DEFAULT_USE_ORIGINAL_IMAGE_INTENSITY_FOR_DOT_INTENSITY_SCORE);
  segmentationDataElement->SetDoubleAttribute("NumberOfMaximumFiducialPointCandidates", DEFAULT_NUMBER_OF_MAXIMUM_FIDUCIAL_POINT_CANDIDATES);

  std::stringstream clipOriginSS;
  clipOriginSS << DEFAULT_CLIP_ORIGIN[0] << " " << DEFAULT_CLIP_ORIGIN[1];
  std::string clipOriginString = clipOriginSS.str();
  segmentationDataElement->SetAttribute("ClipRectangleOrigin", clipOriginString.c_str());

  std::stringstream clipSizeSS;
  clipSizeSS << DEFAULT_CLIP_SIZE[0] << " " << DEFAULT_CLIP_SIZE[1];
  std::string clipSizeString = clipSizeSS.str();
  segmentationDataElement->SetAttribute("ClipRectangleSize", clipSizeString.c_str());
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::WritePossibleFiducialOverlayImage(const std::vector<PlusFidDot>& fiducials, PlusFidSegmentation::PixelType* unalteredImage, const char* namePrefix, int frameIndex)
{
  //LOG_TRACE("FidSegmentation::WritePossibleFiducialOverlayImage");

  typedef itk::RGBPixel< unsigned char >    ColorPixelType;
  typedef itk::Image< ColorPixelType, 2 >   ImageType;

  ImageType::Pointer possibleFiducials = ImageType::New();

  ImageType::SizeType size;
  size[0] = m_FrameSize[0];
  size[1] = m_FrameSize[1];

  ImageType::IndexType start;
  start[0] = 0;
  start[1] = 0;

  ImageType::RegionType wholeImage;
  wholeImage.SetSize(size);
  wholeImage.SetIndex(start);

  possibleFiducials->SetRegions(wholeImage);
  possibleFiducials->Allocate();

  ImageType::IndexType pixelLocation;

  ImageType::PixelType pixelValue;

  // copy pixel by pixel (we need to do gray->RGB conversion and only a ROI is updated)
  for (unsigned int r = m_RegionOfInterest[1]; r < m_RegionOfInterest[3]; r++)
  {
    for (unsigned int c = m_RegionOfInterest[0]; c < m_RegionOfInterest[2]; c++)
    {
      pixelValue[0] = 0; //unalteredImage[r*cols+c];
      pixelValue[1] = unalteredImage[r * m_FrameSize[0] + c];
      pixelValue[2] = unalteredImage[r * m_FrameSize[0] + c];
      pixelLocation[0] = c;
      pixelLocation[1] = r;
      possibleFiducials->SetPixel(pixelLocation, pixelValue);
    }
  }

  // Set pixelValue to red (it will be used to mark the centroid of the clusters)
  for (unsigned int numDots = 0; numDots < fiducials.size(); numDots++)
  {
    const int markerPosCount = 5;
    const int markerPos[markerPosCount][2] = {{0, 0}, { +1, 0}, { -1, 0}, {0, +1}, {0, -1}};

    for (int i = 0; i < markerPosCount; i++)
    {
      pixelLocation[0] = fiducials[numDots].GetX() + markerPos[i][0];
      pixelLocation[1] = fiducials[numDots].GetY() + markerPos[i][1];
      int clusterMarkerIntensity = fiducials[numDots].GetDotIntensity() * 10;
      if (clusterMarkerIntensity > 255)
      {
        clusterMarkerIntensity = 255;
      }
      pixelValue[0] = clusterMarkerIntensity;
      pixelValue[1] = 0;
      pixelValue[2] = 0;
      possibleFiducials->SetPixel(pixelLocation, pixelValue);
    }
  }
  std::ostringstream possibleFiducialsImageFilename;
  possibleFiducialsImageFilename << namePrefix << std::setw(3) << std::setfill('0') << frameIndex << ".bmp" << std::ends;

  SetPossibleFiducialsImageFilename(possibleFiducialsImageFilename.str());

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writeImage = WriterType::New();
  writeImage->SetFileName(m_PossibleFiducialsImageFilename);

  writeImage->SetInput(possibleFiducials);

  try
  {
    writeImage->Update();
  }
  catch (itk::ExceptionObject& err)
  {
    LOG_ERROR("Exception! writer did not update");
    LOG_ERROR(err);
  }
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::WritePossibleFiducialOverlayImage(const std::vector< std::vector<double> >& fiducials, PlusFidSegmentation::PixelType* unalteredImage, const char* namePrefix, int frameIndex)
{
  std::vector<PlusFidDot> dots;
  for (unsigned int numDots = 0; numDots < fiducials.size(); numDots++)
  {
    PlusFidDot newDot;
    newDot.SetX(fiducials[numDots][0]);
    newDot.SetY(fiducials[numDots][1]);
    newDot.SetDotIntensity(10.0);
    dots.push_back(newDot);
  }
  WritePossibleFiducialOverlayImage(dots, unalteredImage, namePrefix, frameIndex);
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::Subtract(PlusFidSegmentation::PixelType* image, PlusFidSegmentation::PixelType* vals)
{
  //LOG_TRACE("FidSegmentation::Subtract");

  for (unsigned int pos = m_FrameSize[1] * m_FrameSize[0]; pos > 0; pos--)
  {
    *image = *vals > *image ? 0 : *image - *vals;
    image++;
    vals++;
  }
}

//-----------------------------------------------------------------------------

/* Possible additional criteria:
 *  1. Track the frame-to-frame data?
 *  2. Lines should be roughly of the same length? */

void PlusFidSegmentation::Suppress(PlusFidSegmentation::PixelType* image, double percent_thresh)
{
  LOG_TRACE("FidSegmentation::Suppress");

  // Get the minimum and maximum pixel value
  PlusFidSegmentation::PixelType max = 0;
  PlusFidSegmentation::PixelType min = 255;
  PlusFidSegmentation::PixelType* pix = image;
  for (unsigned int pos = 0; pos < m_FrameSize[0]*m_FrameSize[1]; pos ++)
  {
    if (*pix > max)
    {
      max = *pix;
    }
    if (*pix < min)
    {
      min = *pix;
    }
    pix++;
  }

  //We use floor to calculate the round value here.

  PlusFidSegmentation::PixelType thresh = min + (PlusFidSegmentation::PixelType)floor((double)(max - min) * percent_thresh + 0.5);

  //thresholding
  int pixelCount = m_FrameSize[0] * m_FrameSize[1];
  PlusFidSegmentation::PixelType* pixel = image;
  for (int i = 0; i < pixelCount; i++)
  {
    if (*pixel < thresh)
    {
      *pixel = BLACK;
    }
    pixel++;
  }

  if (m_DebugOutput)
  {
    WritePng(image, "seg-suppress.png", m_FrameSize[0], m_FrameSize[1]);
  }

}

//-----------------------------------------------------------------------------

inline void PlusFidSegmentation::ClusteringAddNeighbors(PlusFidSegmentation::PixelType* image, int r, int c, std::vector<PlusFidDot>& testPosition, std::vector<PlusFidDot>& setPosition, std::vector<PlusFidSegmentation::PixelType>& valuesOfPosition)
{
  //LOG_TRACE("FidSegmentation::ClusteringAddNeighbors");

  if (image[r * m_FrameSize[0] + c] > 0 && testPosition.size() < MAX_CLUSTER_VALS && setPosition.size() < MAX_CLUSTER_VALS)
  {
    PlusFidDot dot;
    dot.SetY(r);
    dot.SetX(c);
    testPosition.push_back(dot);
    setPosition.push_back(dot);
    valuesOfPosition.push_back(image[r * m_FrameSize[0] + c]);
    image[r * m_FrameSize[0] + c] = 0;
  }
}

//-----------------------------------------------------------------------------

/* Should we accept a dot? */
inline bool PlusFidSegmentation::AcceptDot(PlusFidDot& dot)
{
  //LOG_TRACE("FidSegmentation::AcceptDot");

  if (dot.GetY() >= m_RegionOfInterest[1] + MIN_WINDOW_DIST &&
      dot.GetY() < m_RegionOfInterest[3] - MIN_WINDOW_DIST &&
      dot.GetX() >=  m_RegionOfInterest[0] + MIN_WINDOW_DIST &&
      dot.GetX() < m_RegionOfInterest[2] - MIN_WINDOW_DIST)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------

bool PlusFidSegmentation::Cluster(bool& tooManyCandidates)
{
  LOG_TRACE("FidSegmentation::Cluster");
  tooManyCandidates = false;

  std::vector<PlusFidDot> testPosition;
  std::vector<PlusFidDot> setPosition;
  std::vector<PlusFidSegmentation::PixelType> valuesOfPosition;

  for (unsigned int r = m_RegionOfInterest[1]; r < m_RegionOfInterest[3]; r++)
  {
    for (unsigned int c = m_RegionOfInterest[0]; c < m_RegionOfInterest[2]; c++)
    {
      if (m_Working[r * m_FrameSize[0] + c] > 0)
      {
        testPosition.clear();

        PlusFidDot dot;
        dot.SetX(c);
        dot.SetY(r);
        testPosition.push_back(dot);

        setPosition.clear();
        setPosition.push_back(dot);

        valuesOfPosition.clear();
        valuesOfPosition.push_back(m_Working[r * m_FrameSize[0] + c]);

        m_Working[r * m_FrameSize[0] + c] = 0;

        while (testPosition.size() > 0)
        {
          PlusFidDot dot = testPosition.back();
          testPosition.pop_back();

          ClusteringAddNeighbors(m_Working, dot.GetY() - 1, dot.GetX() - 1, testPosition, setPosition, valuesOfPosition);
          ClusteringAddNeighbors(m_Working, dot.GetY() - 1, dot.GetX(), testPosition, setPosition, valuesOfPosition);
          ClusteringAddNeighbors(m_Working, dot.GetY() - 1, dot.GetX() + 1, testPosition, setPosition, valuesOfPosition);

          ClusteringAddNeighbors(m_Working, dot.GetY(), dot.GetX() - 1, testPosition, setPosition, valuesOfPosition);
          ClusteringAddNeighbors(m_Working, dot.GetY(), dot.GetX() + 1, testPosition, setPosition, valuesOfPosition);

          ClusteringAddNeighbors(m_Working, dot.GetY() + 1, dot.GetX() - 1, testPosition, setPosition, valuesOfPosition);
          ClusteringAddNeighbors(m_Working, dot.GetY() + 1, dot.GetX(), testPosition, setPosition, valuesOfPosition);
          ClusteringAddNeighbors(m_Working, dot.GetY() + 1, dot.GetX() + 1, testPosition, setPosition, valuesOfPosition);
        }

        double dest_r = 0, dest_c = 0, total = 0;
        for (unsigned int p = 0; p < setPosition.size(); p++)
        {
          double amount = (double)valuesOfPosition[p] / (double)UCHAR_MAX;
          dest_r += setPosition[p].GetY() * amount;
          dest_c += setPosition[p].GetX() * amount;
          total += amount;
        }

        dot.SetY(dest_r / total);
        dot.SetX(dest_c / total);
        dot.SetDotIntensity(total);

        if (AcceptDot(dot))
        {
          if (m_UseOriginalImageIntensityForDotIntensityScore)
          {
            // Take into account intensities that are close to the dot center
            const double dotRadius2 = 3.0 * 3.0;
            double dest_r = 0, dest_c = 0, total = 0;
            for (unsigned int p = 0; p < setPosition.size(); p++)
            {
              if ((setPosition[p].GetY() - dot.GetY()) * (setPosition[p].GetY() - dot.GetY()) + (setPosition[p].GetX() - dot.GetX()) * (setPosition[p].GetX() - dot.GetX()) <= dotRadius2)
              {
                //double amount = (double)vals[p] / (double)UCHAR_MAX;
                double amount = (double)m_UnalteredImage[int(setPosition[p].GetY() * m_FrameSize[0] + setPosition[p].GetX())] / (double)UCHAR_MAX;
                dest_r += setPosition[p].GetY() * amount;
                dest_c += setPosition[p].GetX() * amount;
                total += amount;
              }
            }
            dot.SetDotIntensity(total);
          }

          m_DotsVector.push_back(dot);
        }
      }
    }
  }

  std::sort(m_DotsVector.begin(), m_DotsVector.end(), PlusFidDot::IntensityLessThan);
  if (m_DotsVector.size() > m_NumberOfMaximumFiducialPointCandidates)
  {
    LOG_WARNING("Too many candidates found in segmentation. Consider reducing ROI. " << m_DotsVector.size() << " > " << m_NumberOfMaximumFiducialPointCandidates);
    m_DotsVector.erase(m_DotsVector.begin() + m_NumberOfMaximumFiducialPointCandidates, m_DotsVector.begin() + m_DotsVector.size());
    tooManyCandidates = true;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::MorphologicalOperations()
{
  LOG_TRACE("FidSegmentation::MorphologicalOperations");

  // Constraint ROI according to the morphological bar size
  ValidateRegionOfInterest();

  if (m_RegionOfInterest[2] == 0 || m_RegionOfInterest[3] == 0 ||
      m_RegionOfInterest[0] >= m_RegionOfInterest[2] ||
      m_RegionOfInterest[1] >= m_RegionOfInterest[3])
  {
    // the image is empty, nothing to process
    return;
  }

  // Morphological operations with a stick-like structuring element
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg01-initial.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Erode0(m_Eroded, m_Working);
  if (m_DebugOutput)
  {
    WritePng(m_Eroded, "seg02-morph-bar-deg0-erode.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Dilate0(m_Dilated, m_Eroded);
  if (m_DebugOutput)
  {
    WritePng(m_Dilated, "seg03-morph-bar-deg0-dilated.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Subtract(m_Working, m_Dilated);
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg04-morph-bar-deg0-final.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Erode45(m_Eroded, m_Working);
  if (m_DebugOutput)
  {
    WritePng(m_Eroded, "seg05-morph-bar-deg45-erode.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Dilate45(m_Dilated, m_Eroded);
  if (m_DebugOutput)
  {
    WritePng(m_Dilated, "seg06-morph-bar-deg45-dilated.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Subtract(m_Working, m_Dilated);
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg07-morph-bar-deg45-final.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Erode90(m_Eroded, m_Working);
  if (m_DebugOutput)
  {
    WritePng(m_Eroded, "seg08-morph-bar-deg90-erode.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Dilate90(m_Dilated, m_Eroded);
  if (m_DebugOutput)
  {
    WritePng(m_Dilated, "seg09-morph-bar-deg90-dilated.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Subtract(m_Working, m_Dilated);
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg10-morph-bar-deg90-final.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Erode135(m_Eroded, m_Working);
  if (m_DebugOutput)
  {
    WritePng(m_Eroded, "seg11-morph-bar-deg135-erode.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Dilate135(m_Dilated, m_Eroded);
  if (m_DebugOutput)
  {
    WritePng(m_Dilated, "seg12-morph-bar-deg135-dilated.png", m_FrameSize[0], m_FrameSize[1]);
  }

  Subtract(m_Working, m_Dilated);
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg13-morph-bar-deg135-final.png", m_FrameSize[0], m_FrameSize[1]);
  }

  /* Circle operation. */
  ErodeCircle(m_Eroded, m_Working);
  if (m_DebugOutput)
  {
    WritePng(m_Eroded, "seg14-morph-circle-erode.png", m_FrameSize[0], m_FrameSize[1]);
  }

  DilateCircle(m_Working, m_Eroded);
  if (m_DebugOutput)
  {
    WritePng(m_Working, "seg15-morph-circle-final.png", m_FrameSize[0], m_FrameSize[1]);
  }

}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::SetRegionOfInterest(unsigned int xMin, unsigned int yMin, unsigned int xMax, unsigned int yMax)
{
  LOG_TRACE("FidSegmentation::SetRegionOfInterest(" << xMin << ", " << yMin << ", " << xMax << ", " << yMax << ")");

  if (xMin > 0)
  {
    m_RegionOfInterest[0] = xMin;
  }
  if (yMin > 0)
  {
    m_RegionOfInterest[1] = yMin;
  }
  if (xMax > 0)
  {
    m_RegionOfInterest[2] = xMax;
  }
  if (yMax > 0)
  {
    m_RegionOfInterest[3] = yMax;
  }
}

//-----------------------------------------------------------------------------

void PlusFidSegmentation::ValidateRegionOfInterest()
{
  LOG_TRACE("FidSegmentation::ValidateRegionOfInterest");

  unsigned int barSize = GetMorphologicalOpeningBarSizePx();

  if (m_FrameSize[0] < barSize * 2 + 1 || m_FrameSize[1] < barSize * 2 + 1)
  {
    // image is too small
    m_RegionOfInterest[0] = 0;
    m_RegionOfInterest[1] = 0;
    m_RegionOfInterest[2] = 0;
    m_RegionOfInterest[3] = 0;
    return;
  }

  if (m_RegionOfInterest[0] > m_RegionOfInterest[2])
  {
    std::swap(m_RegionOfInterest[0], m_RegionOfInterest[2]);
  }
  if (m_RegionOfInterest[1] > m_RegionOfInterest[3])
  {
    std::swap(m_RegionOfInterest[1], m_RegionOfInterest[3]);
  }

  // xmin
  if (m_RegionOfInterest[0] - barSize <= 0)
  {
    m_RegionOfInterest[0] = barSize + 1;
  }
  if (m_RegionOfInterest[0] + barSize >= m_FrameSize[0])
  {
    m_RegionOfInterest[0] = m_FrameSize[0] - barSize - 1;
  }

  // xmax
  if (m_RegionOfInterest[2] < m_RegionOfInterest[0])
  {
    m_RegionOfInterest[2] = m_RegionOfInterest[0];
  }
  if (m_RegionOfInterest[2] + barSize >= m_FrameSize[0])
  {
    m_RegionOfInterest[2] = m_FrameSize[0] - barSize - 1;
  }

  // ymin
  if (m_RegionOfInterest[1] - barSize <= 0)
  {
    m_RegionOfInterest[1] = barSize + 1;
  }
  if (m_RegionOfInterest[1] + barSize >= m_FrameSize[1])
  {
    m_RegionOfInterest[1] = m_FrameSize[1] - barSize - 1;
  }

  // ymax
  if (m_RegionOfInterest[3] < m_RegionOfInterest[1])
  {
    m_RegionOfInterest[3] = m_RegionOfInterest[1];
  }
  if (m_RegionOfInterest[3] + barSize >= m_FrameSize[1])
  {
    m_RegionOfInterest[3] = m_FrameSize[1] - barSize - 1;
  }
}

//-----------------------------------------------------------------------------
void PlusFidSegmentation::GetRegionOfInterest(unsigned int& xMin, unsigned int& yMin, unsigned int& xMax, unsigned int& yMax)
{
  xMin = m_RegionOfInterest[0];
  yMin = m_RegionOfInterest[1];
  xMax = m_RegionOfInterest[2];
  yMax = m_RegionOfInterest[3];
}

//-----------------------------------------------------------------------------
void PlusFidSegmentation::SetNumberOfMaximumFiducialPointCandidates(int aValue)
{
  m_NumberOfMaximumFiducialPointCandidates = aValue;
}

//-----------------------------------------------------------------------------
void PlusFidSegmentation::SetFiducialGeometry(FiducialGeometryType geometryType)
{
  m_FiducialGeometry = geometryType;
}
