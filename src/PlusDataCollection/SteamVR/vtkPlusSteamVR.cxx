/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusSteamVR.h"
#include "vtkPlusDataSource.h"

/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes

#include <vtkMatrix4x4.h>
#include <vtkXMLDataElement.h>
#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <sstream>

vtkStandardNewMacro(vtkPlusSteamVR);

void vtkPlusSteamVR::PrintSelf(ostream& os, vtkIndent indent)
{
	Superclass::PrintSelf(os, indent);
}

vtkPlusSteamVR::vtkPlusSteamVR()
	: vtkPlusDevice()
{
	this->FrameNumber = 0;
	this->StartThreadForInternalUpdates = true;
}

vtkPlusSteamVR::~vtkPlusSteamVR() {
	delete vr_context;
	delete vr_chaperone;
	delete overlay;

	vr_context = nullptr;
	vr_chaperone = nullptr;
	overlay = nullptr;


	HMD_DS = NULL;
	Controller_DS = NULL;
	GenericTracker_DS = NULL;
}

PlusStatus vtkPlusSteamVR::Probe(){

	LOG_TRACE("Searching for Tracking System:")

	if (vr::VR_IsHmdPresent())
	{
		LOG_TRACE("HMD found!");
		return PLUS_SUCCESS;
	}
	else
	{
		LOG_TRACE("No HMD was found in the system, quitting app");
		return PLUS_FAIL;
	}
}

PlusStatus vtkPlusSteamVR::InternalConnect() {

	vr::HmdError err;
	vr_context = vr::VR_Init(&err, vr::EVRApplicationType::VRApplication_Scene);
	/*
	vr_context == NULL ?
	 LOG_DEBUG("Error while initializing SteamVR runtime. Error code is " << vr::VR_GetVRInitErrorAsSymbol(err))
	  : LOG_DEBUG("SteamVR runtime successfully initialized");
	  */

	if (vr_context == nullptr) {
		LOG_DEBUG("Error while initializing SteamVR runtime. Error code is " << vr::VR_GetVRInitErrorAsSymbol(err));
		return PLUS_FAIL;
	}
	else
	{
		LOG_DEBUG("SteamVR runtime successfully initialized");

		int base_stations_count = 0;
		for (uint32_t td = vr::k_unTrackedDeviceIndex_Hmd; td < vr::k_unMaxTrackedDeviceCount; td++) {

		if (vr_context->IsTrackedDeviceConnected(td))
		{
			vr::ETrackedDeviceClass tracked_device_class = vr_context->GetTrackedDeviceClass(td);

			std::string td_type = GetTrackedDeviceClassString(tracked_device_class);
			tracked_device_type[td] = td_type;

			cout << "Tracking device " << td << " is connected " << endl;
			cout << "  Device type: " << td_type << ". Name: " << GetTrackedDeviceString(vr_context, td, vr::Prop_TrackingSystemName_String) << endl;

			if (tracked_device_class == vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference) base_stations_count++;

			if (td == vr::k_unTrackedDeviceIndex_Hmd)
			{
				// Fill variables used for obtaining the device name and serial ID (used later for naming the SDL window)
			}
		}
		else
			cout << "Tracking device " << td << " not connected" << endl;
		}

	// Check whether both base stations are found, not mandatory but just in case...
		if (base_stations_count < 2)
		{
			cout << "There was a problem indentifying the base stations, please check they are powered on" << endl;

			return PLUS_FAIL;
		}
	}
	return PLUS_SUCCESS;
}

PlusStatus vtkPlusSteamVR::InternalDisconnect() {
	LOG_TRACE("Shutting down Tracking System");
	vr::VR_Shutdown();
	return this->Disconnect();
}

