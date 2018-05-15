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
	os << indent << "RgbDataSourceName: " << RgbDataSourceName << std::endl;
	os << indent << "DepthDataSourceName: " << DepthDataSourceName << std::endl;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
	XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
	XML_READ_STRING_ATTRIBUTE_REQUIRED(RgbDataSourceName, deviceConfig);
	XML_READ_STRING_ATTRIBUTE_REQUIRED(DepthDataSourceName, deviceConfig);

	LOG_DEBUG("Configure RealSense Camera");

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
	
	rs2::context ctx;
	auto list = ctx.query_devices(); // Get a snapshot of currently connected devices
	if (list.size() == 0) { 
		LOG_ERROR("No Intel RealSense camera detected");
		return PLUS_FAIL;
	}
	
	this->num_devices = list.size();

	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_RGB8, 30);
	cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    
	this->profile = pipe.start(cfg);
	// Each depth camera might have different units for depth pixels, so we get it here
	// Using the pipeline's profile, we can retrieve the device that the pipeline uses
	//float depth_scale = get_depth_scale(profile.get_device());

	//Pipeline could choose a device that does not have a color stream
	//If there is no color stream, choose to align depth to another stream
	this->align_to = this->find_stream_to_align(profile.get_streams());
	// Create a rs2::align object.
	// rs2::align allows us to perform alignment of depth frames to others frames
	//The "align_to" is the stream type to which we plan to align depth frames.
	rs2::align align(align_to);

	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::InternalDisconnect()
{
	pipe.stop();
	
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusIntelRealSenseCamera::InternalUpdate()
{
	rs2::frameset frameset;

	if (this->num_devices < 1) {
		LOG_ERROR("vtkPlusIntelRealSenseCamera::InternalUpdate Unable to read date");
		return PLUS_SUCCESS;
	}
	
	frameset = pipe.wait_for_frames(); // Wait for next set of frames from the camera
	
	if (this->profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
	{
		//If the profile was changed, update the align object, and also get the new device's depth scale
		this->profile = pipe.get_active_profile();
		this->align_to = this->find_stream_to_align(profile.get_streams());
	}

	rs2::align align(align_to);
	//Get processed aligned frame
	auto processed = align.process(frameset);

	rs2::video_frame color = processed.first(align_to);
	rs2::depth_frame depth = processed.get_depth_frame();

	if (aSourceRGB == nullptr)
	{
		LOG_ERROR("vtkPlusIntelRealSenseCamera::InternalUpdate Unable to grab a RGB video source. Skipping frame.");
		return PLUS_FAIL;
	}
	
	if (aSourceRGB->GetNumberOfItems() == 0)
	{
	// Init the buffer with the metadata from the first frame
		aSourceRGB->SetImageType(US_IMG_RGB_COLOR);
		aSourceRGB->SetPixelType(VTK_UNSIGNED_CHAR);
		aSourceRGB->SetNumberOfScalarComponents(3);
		aSourceRGB->SetInputFrameSize(color.get_width(), color.get_height(), 1);
	}
	
	FrameSizeType frameSizeColor = { static_cast<unsigned int>(color.get_width()), static_cast<unsigned int>(color.get_height()), 1 };
	if (aSourceRGB->AddItem((void *)color.get_data(), aSourceRGB->GetInputImageOrientation(), frameSizeColor, VTK_UNSIGNED_CHAR, 3, US_IMG_RGB_COLOR, 0, this->FrameNumber) == PLUS_FAIL)
	{
		LOG_ERROR("vtkPlusIntelRealSenseCamera::InternalUpdate Unable to send a DEPTH image.");
		return PLUS_FAIL;
	}

	if (aSourceDEPTH == nullptr)
	{
		LOG_ERROR("vtkPlusIntelRealSenseCamera::InternalUpdate Unable to grab a RGB video source. Skipping frame.");
		return PLUS_FAIL;
	} 

	FrameSizeType frameSize = { static_cast<unsigned int>(depth.get_width()), static_cast<unsigned int>(depth.get_height()), 1 };
	if (aSourceDEPTH->GetNumberOfItems() == 0)
	{
		// Init the buffer with the metadata from the first frame
		aSourceDEPTH->SetImageType(US_IMG_BRIGHTNESS);
		aSourceDEPTH->SetPixelType(VTK_TYPE_UINT16);
		aSourceDEPTH->SetNumberOfScalarComponents(1);
		aSourceDEPTH->SetInputFrameSize(depth.get_width(), depth.get_height(), 1);
	}

	if (aSourceDEPTH->AddItem((void *)depth.get_data(), this->aSourceDEPTH->GetInputImageOrientation(),frameSize,	VTK_TYPE_UINT16, 1,	US_IMG_BRIGHTNESS, 0, this->FrameNumber) == PLUS_FAIL)
	{
			LOG_ERROR("vtkPlusIntelRealSenseCamera::InternalUpdate Unable to send a DEPTH image.");
			return PLUS_FAIL;
	}

	this->FrameNumber++; 

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
	
	if (this->GetDataSource(this->RgbDataSourceName, aSourceRGB) != PLUS_SUCCESS)
	{
		LOG_ERROR("Unable to locate data source for RGB camera: " << this->RgbDataSourceName);
		return PLUS_FAIL;
	}

	if (this->GetDataSource(this->DepthDataSourceName, aSourceDEPTH) != PLUS_SUCCESS)
	{
		LOG_ERROR("Unable to locate data source for DEPTH camera: " << this->DepthDataSourceName);
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

rs2_stream vtkPlusIntelRealSenseCamera::find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
	//Given a vector of streams, we try to find a depth stream and another stream to align depth with.
	//We prioritize color streams to make the view look better.
	//If color is not available, we take another stream that (other than depth)
	rs2_stream align_to = RS2_STREAM_ANY;
	bool depth_stream_found = false;
	bool color_stream_found = false;
	for (rs2::stream_profile sp : streams)
	{
		rs2_stream profile_stream = sp.stream_type();
		if (profile_stream != RS2_STREAM_DEPTH)
		{
			if (!color_stream_found)         //Prefer color
				align_to = profile_stream;

			if (profile_stream == RS2_STREAM_COLOR)
			{
				color_stream_found = true;
			}
		}
		else
		{
			depth_stream_found = true;
		}
	}

	if (!depth_stream_found)
		throw std::runtime_error("No Depth stream available");

	if (align_to == RS2_STREAM_ANY)
		throw std::runtime_error("No stream found to align with Depth");

	return align_to;
}

bool vtkPlusIntelRealSenseCamera::profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
	for (auto&& sp : prev)
	{
		//If previous profile is in current (maybe just added another)
		auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
		if (itr == std::end(current)) //If it previous stream wasn't found in current
		{
			return true;
		}
	}
	return false;
}
