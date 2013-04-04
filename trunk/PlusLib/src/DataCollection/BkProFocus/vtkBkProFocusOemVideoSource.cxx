/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "epiphan/frmgrab.h"
#include "vtkBkProFocusOemVideoSource.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"
#include "vtksys/SystemTools.hxx"

vtkCxxRevisionMacro(vtkBkProFocusOemVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkBkProFocusOemVideoSource);

//----------------------------------------------------------------------------
vtkBkProFocusOemVideoSource::vtkBkProFocusOemVideoSource()
{
  this->VideoFormat = VIDEO_FORMAT_Y8; 
  this->ClipRectangleOrigin[0]=0;
  this->ClipRectangleOrigin[1]=0;
  this->ClipRectangleSize[0]=0;
  this->ClipRectangleSize[1]=0;
  this->GrabberLocation = NULL;

  this->RequireImageOrientationInConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkBkProFocusOemVideoSource::~vtkBkProFocusOemVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  if ( this->FrameGrabber != NULL) 
  {
    FrmGrab_Deinit();
    this->FrameGrabber = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkBkProFocusOemVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalConnect()
{

  LOG_TRACE( "vtkBkProFocusOemVideoSource::InternalConnect" );

  // Initialize frmgrab library
  FrmGrabNet_Init();

  if ( this->GrabberLocation != NULL )
  {
    if ( (this->FrameGrabber = FrmGrab_Open( this->GrabberLocation )) == NULL ) 
    {      
      if ( (this->FrameGrabber = FrmGrabLocal_Open()) == NULL )
      {
        LOG_ERROR("Epiphan Device found");
        return PLUS_FAIL;
      }
      const char UNKNOWN_DEVICE[]="UNKNOWN";
      const char* connectedTo=FrmGrab_GetLocation((FrmGrabber*)this->FrameGrabber);      
      if (connectedTo==NULL)
      {
        connectedTo=UNKNOWN_DEVICE;
      }

      LOG_WARNING("Epiphan Device with the requested location '"<<this->GrabberLocation<<"' not found. Connected to " << connectedTo << " device instead.");
    }
  }
  else
  {
    LOG_DEBUG("Serial Number not specified. Looking for any available device");
    if ( (this->FrameGrabber = FrmGrabLocal_Open()) == NULL )
    {
      LOG_ERROR("Epiphan Device Not found");
      return PLUS_FAIL;
    }
  }

  V2U_VideoMode vm;
  if (!FrmGrab_DetectVideoMode((FrmGrabber*)this->FrameGrabber,&vm)) 
  {
    LOG_ERROR("No signal detected");
    return PLUS_FAIL;
  }

  double maxPossibleAcquisitionRate=vm.vfreq/1000;
  if (this->GetAcquisitionRate()>maxPossibleAcquisitionRate)
  {
    this->SetAcquisitionRate(maxPossibleAcquisitionRate);
  }
  if (vm.width==0 || vm.height==0)
  {
    LOG_ERROR("No valid signal detected. Invalid frame size is received from the framegrabber: "<<vm.width<<"x"<<vm.height);
    return PLUS_FAIL;
  }
  this->FrameSize[0] = vm.width;
  this->FrameSize[1] = vm.height;  

  if( (this->ClipRectangleSize[0] > 0) && (this->ClipRectangleSize[1] > 0) )
  {
    if (this->ClipRectangleSize[0]%4!=0)
    {
      LOG_WARNING("ClipRectangleSize[0] is not a multiple of 4. Acquired image may be skewed.");
    }
    this->FrameSize[0] = this->ClipRectangleSize[0];
    this->FrameSize[1] = this->ClipRectangleSize[1];
  }

  vtkPlusDataSource* aSource(NULL);
  for( ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it )
  {
    if( (*it)->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*it)->GetChannelId());
      return PLUS_FAIL;
    }
    else
    {
      aSource->GetBuffer()->SetPixelType(itk::ImageIOBase::UCHAR);
      aSource->GetBuffer()->SetFrameSize(this->FrameSize);
    }
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalDisconnect()
{

  LOG_DEBUG("Disconnect from Epiphan ");

  //this->Initialized = 0;
  if (this->FrameGrabber != NULL) {
    FrmGrab_Close((FrmGrabber*)this->FrameGrabber);
  }
  this->FrameGrabber = NULL;

  return this->StopRecording();

  return PLUS_SUCCESS;

}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalStartRecording()
{
  if (!this->Recording)
  {
    return PLUS_SUCCESS;
  }
  FrmGrab_Start((FrmGrabber*)this->FrameGrabber); 

  return this->Connect();
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalStopRecording()
{
  FrmGrab_Stop((FrmGrabber*)this->FrameGrabber);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::InternalUpdate()
{

  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  V2U_GrabFrame2 * frame = NULL;

  V2U_UINT32 videoFormat=V2U_GRABFRAME_FORMAT_Y8;
  switch (this->VideoFormat)
  {
  case VIDEO_FORMAT_Y8: videoFormat=V2U_GRABFRAME_FORMAT_Y8; break;
  case VIDEO_FORMAT_RGB8: videoFormat=V2U_GRABFRAME_FORMAT_RGB8; break;
  default:
    LOG_ERROR("Unknown video format: "<<this->VideoFormat);
    return PLUS_FAIL;
  }

  V2URect *cropRect=NULL;
  if (this->ClipRectangleSize[0]>0 && this->ClipRectangleSize[1]>0)
  {
    cropRect=new V2URect;
    cropRect->x = this->ClipRectangleOrigin[0];
    cropRect->y = this->ClipRectangleOrigin[1];
    cropRect->width = this->ClipRectangleSize[0];
    cropRect->height = this->ClipRectangleSize[1];
  }

  frame = FrmGrab_Frame((FrmGrabber*)this->FrameGrabber, videoFormat, cropRect);

  delete cropRect;
  cropRect = NULL;

  if (frame == NULL)
  {
    LOG_WARNING("Frame not captured");
    return PLUS_FAIL;
  }

  this->FrameNumber++; 

  vtkPlusDataSource* aSource(NULL);
  for( ChannelContainerIterator it = this->OutputChannels.begin(); it != this->OutputChannels.end(); ++it )
  {
    if( (*it)->GetVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the Epiphan device on channel " << (*it)->GetChannelId());
      return PLUS_FAIL;
    }
    else
    {
      if( aSource->GetBuffer()->AddItem(frame->pixbuf , (*it)->GetImageOrientation(), FrameSize, itk::ImageIOBase::UCHAR,US_IMG_BRIGHTNESS, 0, this->FrameNumber) != PLUS_SUCCESS )
      {
        LOG_ERROR("Error adding item to video source " << aSource->GetSourceId() << " on channel " << (*it)->GetChannelId() );
        return PLUS_FAIL;
      }
      else
      {
        this->Modified();
      }
    }
  }
  FrmGrab_Release((FrmGrabber*)this->FrameGrabber, frame);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{

  LOG_TRACE("vtkBkProFocusOemVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Epiphan video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  const char* videoFormat = imageAcquisitionConfig->GetAttribute("VideoFormat"); 
  if ( videoFormat != NULL) 
  {
    if( !strcmp(videoFormat,"RGB8") )
    {
      this->SetVideoFormat( VIDEO_FORMAT_RGB8 );
    }
    else if ( !strcmp(videoFormat,"Y8") )
    {
      this->SetVideoFormat( VIDEO_FORMAT_Y8 );
    }
    else 
    {
      LOG_WARNING("Video Format unspecified/not supported. Using Y8"); 
      this->SetVideoFormat( VIDEO_FORMAT_Y8 );
    }
  }

  // SerialNumber is kept for backward compatibility only. Serial number or other address should be specified in the
  // GrabberLocation attribute.
  const char* grabberLocation = imageAcquisitionConfig->GetAttribute("GrabberLocation");
  const char* serialNumber = imageAcquisitionConfig->GetAttribute("SerialNumber");
  if (grabberLocation!=NULL)
  {
    SetGrabberLocation(grabberLocation);
  }
  else if( serialNumber !=NULL)
  {
    std::string grabberLocationString=std::string("sn:")+serialNumber;
    SetGrabberLocation(grabberLocationString.c_str());
    LOG_WARNING("Epiphan SerialNumber is specified. This attribute is deprecated, please use GrabberLocation=\"sn:SERIAL\" attribute instead.");
  }
  else
  {
    LOG_DEBUG("Epiphan device location is not specified in the configuration");
  }

  // clipping parameters
  int clipRectangleOrigin[2]={0,0};
  if (imageAcquisitionConfig->GetVectorAttribute("ClipRectangleOrigin", 2, clipRectangleOrigin))
  {
    this->SetClipRectangleOrigin(clipRectangleOrigin);
  }
  int clipRectangleSize[2]={0,0};
  if (imageAcquisitionConfig->GetVectorAttribute("ClipRectangleSize", 2, clipRectangleSize))
  {
    this->SetClipRectangleSize(clipRectangleSize);
  }

  return PLUS_SUCCESS;

}

//-----------------------------------------------------------------------------
PlusStatus vtkBkProFocusOemVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  if ( this->VideoFormat == VIDEO_FORMAT_RGB8 )
  {
    imageAcquisitionConfig->SetAttribute("VideoFormat", "RGB8");
  }
  else if ( this->VideoFormat == VIDEO_FORMAT_Y8 )
  {
    imageAcquisitionConfig->SetAttribute("VideoFormat", "Y8");
  }
  else
  {
    LOG_ERROR("Attempted to write invalid video format into the config file: "<<this->VideoFormat<<". Written Y8 instead.");
    imageAcquisitionConfig->SetAttribute("VideoFormat", "Y8");
  }

  if (this->GrabberLocation!=NULL)
  {
    imageAcquisitionConfig->SetAttribute("GrabberLocation", this->GrabberLocation);
  }
  else
  {
    imageAcquisitionConfig->RemoveAttribute("GrabberLocation");
  }

  // SerialNumber is an obsolete attribute, the information is stored onw in GrabberLocation
  imageAcquisitionConfig->RemoveAttribute("SerialNumber");

  // clipping parameters
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleOrigin", 2, this->GetClipRectangleOrigin());
  imageAcquisitionConfig->SetVectorAttribute("ClipRectangleSize", 2, this->GetClipRectangleSize());

  return PLUS_SUCCESS;
}




#if 0

// TestParametersLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <ostream>
#include <string>
#include "stdint.h"

#include "ParamConnectionSettings.h"
#include "ParamSyncConnection.h"
#include "TCPClient.h"
#include "WSAIF.h"
#include "OemParams.h"

#include "UseCaseParser.h"
#include "UseCaseStructs.h"




using namespace std;

#ifdef _MSC_VER
#pragma warning (disable: 4996; push)    // Begging for safe versions of strcpy, strcmp etc.
#endif



/// <summary> Global structure with parameters for the connection to the OEM and Toolbox interfaces </summary>
ParamConnectionSettings params;




/**
 * \brief Example of how to establish connection to scanner.
 *        This example creates two tcp clisnets - one for the research interface and one for the Toolbox.
 *        Addresses and ports are obtained from the global parameters structure.
 *        A ParamSyncConnection is created, which uses the two clients.
 *
 *  \note The connection is closed at the end of the function. 
 *
 *  \returns 0 upon success, -1 upon failure
 */

int ConnectToScanner()
{
	
	WSAIF wsaif;
	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);

	bool succToolbox = paramSync.ConnectToolboxCommandInterface();
	bool succOem = paramSync.ConnectOEMInterface();

	cout << "Connection to Toolbox Command Interface  " << ((succToolbox)? "SUCCESS":"FAIL")  << endl;
	cout << "Connection to OEM Interface  " << ((succOem)? "SUCCESS":"FAIL") << endl;
	 
	return ((succToolbox && succOem)?0:-1);
}




/**
 *  \brief - Gets a use case from the scanner and prints in the terminal window.
 * 
 *   \returns 0 upon success, -1 upon failure
 */

int GetUseCaseFromToolbox()
{
	WSAIF wsaif;
	
	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);

	
	bool success = paramSync.ConnectToolboxCommandInterface();

	if (!success)
	{
		cout << " Failed to connect to Toolbox " << endl;
		return -1;
	}else{
		cout << "Connected to toolbox" << endl;
	}


	
	char* useCase; 
	success = paramSync.GetUseCase(&useCase);

	if (!success)
	{
		cout << "Failed to acquire use case" << endl;
		return -1;
	}

	cout << "___________________________________________"<<endl;
	cout << useCase << endl;
	cout << "___________________________________________"<<endl;
	return 0;
}


 


 /** 
  * Get all OEM parameters
  */
struct OemDefinition{
	char* name;
	oemTypeID type;
	bool needsView;
};




int TestCreattionOfOemParameters()
{

	static char* data_str[] = {
		"DATA:LANGUAGE ENGLISH;",
		"DATA:IMAGE_MODE:A 1;",
		"DATA:TRANSDUCER:A \"C\",\"8811\";",
		"DATA:TRANSDUCER_LIST \"8662\",\"C\",\"\",\"\",\"8811\",\"L\",\"\",\"\";",
		"DATA:B_SCANLINES_COUNT:A 269;",
		"DATA:B_GEOMETRY_SCANAREA:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;",
		"DATA:B_SPLIT:A \"A\";",
		"DATA:B_SIMULTANEOUS_SPLIT: \"OFF\";",
		"DATA:B_GAIN:A 70;", 
		"DATA:B_GAIN_DB:A 72;", 
		"DATA:B_DYN_RANGE:A 60;",
		"DATA:B_MFI:A 9.0e6;",
		"DATA:B_MFI_REAL:A 8.73e6;", 
		"DATA:B_GEOMETRY_TISSUE:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;",
		"DATA:B_FRAMERATE:A 12.5;",
		"DATA:B_HARMONIC_ACTIVATED:A OFF;", 
		"DATA:B_HARMONIC_MODEA: OFF;", 
		"DATA:B_BUBBLE_BURST:A OFF;",
		"DATA:B_EXTENDED_RESOLUTION:A ON;", 
		"DATA:B_MULTI_BEAM:A 4;",
		"DATA:B_SAMPLE_FREQUENCY:A 33.2e6;",
		"DATA:B_RF_LINE_LENGTH:A 3352;",
		"DATA:B_EXPANDED_SECTOR:A OFF;", 
		"DATA:B_COMPOUND:A OFF;", 
		"DATA:B_SCANLINES_COUNT:A 253;", 
	};

	int numVars = sizeof(data_str) / sizeof(char*);
	oemArray* var = new oemArray [numVars];


	
	for (int n = 0; n < numVars; ++n)
	{
		static char localdatastring[256];   // We need a copy of the data string, because the function 
		                                    //fill_oem_array_from_data_str writes to the data string
		static char outdatastring[256];

		strncpy_s(localdatastring, sizeof(localdatastring), data_str[n], strlen(data_str[n]) );
		fill_oem_array_from_data_str(&var[n], localdatastring, strlen(localdatastring));
		var[n].fillDataString(outdatastring, sizeof(outdatastring));

		cout << " In : " << data_str[n] << endl;
		cout << "Out : " << outdatastring << endl;
	}

	delete [] var;
	return 0;
}




/**
 */

int CaptureJPG()
{
	WSAIF wsaif;

	TcpClient oemClient(&wsaif, 8*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 8*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);


	bool connected = paramSync.ConnectOEMInterface();

	if (!connected)
	{
		cout << "Could not connect to OEM interface " << endl;
	}


	oemArray jpg;

	char query[] = "query:capture_image \"BMP\";";
	cout << "query = " << query << endl;

	
	oemClient.Write(query, strlen(query));
	


	char buf[8*1024*1024];
	memset(buf, 0, sizeof(buf));
	std::cout << "Before client read" << std::endl;
	size_t numBytes = oemClient.Read(&buf[0],8*1024*1024);
	std::cout << numBytes << " read!" << std::endl;

	// First detect the #
	int n=0, k=0;
	for ( n = 0; buf[n]!='#' && n < numBytes; n++ ) ;
	n++;

	int numChars = (int)buf[n] - (int)('0');
	n++;
	std::cout << "numChars = " << numChars << std::endl;
	int jpgSize = 0;
	for ( k = 0; k < numChars; k++, n++) 
	{
		jpgSize = jpgSize * 10 + ((int)buf[n] - '0');
	}
	std::cout << "jpgSize = " << jpgSize << std::endl;

	FILE * f;
	f = fopen("c:\\andrey\\bktest.bmp", "wb");
	if(f!=NULL){
	  fwrite(&buf[n],1,jpgSize,f);
	  fclose(f);
	}

	return 0;
}

/**
 */

int CaptureJPGFrameRate()
{
	WSAIF wsaif;

	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);

	TcpClient oemClient1(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());

	bool connected = paramSync.ConnectOEMInterface();

	if (!connected)
	{
		cout << "Could not connect to OEM interface " << endl;
	}


	oemArray jpg;

	

	char query[] = "query:capture_image \"JPG\";";
	cout << "query = " << query << endl;
	
	//double startTime;//= timeGetTime()*1000.;
	//QueryPerformanceTimer(startTime);

	UINT64 startCount, endCount, diffCount, freq;
    QueryPerformanceCounter((LARGE_INTEGER*)&startCount);

	std::string nframesStr;
	
	std::cout << "Enter number of frames to capture: ";
	std::getline(cin, nframesStr);
	std::getline(cin, nframesStr);
	double nframes = atoi(nframesStr.c_str());
	std::cout << "Will capture " << nframes << " frames" << std::endl;

	for(int i=0;i<nframes;i++)
	{
		oemClient.Write(query, strlen(query));
	


		char buf[4*1024*1024];
		memset(buf, 0, sizeof(buf));
		//std::cout << "Before client read" << std::endl;
		size_t numBytes = oemClient.Read(&buf[0],2*1024*1024);
		//std::cout << numBytes << " read!" << std::endl;

		// First detect the #
		int n=0, k=0;
		for ( n = 0; buf[n]!='#' && n < numBytes; n++ ) ;
		n++;
	
		int numChars = (int)buf[n] - (int)('0');
		n++;
		//std::cout << "numChars = " << numChars << std::endl;
		int jpgSize = 0;
		for ( k = 0; k < numChars; k++, n++) 
		{
			jpgSize = jpgSize * 10 + ((int)buf[n] - '0');
		}
		//std::cout << "jpgSize = " << jpgSize << std::endl;
		
		/*
		FILE * f;
		f = fopen("c:\\andrey\\bktest.jpg", "wb");
		if(f!=NULL){
			fwrite(&buf[n],1,jpgSize,f);
			fclose(f);
		}
		*/
	}
	//double endTime = timeGetTime()*1000.;
	//double fps = 1000./(endTime-startTime);
	//double tpf = (endTime-startTime)/1000.; 
    QueryPerformanceCounter((LARGE_INTEGER*)&endCount);

    diffCount = endCount - startCount;

    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
 
    double exeTime_in_ms = (double)diffCount * 1000.0 / freq;
	std::cout << "Frames captured: " << nframes << std::endl;
	std::cout << "Frames per second: " << nframes/exeTime_in_ms*1000. << std::endl;
	std::cout << "Time per frame (ms): " << exeTime_in_ms/nframes << std::endl;

	//std::cout << "fps = " << fps << ", " << " time per frame = " << tpf << " sec" << std::endl;

	return 0;
}


/**
 * \brief Example of how to read Oem Parameters 
 * 
 * This example shows
 *     \li how to connect to a scanner 
 *     \li how to create a oemArray object
 *     \li how to get its value using the class ParamSyncConnection
 */
int GetOemParameters()
{

	static OemDefinition oemParamsTable [] = {
	{"IMAGE_MODE", oemINT,  true},
	{"TRANSDUCER", oemCHAR, true},
	{"TRANSDUCER_LIST", oemCHAR, false},
	{"B_SCANLINES_COUNT", oemINT, false},
	{NULL, oemUNKNOWN , false}
    };


	WSAIF wsaif;

	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);


	bool connected = paramSync.ConnectOEMInterface();

	if (!connected)
	{
		cout << " Failed to connect to OEM interface " << endl;
		return -1;
	}

	paramSync.SendOemQuery("ALIVE:REQUEST;", true);
	
	int numOemCommands = sizeof(oemParamsTable)/sizeof(OemDefinition) - 1;
	
	oemArray *var;
	static char datastring[2048];

	for (int n = 0; n < numOemCommands; n++)
	{
		var = new oemArray(oemParamsTable[n].name, oemParamsTable[n].type);
		
		if (oemParamsTable[n].needsView)
		{
			var->setView('A');
		}

		paramSync.GetSingleOemParamVal(var);
		cout << "var [" << var->oem_name << "] is of size (" << var->M << " x " << var->N << ")" << endl;

		memset( datastring, 0, sizeof(datastring));
		var->fillDataString(datastring, sizeof(datastring));

		delete var;

		cout << datastring << endl;
	}


	return 0;
}




/**
 * \brief Example of sending OEM queries to the scanner
 *
 * In this function, a TcpClient object is created. Then this object is used to send queries
 * and receive the replies from the scanner.
 *
 */

int SendReceiveOemCommands()
{
	WSAIF wsaif;
	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	bool connected = oemClient.Connect();

	

	if (!connected)
	{
		return -1;
	}

	oemClient.Start();    // Start the client

	static char query[ 2048 ];
	static char reply[2*1024*1024];

	memset(query,0, sizeof(query));
	memset(reply,0,sizeof(reply));


	bool quit = false;

	while(!quit)
	{
		cout << "query >> ";
		std::gets(query);
		//if(!strlen(query))
		//	continue;
		quit = !_strnicmp(query,"quit", 4);

		if (!quit)
		{
			memset(reply, 0, sizeof(reply));
			oemClient.Write(query, strlen(query));
			//cout << "DEBUG: will write this query to client: " << std::endl << query << std::endl;
			oemClient.Read(reply,sizeof(reply));
			cout << "reply  ] " << reply << endl;
		}
		else
		{
			cout << endl << "Quitting. May take up to 2 seconds" << endl;
		}
	}


	oemClient.Stop();
	return 0;
}





/**
 *\brief Example of how to manipulate oemArray objects:
 *    \li Creation
 *    \li Initialization
 *    \li Initialization from OEM Interface data strings
 *    \li Typecasting 
 *    \li Comparison of arrays
 */

int ManipulateOemArrayVars()
{
	/* Scalars and type casting */
	oemArray a('A');
	cout << " Variable 'a' is char, and has a value of '" << a.data.vChar << "'" << endl;
	cout << " The ASCII code of 'a' is " << (int) a << endl;
	cout << " The double representation of 'a' is " << (double) a << endl;
	cout << endl;

	if ( 65 != (int) a)
	{
		cout << "Error in type casting operation " << endl;
		return -1;
	}

	// ---
	oemArray b(66);
	cout << " Variable 'b' is an integer and has a value of " << b.data.vInt << endl;
	cout << " It can be type cast to (double) : " << (double) b << endl;
	cout << " It can be even type cast to (char) :" << (char) b << endl;
	cout << endl;

	if (66.0 != (double) b)
	{
		cout << " Error type casting int to double  " << endl;
		return -1;
	}


	oemArray c(67.5);
	cout << " Variable 'c' is a double precision number :" << c.data.vDouble << endl;
	cout << " It can be type cast to integer  :" << (int)c << endl;
	cout << " It can be type cast to (char) :" << (char) c << endl;
	cout << endl;

	if ('C' != (char)c )
	{
		cout << "Error typecasting double to char " << endl;
		return -1;
	}

	/// Test the function fill_oem_array_from_data_str
	const char origstring[] = "DATA:RAND_VALUES:A 1.0, 2.1, 3.2;";
	char datastring[sizeof(origstring)];   
	memcpy(datastring, origstring, sizeof(origstring));   // Copies also the trailing '\0'
	fill_oem_array_from_data_str(&a , datastring, strlen(datastring));   // datastring is modified

	if (a.type != oemDOUBLE)
	{
		cout << " Error, fill_oem_array_from_data_str - Wrong type " << endl;
		return -1;
	}

	if (a.scalar != false)
	{
		cout << " Error, a.scalar is not set properly " << endl;
		return -1;
	}

	if (a.M != 3)
	{
		cout << " Error, a.M is not set properly. a.M = " << a.M << endl;
		return -1;
	}

	if (a.N != 1)
	{
		cout << "Error, a.N is not set properly. a.N = " << a.N << endl;
		return -1;
	}

	if (a.getDoubleAt(0) != 1.0 || a.getDoubleAt(1) != 2.1 || a.getDoubleAt(2) != 3.2)
	{
		cout << "The values of 'a' are not set properly " << endl;
		cout << "They should be [1.0, 2.1, 3.2]"<< endl;
		cout << "They are in reality [" << a.getDoubleAt(0) << "," << a.getDoubleAt(1) << "," << a.getDoubleAt(2) << "]"<< endl;
		return -1;
	}


	cout << " Array is created : [ " << a.getDoubleAt(0) << ", " << a.getDoubleAt(1) << ", " << a.getDoubleAt(2) << " ]"<< endl;


	/// Test the function cmp_oem_array

	memcpy(datastring, origstring, sizeof(origstring));   // Copies also the trailing '\0'
	fill_oem_array_from_data_str(&b , datastring, strlen(datastring));   // datastring is modified

	bool a_eq_b = cmp_oem_array_eq(a, b);
	if (a_eq_b == true)
	{
		cout << "[ " << a.getDoubleAt(0) << ", " << a.getDoubleAt(1) << ", " << a.getDoubleAt(2) << " ] is equal ";
		cout << "[ " << b.getDoubleAt(0) << ", " << b.getDoubleAt(1) << ", " << b.getDoubleAt(2) << " ]" << endl;
	}
	else
	{
		cout << "Error comparing two equal arrays " << endl;
	}

	return 0;
}


/**  
 *  \brief Calculate needed buffer size
 */

int CalcSapBufSize()
{
	WSAIF wsaif;

	/* Create connections and use them in a parameter synchronization object */
	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);

	cout << "Connecting to OEM interface ... " ;

	/*  Connect to OEM interface                   */
	bool connected = paramSync.ConnectOEMInterface();

	if (!connected)
	{
		cout << " Failed to connect to OEM interface " << endl;
		return -1;
	}
	cout << "SUCCESS" << endl;

	cout << "Connecting to tcp2toolbox " << endl;
	connected = paramSync.ConnectToolboxCommandInterface();

	if (!connected)
	{
		cout << " Failed to connect to OEM interface " << endl;
		return -1;
	}
	cout << "SUCCESS" << endl;



	int numLines = 0;
	int numSamples = 0;
	static char* useCaseBuffer;

	paramSync.CalcSapBufSizeFromLatestUseCase(&numSamples, &numLines);

	cout << "Num Lines : " << numLines << endl;
	cout << "Num Samples : " << numSamples << endl;
	return 0;
}



