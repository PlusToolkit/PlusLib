/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/



#ifndef __vtkPlusSteamVR_h
#define __vtkPlusSteamVR_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

#include "openvr.h"
#include "vtkPlusSteamVRUtility.h"
//#include "vtkPlusUtil.h"
#include <string>
#include <list>

class vtkPlusDataCollectionExport vtkPlusSteamVR : public vtkPlusDevice {

public:

	static vtkPlusSteamVR *New();
	vtkTypeMacro(vtkPlusSteamVR, vtkPlusDevice);
	void PrintSelf(ostream& os, vtkIndent indent);

	/*! Read configuration from xml data */
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* rootConfigElement);

	/*! Write configuration to xml data*/
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

	/*! Called after all devices have been configured / inputs & outputs are connected / collection of data not started*/
	virtual PlusStatus NotifyConfigured();

	
	virtual bool IsTracker() const { return true; }

	virtual bool IsShotoDevice() const { return true; }

	/*! Connect to device*/
	PlusStatus InternalConnect();

	/*! Disconnect from device*/
	virtual PlusStatus InternalDisconnect();

	PlusStatus InternalUpdate();

	PlusStatus Probe();

protected:
	vtkPlusSteamVR();
	~vtkPlusSteamVR();

	vtkPlusDataSource *HMD_DS = NULL;
	vtkPlusDataSource *GenericTracker_DS = NULL;
	vtkPlusDataSource *Controller_DS = NULL;

private:
	vtkPlusSteamVR(const vtkPlusSteamVR&);
	void operator=(const vtkPlusSteamVR&);

	/*! Start the tracking system.*/
	PlusStatus InternalStartRecording();


	/*! Stop the tracking system and bring it back to its initial state.*/
	PlusStatus InternalStopRecording();


	vr::IVRSystem* vr_context;
	vr::IVRChaperone* vr_chaperone;
	vr::IVROverlay* overlay;
	vr::VROverlayHandle_t handle;
	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];

	std::string tracked_device_type[vr::k_unMaxTrackedDeviceCount];


	bool HMD_registered = false;
	int number_controller_registered = 0;
	bool generic_tracker_registered = false;
	bool controller_registered = false;
	
	double steamvr_connection_timeout = 10.0;

	void process_vr_event(const vr::VREvent_t &event);
};

#endif
