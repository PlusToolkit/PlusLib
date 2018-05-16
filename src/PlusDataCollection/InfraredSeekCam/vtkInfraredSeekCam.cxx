/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.

Developed by MACBIOIDI-ULPGC & IACTEC-IAC group
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkInfraredSeekCam.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkInfraredSeekCam);

//----------------------------------------------------------------------------
vtkInfraredSeekCam::vtkInfraredSeekCam()
{
  temp = true;
  existsBias = false;
  existsFlat = false;
  this->RequireImageOrientationInConfiguration = true;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkInfraredSeekCam::~vtkInfraredSeekCam()
{
}

//----------------------------------------------------------------------------
void vtkInfraredSeekCam::PrintSelf(ostream& os, vtkIndent indent)
{

  this->Superclass::PrintSelf(os, indent);

  os << indent << "InfraredSeekCam: Pro Seek Camera" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  std::string flatFile;
  std::string biasFile;

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  // XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(FlatFile, flatFile, deviceConfig);
  // XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(BiasFile, biasFile, deviceConfig);
  // XML_READ_BOOL_ATTRIBUTE_OPTIONAL(temp, deviceConfig);
  //
  // if (!flatFile.empty()) {
  //   LOG_INFO("Detected Flat File: " << flatFile);
  //   existsFlat = readImage(Flat, flatFile);
  // }
  //
  // if (!biasFile.empty()) {
  //   LOG_INFO("Detected Bias File: " << biasFile);
  //   existsBias = readImage(Bias, biasFile);;
  // }
  //
  // if (!existsBias || !existsFlat) {
  //   return PLUS_FAIL;
  // }

  LOG_DEBUG("Configure Pro Seek Camera");
  return PLUS_SUCCESS;
}

void vtkInfraredSeekCam::Settemp(bool value) {
  temp = value;
}

bool vtkInfraredSeekCam::readImage(cv::Mat &output, const std::string& filename) const {
  // Try to read with opencv read function
  cv::Mat aux = cv::imread(filename, 0);
  if (aux.data) {
    aux.convertTo(output, CV_32F);
    return true;
  }
  // If that didnt work, try to read as a binary file
  if (!readBinaryFile(filename, output)) {
    LOG_ERROR("Failed to open " << filename << " file");
    return false;
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::FreezeDevice(bool freeze)
{
  if (freeze)
    this->Disconnect();
  else
    this->Connect();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalConnect()
{
  // Select the camera. Seek Pro is default.
  this->Capture = std::make_shared<LibSeek::SeekThermalPro>();
  this->Frame = std::make_shared<cv::Mat>();

  if (!this->Capture->open())
  {
    LOG_ERROR("Failed to open seek pro");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalDisconnect()
{
  this->Capture = nullptr; // automatically closes resources/connections
  this->Frame = nullptr;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::InternalUpdate()
{
  if (!this->Capture->isOpened())
  {
    // No need to update if we're not able to read data
    LOG_ERROR("vtkInfraredSeekCam::InternalUpdate Unable to read date");
    return PLUS_SUCCESS;
  }

  // Capture one frame from the SeekPro capture device
  if (!this->Capture->read(*this->Frame))
  {
    LOG_ERROR("Unable to receive frame");
    return PLUS_FAIL;
  }
  // if (!this->Capture->read(FrameInt))
  // {
  //   LOG_ERROR("Unable to receive frame");
  //   return PLUS_FAIL;
  // }
  //
  // FrameInt.convertTo(FrameFloat, CV_32F);
  //
  // if (existsFlat) {
  //   FrameFloat = FrameFloat.mul(Flat);
  // }
  //
  // if (existsBias) {
  //   FrameFloat -= Bias;
  // }
  //
  // if (temp) {
  //   FrameFloat *= CALIBRATION1;
  //   FrameFloat -= CALIBRATION2;
  // }
  //
  // *this->Frame = FrameFloat;

  vtkPlusDataSource* aSource(nullptr);
  if (this->GetFirstActiveOutputVideoSource(aSource) == PLUS_FAIL || aSource == nullptr)
  {
    LOG_ERROR("Unable to grab a video source. Skipping frame.");
    return PLUS_FAIL;
  }

  if (aSource->GetNumberOfItems() == 0)
  {
    // Init the buffer with the metadata from the first frame
    aSource->SetImageType(US_IMG_BRIGHTNESS);
    aSource->SetPixelType(VTK_TYPE_UINT16);
    aSource->SetNumberOfScalarComponents(1);
    aSource->SetInputFrameSize(this->Frame->cols, this->Frame->rows, 1);
  }

  // Add the frame to the stream buffer
  FrameSizeType frameSize = { static_cast<unsigned int>(this->Frame->cols), static_cast<unsigned int>(this->Frame->rows), 1 };
  if (aSource->AddItem(this->Frame->data, aSource->GetInputImageOrientation(), frameSize, VTK_TYPE_UINT16, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  this->FrameNumber++;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkInfraredSeekCam::NotifyConfigured()
{
  if (this->OutputChannels.size() > 1)
  {
    LOG_WARNING("vtkInfraredSeekCam is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if (this->OutputChannels.empty())
  {
    LOG_ERROR("No output channels defined for vtkInfraredSeekCam. Cannot proceed.");
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

bool vtkInfraredSeekCam::readBinaryFile(const std::string& filename, cv::Mat& mat) const {
    float rows, cols;
    try {

      std::ifstream ifs(filename, std::ios::binary);
      if(!ifs.is_open()) {
        return false;
      }

      ifs.read((char*)(&rows), sizeof(float));
      ifs.read((char*)(&cols), sizeof(float));
      if (rows == 0 || cols == 0) {
        return false;
      }

      mat.release();
      mat.create(rows, cols, CV_32F);
      ifs.read((char*)(mat.data), mat.elemSize() * mat.total());
      return true;

    } catch (const std::ifstream::failure& ex) {
      LOG_ERROR(ex.what() << " at " << filename);
      return false;
    }
}