/**
 * \brief Example and test of reading / writing oemArray variables to file
 *   \li The function creates an array of oemArray objects
 *   \li This array is initialized from data strings as returned by the OEM interface
 *   \li The array is stored to a file
 *   \li The file is read into a new array
 *   \li The two arrays are compared
 *  
 */

int ReadWriteOemArrayFile()
{

	static char* data_str[] = {
		"DATA:LANGUAGE ENGLISH;",
		"DATA:IMAGE_MODE: 1;",     // This is an invalid format. Variable will be called invalid_data_string
		"DATA:TRANSDUCER:A \"C\",\"8811\";",
		"DATA:TRANSDUCER_LIST \"8662\",\"C\",\"\",\"\",\"8811\",\"L\",\"\",\"\";",
		"DATA:B_SCANLINES_COUNT:A 269;",
		"DATA:B_GEOMETRY_SCANAREA:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;",
		"DATA:B_SPLIT:A \"A\";",
		"DATA:B_SIMULTANEOUS_SPLIT:A \"OFF\";",
		"DATA:B_GAIN:A 70;", 
		"DATA:B_GAIN_DB:A 72;", 
		"DATA:B_DYN_RANGE:A 60;"    // Forgotten comma, result - array of strings. The first one will be "60". The erst will be empty
		"DATA:B_MFI:A 9.0e6;",
		"DATA:B_MFI_REAL:A 8.73e6;", 
		"DATA:B_GEOMETRY_TISSUE:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;",
		"DATA:B_FRAMERATE:A 12.5;",
		"DATA:B_HARMONIC_ACTIVATED:A OFF;", 
		"DATA:B_HARMONIC_MODE:A: OFF;", 
		"DATA:B_BUBBLE_BURST:A OFF;",
		"DATA:B_EXTENDED_RESOLUTION:A ON;", 
		"DATA:B_MULTI_BEAM:A 4;",
		"DATA:B_SAMPLE_FREQUENCY:A 33.2e6;",
		"DATA:B_RF_LINE_LENGTH:A 3352;",
		"DATA:B_EXPANDED_SECTOR:A OFF;", 
		"DATA:B_COMPOUND:A OFF;", 
		"DATA:B_SCANLINES_COUNT:A 253;", 
	};

	int numVars = sizeof(data_str) / sizeof(char*);
	
	oemArray* varToWrite = new oemArray [numVars];
	oemArray* varToRead  = new oemArray [numVars];



	
	for (int n = 0; n < numVars; ++n)
	{
		static char localdatastring[256];   // We need a copy of the data string, because the function 
		                                    //fill_oem_array_from_data_str writes to the data string

		strncpy_s(localdatastring, sizeof(localdatastring), data_str[n], strlen(data_str[n]) );
		int retval  = fill_oem_array_from_data_str(&varToWrite[n], localdatastring, strlen(localdatastring));
		if (n == 1 && retval ==-1)
		{
			cout << "Correctly detected a bad DATA string        [ SUCCESS ]" << endl;
		}
		else if(n == 1 && retval == 0 )
		{
			cout << "Failed to detect a bad DATA string            [ ERROR ]" << endl;
		}
		else if (retval == -1)
		{
			cout << "Reported a bad DATA string where there is none [ ERROR ]" << endl;
		}

	}



	FILE* outfile;
	FILE* infile;

	/* First write the variable to a file */
	outfile = fopen("test.oem","wb");     // Work with binary mode files to be on the certain about what is read
	if (outfile == NULL)
	{
		cout << "Error could not create file " << endl;
		return -1;
	}


	for(int n = 0; n < numVars; n++)
	{
		append_oem_array_to_file(outfile, &varToWrite[ n ]);
	}
	
	fclose(outfile);


	infile = fopen("test.oem", "rb");   // Open in binary mode to be sure
	if ( infile == NULL )
	{
		cout << " Could not open file for input " << endl;
		return -1;
	}

	for(int n = 0; n < numVars; n++)
	{
		read_oem_array_from_file(&varToRead[n], infile );
	}

	fclose(infile);


	bool success = true;

	// Now compare the strings
	for (int n = 0; n < numVars; ++n)
	{
		char localstr [256];
		
		varToWrite[n].fillDataString(localstr,sizeof(localstr));
		
		// cout << "Comparing " << varToWrite[n].oem_name << " to " << varToRead[n].oem_name << endl;
		cout << "Comparing " << varToWrite[n].oem_name << endl;
		cout << "[ " << localstr << " ] to "<< endl ;
		varToRead[n].fillDataString(localstr,sizeof(localstr));
		cout << "[ " << localstr <<  " ] ";
		bool result = cmp_oem_array_eq( varToWrite[n], varToRead[n]);

		if (result == false)
		{
			cout << "___________________" << endl;
			cout << "w.type =" << varToWrite[n].type << "  r.type = " << varToRead[n].type << endl;
			cout << "w.M = " << varToWrite[n].M << "  r.M =" << varToRead[n].M << endl;
			cout << "w.N = " << varToWrite[n].N << "  r.N =" << varToRead[n].N << endl;
			
			if (varToWrite[ n ].type == oemDOUBLE)
			{
				if (varToWrite[n] .scalar)
				{
					uint64_t *pW = (uint64_t*) &varToWrite[ n ].data.vDouble;
					uint64_t *pR = (uint64_t*) &varToRead[ n ].data.vDouble;

					cout << hex <<  *pW << " " << *pR << dec << endl;
				}
				else
				{
					uint64_t *pW = (uint64_t*) varToWrite[ n ].data.pDouble;
					uint64_t *pR = (uint64_t*) varToRead[ n ].data.pDouble;

					for (int k = 0; k < varToWrite[n].M; ++k)
					{
						cout << hex << pW[ k ] << " " << pR[ k ] << dec << endl;
					}
				}
			}

			cout << "___________________" << endl;

		}


		success = (success && result);

		cout << ((result)? "EQUAL": "DIFFERENT") << endl<<endl;
	}
	
	if (success == false)
	{
		cout << "Failed reading what has been written " << endl;
		return -1;
	}
	delete [] varToWrite;
	delete [] varToRead;
	return 0;
}

