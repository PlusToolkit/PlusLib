/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/
//Marker Location
//C:\devel\PlusExp-bin\PlusLibData\ConfigFiles\IntelRealSenseToolDefinitions
#include "vtkPlusIntelRealSenseCamera.h"

#include "PlusConfigure.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkObjectFactory.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusIntelRealSenseCamera);

//----------------------------------------------------------------------------
vtkPlusIntelRealSenseCamera::vtkPlusIntelRealSenseCamera()
{
	this->RequireImageOrientationInConfiguration = true;
	this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusIntelRealSenseCamera::~vtkPlusIntelRealSenseCamera()
{
}

//----------------------------------------------------------------------------
void vtkPlusIntelRealSenseCamera::PrintSelf(ostream& os, vtkIndent indent)
{

	this->Superclass::PrintSelf(os, indent);

	os << indent << "Intel RealSense 3d Camera: D415" << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
	XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
	LOG_DEBUG("Configure Pro Seek Camera");

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
	XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::FreezeDevice(bool freeze)
{
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::InternalConnect()
{
	this->pipe.start();
	// Select the camera. Seek Pro is default.
	/*this->Capture = std::make_shared<LibSeek::SeekThermalPro>();
	this->Frame = std::make_shared<cv::Mat>();

	if (!this->Capture->open())
	{
		LOG_ERROR("Failed to open seek pro");
		return PLUS_FAIL;
	}*/

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::InternalDisconnect()
{
	/*this->Capture = nullptr; // automatically closes resources/connections
	this->Frame = nullptr;*/

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::InternalUpdate()
{
	rs2::colorizer color_map;

	rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera

	rs2::frame depth = color_map(data.get_depth_frame()); // Find and colorize the depth data
	rs2::frame color = data.get_color_frame();

	/*if (!this->Capture->isOpened())
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

	this->FrameNumber++; */

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::NotifyConfigured()
{
	if (this->OutputChannels.size() > 2)
	{
		LOG_WARNING("vtkPlusIntelRealSenseCamera is expecting 2 output channel and there are " << this->OutputChannels.size() << " channels. 2 firsts outputs channel will be used.");
	}

	if (this->OutputChannels.size() < 2)
	{
		LOG_ERROR("vtkPlusIntelRealSenseCamera is expecting 2 output channel. Cannot proceed.");
		this->CorrectlyConfigured = false;
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}