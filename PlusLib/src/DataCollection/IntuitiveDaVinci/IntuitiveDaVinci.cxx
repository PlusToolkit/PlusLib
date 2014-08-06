#include "PlusConfigure.h"
#include "IntuitiveDaVinci.h"

IntuitiveDaVinci::IntuitiveDaVinci() : mPrintStream(ISI_FALSE), mQuit(ISI_FALSE), mManipIndex(ISI_PSM2), mStatus(ISI_SUCCESS)
{
	mIpAddr = "10.0.0.5";
	mPort = 5002;
	mPassword = "";
}

IntuitiveDaVinci::~IntuitiveDaVinci()
{
	stop();
}

void IntuitiveDaVinci::stop()
{
	if(isConnected())
	{
		isi_stop_stream();
		isi_disconnect();
		mConnected = false;
	}
}

ISI_STATUS IntuitiveDaVinci::connect()
{
	ISI_UINT password = strtol(mPassword.c_str(), 0, 16);
	mStatus = isi_connect_ex(mIpAddr.c_str(), mPort, password);
	
	if (mStatus != ISI_SUCCESS)
	{
	    LOG_WARNING("IntuitiveDaVinci::connect failed to connect with arguments");
		isi_stop_stream();
		isi_disconnect();
	}
	mConnected = true;
	return mStatus;
}

ISI_STATUS IntuitiveDaVinci::subscribe(ISI_EVENT_CALLBACK eCB, ISI_STREAM_CALLBACK sCB, void* eventUserData, void* streamUserData)
{
	setStreamCallback(sCB, streamUserData);
	setEventCallback(eCB, eventUserData);

	// Start streaming information at 60 Hz
	mStatus = isi_start_stream(ISI_API_RATE);

	if (mStatus != ISI_SUCCESS)
	{
		LOG_WARNING("IntuitiveDaVinci::subscribe failed to start stream. Disconnecting from da Vinci");
		stop();
	}
	return mStatus;
}

ISI_STATUS IntuitiveDaVinci::setStreamCallback(ISI_STREAM_CALLBACK sCB, void* userData)
{
	// Subscribe to all fields
	mStatus = isi_subscribe_all_stream_fields();
	if(mStatus != ISI_SUCCESS)
	{
		// Propagate error message to caller
		return mStatus;
	}

	if(sCB != NULL)
	{
		mStatus = isi_set_stream_callback(sCB, userData);
	}
	Sleep(500);

	return mStatus;
}

ISI_STATUS IntuitiveDaVinci::setEventCallback(ISI_EVENT_CALLBACK eCB, void* userData)
{
	// Subscribe to all events
	mStatus = isi_subscribe_all_events();

    if(mStatus != ISI_SUCCESS)
	{
		// Propagate error message to caller
		return mStatus;
	}

	if(eCB != NULL)
	{
		mStatus = isi_set_event_callback(eCB, userData);  
	}
	Sleep(500);

	return mStatus;
}

std::vector<std::string> IntuitiveDaVinci::getManipulatorNames()
{
	std::vector<std::string> names;

	for(int i = 0; i != ISI_NUM_MANIPS; i++)
	{
		ISI_MANIP_INDEX index = (ISI_MANIP_INDEX) i;
		std::string name = isi_get_manip_name(index);

		if(name.size() == 0) continue;

		names.push_back(name);
	}

	return names;
}

void IntuitiveDaVinci::getPosition(ISI_TRANSFORM* T)
{
	ISI_STREAM_FIELD stream_data;	// Datastreams

	isi_get_stream_field(mManipIndex, ISI_TIP_TRANSFORM, &stream_data);

	copyTransform((ISI_TRANSFORM *) stream_data.data, T);
}

void IntuitiveDaVinci::copyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out)
{
	if( in == NULL || out == NULL) return;

	out->pos.x = in->pos.x;
	out->pos.y = in->pos.y;
	out->pos.z = in->pos.z;

	out->rot.row0 = in->rot.row0;
	out->rot.row1 = in->rot.row1;
	out->rot.row2 = in->rot.row2;

}

void IntuitiveDaVinci::printTransform(const ISI_TRANSFORM *T)
{
  LOG_INFO("Position: " << T->pos.x << " " << T->pos.y << " "  << T->pos.z);

  LOG_INFO("X Axis Rotation: " << T->rot.row0.x << " " << T->rot.row1.x << " "  << T->rot.row2.x);

  LOG_INFO("Y Axis Rotation: " << T->rot.row0.y << " " << T->rot.row1.y << " "  << T->rot.row2.y);

  LOG_INFO("Z Axis Rotation: " << T->rot.row0.z << " " << T->rot.row1.z << " " << T->rot.row2.z);
}

void IntuitiveDaVinci::printHelp()
{
	static const char help[] = {"\
								API Stream Menu \n\
								---------------- \n\
								Q: Quit          \n\
								M: Set Manip ID  \n\
								V: Print Version \n\
								P: Print Sample  \n\
								S: Print Stream  \n\
								R: Record file   \n\
								"};

	LOG_INFO(help);    
}

std::string IntuitiveDaVinci::getLibraryVersion()
{
	ISI_STATUS status;
	ISI_SYSTEM_CONFIG config;    

	status = isi_get_system_config(&config);

	if(status != ISI_SUCCESS)
	{
		return "";
	}

	return config.library_version;
}

bool IntuitiveDaVinci::isConnected()
{
	return mConnected;
}

void IntuitiveDaVinci::printStreamState(ISI_MANIP_INDEX manipIndex)
{
	ISI_STREAM_FIELD stream_data;
	int i;

	LOG_INFO("Retrieving stream state for manipulator: " << isi_get_manip_name(manipIndex));
	
	LOG_INFO("\nISI_TIP_TRANSFORM: \n");
	isi_get_stream_field(manipIndex, ISI_TIP_TRANSFORM, &stream_data);    
	printTransform((ISI_TRANSFORM *) stream_data.data);
	
	LOG_INFO("\nISI_JOINT_VALUES: \n");
	
	isi_get_stream_field(manipIndex, ISI_JOINT_VALUES, &stream_data);    
	for (i = 0; i != stream_data.count; i++)
	{
	    LOG_INFO(" " << stream_data.data[i]);
	}

	LOG_INFO("\n ISI Timestamp (seconds): " <<  stream_data.timestamp);
}

void IntuitiveDaVinci::printVersion()
{
	ISI_STATUS status;
	ISI_SYSTEM_CONFIG config;    

	status = isi_get_system_config(&config);

	if (status == ISI_SUCCESS)
	{
       LOG_INFO("System Name: " << config.system_name);
	   LOG_INFO("System Version: " << config.system_version);
	   LOG_INFO("Library Version: " << config.library_version);
	}
	else
	{
		LOG_WARNING("Failed to get IntuitiveDaVinci system configuration");
	}
}

void IntuitiveDaVinci::setHostInfo(const std::string ip, const unsigned int port, const std::string pass)
{
  mIpAddr = ip;
  mPort = port;
  mPassword = pass;
}

void IntuitiveDaVinci::setIpAddr(const std::string ip)
{
  mIpAddr = ip;
}
	
void IntuitiveDaVinci::setPort(const unsigned int port)
{
  mPort = port;
}
	
void IntuitiveDaVinci::setPassword(const std::string password)
{
  mPassword = password;
}