/**
 * Debug 
 */

int DebugFillOemArray()
{
   static char str[] = "DATA:B_GEOMETRY_SCANAREA:A 0.023075,-1.16415e-10,1.5708,-5.411669e-006,-0.023075,-1.16415e-010,1.5708,0.0399383;";
   static char localdatastring[2*sizeof(str)];  

   memcpy(localdatastring, str, sizeof(str));
   oemArray a;

   fill_oem_array_from_data_str(&a, localdatastring, strlen(localdatastring));

   return 0;
}
/**
 *  \brief Displays the values of the global variable "params".
 */
void ShowConnectionParameters()
{
	cout << "   Addr : " << params.GetScannerAddress() ;
	cout << "     | OemPort : " << params.GetOemPort() ;
	cout << "     | ToolboxPort : "<< params.GetToolboxPort() << endl;
	cout << "______________________________________________________________________________"<< endl;

}

/** 
 *\brief Get the latest use case saved by the console.
 *       To have this feature working, one needs to set the EnableUseCaseDumpAfterEachCalc
 *       parameter in console.ini
 *
 */
int GetLatestCalcOkUseCase()
{
	WSAIF wsaif;

	/* Create connections and use them in a parameter synchronization object */
	TcpClient oemClient(&wsaif, 2*1024*1024, params.GetOemPort(), params.GetScannerAddress());
	TcpClient tbxClient(&wsaif, 2*1024*1024, params.GetToolboxPort(), params.GetScannerAddress());
	ParamSyncConnection paramSync(&oemClient, &tbxClient);

	cout << "Connecting to OEM interface ... " ;

	/*  Connect to OEM interface                   */
	bool connected = paramSync.ConnectOEMInterface();

	if (!connected)
	{
		cout << " Failed to connect to OEM interface " << endl;
		return -1;
	}

	cout << "SUCCESS" << endl;
	cout << "Connecting to Toolbox interface ... " ;
	connected = paramSync.ConnectToolboxCommandInterface();

	
	if (!connected)
	{
		cout << "FAILED" << endl;
		return -1;
	}
	
	cout << "Getting USE case ... "  << endl;

	char * useCaseMem = NULL;
	bool success = paramSync.GetConsoleLatestCalcOKUseCase(&useCaseMem);

	if (!success)
	{
		cout << "FAILED" << endl;
	}
	else
	{
		cout << "SUCCESS" <<endl;
	}

	cout << "____________________________________________________" << endl;
	cout << "Do you want to save the usecase ? (y/n) : " ;

	char ans;
	cin >> ans;
	ans = (char)(tolower(ans));
	if (ans == 'y')
	{
		 
		static char filename[1024];
		cout  << "File name : " ;
		cin >> filename;
		
		FILE * fid;

		fid = fopen(filename, "wb");
		size_t count = fwrite(useCaseMem, strlen(useCaseMem), 1, fid);
		fclose(fid); 
		if (count == 1)
		{
			cout << "File saved" << endl;
		}
		else
		{
			cout << "Failed saving file" << endl;
		}
		
	}
	
	cout << "____________________________________________________" << endl;
	cout << useCaseMem << endl;
	cout << "____________________________________________________" << endl;

	return 0;
}