PlusStatus vtkPlusSteamVR::InternalUpdate() {

	if (vr_context != NULL)
	{
		// Obtain tracking device poses
		vr_context->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, 0, tracked_device_pose, vr::k_unMaxTrackedDeviceCount);

		vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

		int tracked_device_count = 0;
		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; nDevice++)
		{
			
			LOG_DEBUG(tracked_device_type[nDevice].c_str());
			if ((tracked_device_pose[nDevice].bDeviceIsConnected) && (tracked_device_pose[nDevice].bPoseIsValid))
			{
				float v[3] = { tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[0][3], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[1][3], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[2][3] };
				float r1[3] = { tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[0][0], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[0][1], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[0][2] };
				float r2[3] = { tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[1][0], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[1][1], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[1][2] };
				float r3[3] = { tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[2][0], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[2][1], tracked_device_pose[nDevice].mDeviceToAbsoluteTracking.m[2][2] };

				matrix->Identity();
				matrix->Element[0][0] = r1[0];
				matrix->Element[0][1] = r1[1];
				matrix->Element[0][2] = r1[2];
				matrix->Element[1][0] = r2[0];
				matrix->Element[1][1] = r2[1];
				matrix->Element[1][2] = r2[2];
				matrix->Element[2][0] = r3[0];
				matrix->Element[2][1] = r3[1];
				matrix->Element[2][2] = r3[2];
				matrix->Element[0][3] = v[0] * 1000;
				matrix->Element[1][3] = v[1] * 1000;
				matrix->Element[2][3] = v[2] * 1000;

				double unfilteredTimestamp = 0.0;

				if(strcmp(tracked_device_type[nDevice].c_str(), "hmd") == 0 && HMD_registered)
				{
					unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
					this->ToolTimeStampedUpdate(this->HMD_DS->GetSourceId(), matrix, ToolStatus(TOOL_OK), this->FrameNumber, unfilteredTimestamp, NULL);
				}
				else if (strcmp(tracked_device_type[nDevice].c_str(), "controller") == 0 && controller_registered) 
				{
					unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
					this->ToolTimeStampedUpdate(this->Controller_DS->GetSourceId(), matrix, ToolStatus(TOOL_OK), this->FrameNumber, unfilteredTimestamp, NULL);
				}
				else if (strcmp(tracked_device_type[nDevice].c_str(), "generic tracker") == 0 && generic_tracker_registered)
				{
					unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();
					this->ToolTimeStampedUpdate(this->GenericTracker_DS->GetSourceId(), matrix, ToolStatus(TOOL_OK), this->FrameNumber, unfilteredTimestamp, NULL);
				}
			}
			
		}
		this->FrameNumber++;
		return PLUS_SUCCESS;
	}
	else
	{
		LOG_ERROR("Could not initialize SteamVR!")
		return PLUS_FAIL;
	}
}

PlusStatus vtkPlusSteamVR::NotifyConfigured() {
	if(HMD_DS != NULL)
	{
		LOG_INFO("HMD registered!")
	}
	if(Controller_DS != NULL && controller_registered)
	{
		LOG_INFO("Controller registered")
	}
	if(GenericTracker_DS != NULL && generic_tracker_registered)
	{
		LOG_INFO("Generic tracker registered");
	}
	return PLUS_SUCCESS;
}

PlusStatus vtkPlusSteamVR::ReadConfiguration(vtkXMLDataElement*  rootConfigElement ) {
	  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

	XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
	for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
	{
		vtkXMLDataElement* toolDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
		if (STRCASECMP(toolDataElement->GetName(), "DataSource") == 0)
		{
			if(toolDataElement->GetAttribute("TrackingToolDeviceType") != NULL && STRCASECMP(toolDataElement->GetAttribute("TrackingToolDeviceType"), "HMD") == 0)
			{
				vtkPlusDataSource *HMD_DS = NULL;
				if(this->GetToolByPortName("HMD", HMD_DS) == PLUS_SUCCESS)
				{
					LOG_INFO("Registered HMD");
					this->HMD_registered = true;
					this->HMD_DS = HMD_DS;
				}
				else
				{
					LOG_INFO("Failed to add HMD DataSource. Be sure to add \"PortName=\"HMD\"\" to your configuration file");
				}
			}
			else if(toolDataElement->GetAttribute("TrackingToolDeviceType") != NULL && STRCASECMP(toolDataElement->GetAttribute("TrackingToolDeviceType"), "Controller") == 0)
			{
				vtkPlusDataSource *Controller_DS = NULL;
				if(this->GetToolByPortName("Controller1", Controller_DS) == PLUS_SUCCESS)
				{
					LOG_INFO("Registered Controller");
					this->number_controller_registered++;
					this->Controller_DS = Controller_DS;
				}
				else
				{
					LOG_INFO("Failed to add HMD DataSource. Be sure to add \"PortName=\"Controller#Number\"\" to your configuration file");
				}
			}
			else if(toolDataElement->GetAttribute("TrackingToolDeviceType") != NULL && STRCASECMP(toolDataElement->GetAttribute("TrackingToolDeviceType"), "GenericTracker") == 0)
			{
				vtkPlusDataSource *GenericTracker_DS = NULL;
				if(this->GetToolByPortName("GenericTracker", GenericTracker_DS) == PLUS_SUCCESS)
				{
					LOG_INFO("Registered GenericTracker");
					this->generic_tracker_registered = true;
					this->GenericTracker_DS = GenericTracker_DS;
				}
				else
				{
					LOG_INFO("Failed to add GenericTracker DataSource. Be sure to add \"PortName=\"GenericTracker\"\" to your configuration file");
				}
			}
		}
		else
		{
			LOG_ERROR("Configuration file does not specify data sources!")
		}
		
	} 
	return PLUS_SUCCESS;
}

PlusStatus vtkPlusSteamVR::WriteConfiguration(vtkXMLDataElement* rootConfigElement) {
	return PLUS_SUCCESS;
}

PlusStatus vtkPlusSteamVR::InternalStartRecording() {
	//Not implemented yet
	return PLUS_SUCCESS;
}

PlusStatus vtkPlusSteamVR::InternalStopRecording() {
	//Not implemented yet
	return PLUS_SUCCESS;
}