/// <summary>	Tests use case parser. </summary>
/// <returns>	. </returns>
int TestUseCaseParser()
{
	char useCaseName[256] ;

	cout << "Input a name of an use case : ";

	cin >> useCaseName;

	UseCaseParser parser(useCaseName);


	ScanParams scan;

	parser.ReadUseCaseStruct("ScanParams", 9, &scan, gScanParamsDef);
	
	static char buffer [2048];
	parser.PrintToCharArray(buffer, sizeof(buffer), scan, gScanParamsDef);

	cout << buffer << endl;

	Int32Vector mlm;
    parser.GetActiveMidlevelModes(&mlm);

	return 0;
}





/**
 *   \brief Manually set new connection parameters
 *   \returns 0. This function cannot fail, and 0 indicates success
 */
int ChangeConnectionSettings()
{
	unsigned short oemPort;
	unsigned short tbxport;
	char tcpAddr[HOST_ADDRESS_LENGTH];

	cout << "               Oem Port :"; cin >> oemPort;
	cout << "           Toolbox Port :"; cin >> tbxport;
	cout << " Scanner TCP/IP address :"; cin >> tcpAddr;

	params.SetOemPort(oemPort);
	params.SetToolboxPort(tbxport);
	params.SetScannerAddress(tcpAddr);

	return 0;
}




// <summary> Pointer to a benchmarking function </summary>
typedef int (__cdecl * FuncPtr)();

/// <summary> Benchmarking structure - pointer to function and a description </summary>
typedef struct {
	FuncPtr func;
	char* descr;
}FuncCallStruct;



FuncCallStruct testFunctions[] = {
	/* ----- [ Tests involving the network ] ---- */
	{ChangeConnectionSettings,     " Change Connection Settings"},
	{ConnectToScanner,             " Connect to Scanner " },
	{GetUseCaseFromToolbox,        " Get Usecase from Toolbox "},
	{GetOemParameters,             " Get Oem Parameters" },
	{CaptureJPG,                   " Capture JPG image"},
	{SendReceiveOemCommands,       " Send Oem Commands. Receive replies"},
	{GetLatestCalcOkUseCase,       " Get the latest Use Case " },
	{CalcSapBufSize,               " Calculate Sapera Buffer Size from Latest Use Case\n" },
	{CaptureJPGFrameRate,          " Measure frame rate capturing JPG image"},
	/* ---- [ Tests without network ] ---- */
	{ManipulateOemArrayVars,       " Manipulation of OEM Array variables "},
	{TestCreattionOfOemParameters, " Creation of OEM parameters"},
	{ReadWriteOemArrayFile,        " Reading and writing oem parameters to file"},
	{TestUseCaseParser,            " Read a structure from an use case file"},
	{DebugFillOemArray,             " Debug fill oem array "},
	{NULL,                  NULL}
};




void ChooseRunTest()
{
	
	while(true)
	{
		int N;  // N is used as a counter. After the loop, it will contain the number of menu points

		system("cls");   // Clear the console window
		ShowConnectionParameters();

		for (N = 0; testFunctions[ N ].func != NULL; ++N )
		{
			cout << "  " << N << ". " << testFunctions[ N ].descr << endl;
		}
		cout << "  " << N << ". " << " Quit "<< endl;
		
		int choice = N;
		cout << ">> ";
		cin >> choice;

		if (choice == N)
		{
			return;
		}
		else if (choice > N)
		{
			continue;
		}
		else
		{
			cout << "______________________________________________________________________________"<< endl;
			testFunctions[ choice ].func();
			system("pause");
		}
	}
}





int _tmain(int argc, _TCHAR* argv[])
{
	
	//
	// The following if() is to avoid warning about unused variables
	//
	
	//params.SetScannerAddress("10.200.28.41");
	params.SetScannerAddress("10.1.1.100");
	if (argc > 1)
	{
		for (int n = 0; n < argc; n++)
		{
			cout << argv[n];
		}
	}
	else
	{
		ChooseRunTest();
	}

	return 0;
}


#ifdef _MSC_VER
#pragma warning (pop)
#endif


#endif