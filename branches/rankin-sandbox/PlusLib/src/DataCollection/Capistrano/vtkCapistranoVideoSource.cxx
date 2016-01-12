/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCapistranoVideoSource.h"

//// cannon96
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

#include "vtkUSImagingParameters.h"

#include "usbprobedll_net.h"
#include "BmodeDLL.h"

#include <algorithm>
#include <PlusMath.h>


vtkStandardNewMacro(vtkCapistranoVideoSource);

class vtkCapistranoVideoSource::vtkInternal
{
public:
	// ---------------------------------------------------------------------------
	// Public member variables ---------------------------------------------------
	vtkCapistranoVideoSource *  External;
  
	HWND                        ImageWindowHandle;
	HBITMAP                     DataHandle;
	HANDLE                      ProbeHandle;
	std::vector<unsigned char>  MemoryBitmapBuffer;
	BITMAP                      Bitmap;
	bmBITMAPINFO                BitmapInfo;
	BYTE                        *RfDataBuffer;
	static const int            samplesPerLine = 2048;

	/* ! A data structure to include the parameters of Probe's Servo */
	typedef struct {
		int                       JitterComp;
		int                       PositionScale;
		float                     SweepAngle;
		int                       ServoGain;
		int                       Overscan;
		int                       DerivativeCompensation;  
	} ProbeServo;

	/* ! A Combined data structure for ProbeParams */
	typedef struct{
		ProbeType	                probetype;
		ProbeServo                probeservo;
		int                       Samples;
		int                       Filter;
		bool                      Amode;
		bool                      Preamp;
		int                       DisplayOffset;
		float                     PluseVoltage;
		int                       ProbeID;
	} ProbeParams;

	// Current Probe's parameters	
	ProbeParams                 USProbeParams;
	// A database of probe's parameters
	std::map<int, ProbeParams>  USProbeParamsDB;   
  
	// Current Probe's pulser
	PULSER                      USProbePluserParams;
	// A database  of probe's pulser
	std::map<float, PULSER>     USProbePluserParamsDB;
  
	// ---------------------------------------------------------------------------
	// Public member functions  --------------------------------------------------
	/*! Constructor */
	vtkCapistranoVideoSource::vtkInternal::vtkInternal(vtkCapistranoVideoSource * external):
	External( external )
	, RfDataBuffer(NULL) 
	, ProbeHandle(NULL)
	{
		this->CreateUSProbeParamsDB();
		this->CreateUSProbePluserParamsDB();
	}

	/*! Destructor */
	virtual vtkCapistranoVideoSource::vtkInternal::~vtkInternal()
	{
		this->USProbeParamsDB.clear();
		this->USProbePluserParamsDB.clear();

		this->External = NULL;
	}

	/* Clear a linear TGC for US B-Mode image */
	void vtkCapistranoVideoSource::vtkInternal::CreateLinearTGC(int tgcMin, int tgcMax)
	{
		int tgc[samplesPerLine]={0};
		int b                  = tgcMin;
		float m                = (float) (tgcMax - tgcMin) / samplesPerLine;
		for (int x = 0; x < samplesPerLine; x++)
		{
			tgc[x] = (int) (m * (float) x) + b;
		}
    
		bmSetTGC(tgc);
	}

	/* Clear a linear TGC for US B-Mode image */
	void vtkCapistranoVideoSource::vtkInternal::CreateLinearTGC(int initialTGC, int midTGC, int farTGC)
	{

		/*  A linear TGC function is created. 
			The initial point is initialTGC, then is linear until the
			middle point (midTGC) and then linear until the maximum depth 
			where the compensationis equal to farTGC*/

		int tgc[samplesPerLine]= {0};
		double firstSlope      = (double) (midTGC-initialTGC ) / (samplesPerLine/2);
		double secondSlope     = (double) (farTGC-midTGC ) / (samplesPerLine/2);
    
		for (int x = 0; x < samplesPerLine/2; x++)
		{
			tgc[x] = (int) (firstSlope * (double) x) + initialTGC;
			tgc[samplesPerLine/2 +x] = (int) (secondSlope * (double) x) + midTGC;
		}
    
		bmSetTGC(tgc);
	}

	/* Clear a Sine TGC for US B-Mode image */
	void vtkCapistranoVideoSource::vtkInternal::CreateSinTGC(int initialTGC, int midTGC, int farTGC)
	{
		/*  A sine TGC function is created. */
		int x, b;
		float m,tgc, c;
    
		int TGC[samplesPerLine]={0};
    
		b = initialTGC;
		m = (float)(farTGC - initialTGC)/samplesPerLine;
		c = (float)(midTGC) -  (farTGC + initialTGC)/2.0f;

		for(x=0; x< samplesPerLine; x++)
		{
			tgc = (m * (float)x) + b;
			TGC[x] = (int)(tgc + (float)c*std::sin(x*vtkMath::Pi()/samplesPerLine -1.0f));
		}
    
		bmSetTGC(TGC);
	}

	/* ???  */
	static LRESULT CALLBACK vtkCapistranoVideoSource::vtkInternal::ImageWindowProc(HWND hwnd, 
																					UINT iMsg, 
																					WPARAM wParam, 
																					LPARAM lParam)
	{
		vtkCapistranoVideoSource::vtkInternal* self = 
			(vtkCapistranoVideoSource::vtkInternal*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
	}

	/* Initialize DIB for US B-Mode image */
	PlusStatus vtkCapistranoVideoSource::vtkInternal::InitializeDIB(int imageSize[2])
	{
		this->BitmapInfo.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
		this->BitmapInfo.bmiHeader.biWidth          = imageSize[0];
		this->BitmapInfo.bmiHeader.biHeight         = -imageSize[1];
		this->BitmapInfo.bmiHeader.biPlanes         = 1;
		this->BitmapInfo.bmiHeader.biBitCount       = 8;
		this->BitmapInfo.bmiHeader.biCompression    = 0;
		this->BitmapInfo.bmiHeader.biXPelsPerMeter  = 0;
		this->BitmapInfo.bmiHeader.biYPelsPerMeter  = 0;
		this->BitmapInfo.bmiHeader.biClrUsed        = 0;
		this->BitmapInfo.bmiHeader.biClrImportant   = 0;

		// Compute the number of bytes in the array of color  
		// indices and store the result in biSizeImage.  
		// The width must be DWORD aligned unless the bitmap is RLE compressed. 
		this->BitmapInfo.bmiHeader.biSizeImage      = ((imageSize[0]*8+31)&~31)/8 * imageSize[1]; 	 /// cannon96:: check this part

		for( int i = 0; i < 256; ++i )
		{
			this->BitmapInfo.bmiColors[i].rgbRed      = i;
			this->BitmapInfo.bmiColors[i].rgbBlue     = i;
			this->BitmapInfo.bmiColors[i].rgbGreen    = i;
			this->BitmapInfo.bmiColors[i].rgbReserved = 0;
		}

		bmSetBitmapInfo(&BitmapInfo);
		return PLUS_SUCCESS;
	}

	/* Clear a linear LUT for US B-Mode image */
	void CreateLinearLUT(BYTE lut[], int Level, int Window)
	{
		int center = Window / 2;          // center of window
		int left   = Level - center;      // left of window
		int right  = Level + center;      // right of window

		// everything to our left is black
		for (int x=0; x<left; x++)
		{
			lut[x]   = 0;
		}

		// everything to our right is white
		for (int x=right+1; x < 256; x++)
		{
			lut[x]   = 255;
		}

		// everything in between is on the line
		float m    = 255.0f / ((float) Window);

		int startX = std::max(0, left);
		int endX   = std::min(right, 255);

		for (int x=startX; x <= endX; x++)
		{
			int y    = (int) (m * (float)(x-left) + 0.5f);	  

			lut[x]   = (BYTE)std::max(0, std::min(y,255));
		}
	}

	/* Clear an LUT for US B-Mode image */
	void CreateLUT(BYTE lut[], int Brightness, int Contrast, int Level, int Window)
	{
		int center = Window / 2;                // center of window
		int left   = Level - center;              // left of window
		int right  = Level + center;             // right of window
		for (int x=0; x <= 255; x++)
		{
			int y    = (int) ((float)Contrast/256.0f * (float)(x-128) + Brightness);

			lut[x]   = std::min(255, std::max(y,0));
		}
	}

	/* Clear BITMAP buffer for US B-Mode image */
	void vtkCapistranoVideoSource::vtkInternal::ClearBitmap()
	{
		bmClearBitmap(this->ImageWindowHandle, this->Bitmap);
	}

	/*! Check Capistrano US Hardware */
	void vtkCapistranoVideoSource::vtkInternal::CheckCapistranoUSHW()
	{
		if (usbHardwareDetected())
		{
			// get handle to hardware probe
			usbProbeHandle(0, &ProbeHandle);
			// and be sure it's selected as active rpobe
			usbSelectProbe(ProbeHandle, 0);
		}
		else // no hardware present so use the "user probe" 
		{
			ProbeHandle = usbUserProbeHandle();
			usbSelectProbe(ProbeHandle, 0);
		}
	}

	/*! Create a data-base of ProbeType instead of unsing cliProbe.xml file */
	void vtkCapistranoVideoSource::vtkInternal::CreateUSProbeParamsDB()
	{
		USProbeParamsDB.clear();	
		ProbeParams pt;
		// ID : 0 ---------------------------------------------------------------
		pt.probetype.PFDistance              = 20.0f;		 // pivot to face distance (mm)
		pt.probetype.FFDistance              = 1.0f;		 // trancducer face to probe face distanc
		pt.probetype.Velocity                = 1532.0f;  // sound velocity (mm/us)  	  
		pt.probetype.NumVectors              = 255;		   // number of vectors taken
		pt.Samples                           = 2048;
		pt.probetype.PulseFrequency          = 35.0f;		 // current pulse frequency
		pt.Filter                            = 22;
		pt.probetype.SampleFrequency         = 80.0f;		 // in MHz
		pt.Amode                             = false;
		pt.Preamp                            = false;
		pt.probetype.DisplayAngle            = 			     // display angle from center (rad)
			vtkMath::RadiansFromDegrees(30.0f/2.0f);
		sprintf(pt.probetype.Name, "WP");			           // Name of this probe
		pt.DisplayOffset                     = 0;
		pt.PluseVoltage                      = 100.0f;
		pt.probetype.OversampleRate          = 0.0f;     // Amount of oversampling in R
		pt.probetype.PivFaceSamples          = 0.0f;     // calculated pivot to face samples
		pt.probetype.MModeOffset             = 0.0f;     // Fudge Factor for MMode calibration (unused)
		pt.probetype.ArcScan                 = 0;			   // Is this an ArcScan probe
		pt.probetype.PFDOffset               = 0;        // offset added to FPD when delaying sampling
		pt.probeservo.JitterComp             = 25;
		pt.probeservo.PositionScale          = 60;
		pt.probeservo.SweepAngle             = 36.0f;
		pt.probeservo.ServoGain              = 60;
		pt.probeservo.Overscan               = 50;
		pt.probeservo.DerivativeCompensation = 100;
		pt.ProbeID                           = 0;        // ProbeID
		USProbeParamsDB[0]                   = 	pt;

		// ID : 1 ---------------------------------------------------------------
		pt.probetype.PFDistance              = 4.94f;		 // pivot to face distance (mm)
		pt.probetype.FFDistance              = 3.15;		 // trancducer face to probe face distanc
		pt.probetype.Velocity                = 1532.0f;  // sound velocity (mm/us)  	  
		pt.probetype.NumVectors              = 255;		   // number of vectors taken
		pt.Samples                           = 2048;
		pt.probetype.PulseFrequency          = 12.0f;		 // current pulse frequency
		pt.Filter                            = 8;
		pt.probetype.SampleFrequency         = 40.0f;		 // in MHz
		pt.Amode                             = false;
		pt.Preamp                            = false;
		pt.probetype.DisplayAngle            = 			     // display angle from center (rad)
			vtkMath::RadiansFromDegrees(60.0f/2.0f);
		sprintf(pt.probetype.Name, "OP10");			         // Name of this probe
		pt.DisplayOffset                     = 128;
		pt.PluseVoltage                      = 75.0f;
		pt.probetype.OversampleRate          = 0.0f;     // Amount of oversampling in R
		pt.probetype.PivFaceSamples          = 0.0f;     // calculated pivot to face samples
		pt.probetype.MModeOffset             = 0.0f;     // Fudge Factor for MMode calibration (unused)
		pt.probetype.ArcScan                 = 0;			   // Is this an ArcScan probe
		pt.probetype.PFDOffset               = 0;        // offset added to FPD when delaying sampling
		pt.probeservo.JitterComp             = 35;
		pt.probeservo.PositionScale          = 14;
		pt.probeservo.SweepAngle             = 70.0f;
		pt.probeservo.ServoGain              = 70;
		pt.probeservo.Overscan               = 25;
		pt.probeservo.DerivativeCompensation = 30;
		pt.ProbeID                           = 1;        // ProbeID

		USProbeParamsDB[1]                   = 	pt;

		// ID : 2 ---------------------------------------------------------------
		pt.probetype.PFDistance              = 4.94f;		 // pivot to face distance (mm)
		pt.probetype.FFDistance              = 3.15;		 // trancducer face to probe face distanc
		pt.probetype.Velocity                = 1532.0f;  // sound velocity (mm/us)  	  
		pt.probetype.NumVectors              = 255;		   // number of vectors taken
		pt.Samples                           = 2048;
		pt.probetype.PulseFrequency          = 16.0f;		 // current pulse frequency
		pt.Filter                            = 8;
		pt.probetype.SampleFrequency         = 40.0f;		 // in MHz
		pt.Amode                             = false;
		pt.Preamp                            = false;
		pt.probetype.DisplayAngle            = 			     // display angle from center (rad)
			vtkMath::RadiansFromDegrees(60.0f/2.0f);
		sprintf(pt.probetype.Name, "OP20");			         // Name of this probe
		pt.DisplayOffset                     = 132;
		pt.PluseVoltage                      = 100.0f;
		pt.probetype.OversampleRate          = 0.0f;     // Amount of oversampling in R
		pt.probetype.PivFaceSamples          = 0.0f;     // calculated pivot to face samples
		pt.probetype.MModeOffset             = 0.0f;     // Fudge Factor for MMode calibration (unused)
		pt.probetype.ArcScan                 = 0;			   // Is this an ArcScan probe
		pt.probetype.PFDOffset               = 0;        // offset added to FPD when delaying sampling
		pt.probeservo.JitterComp             = 0;
		pt.probeservo.PositionScale          = 14;
		pt.probeservo.SweepAngle             = 72.0f;
		pt.probeservo.ServoGain              = 30;
		pt.probeservo.Overscan               = 25;
		pt.probeservo.DerivativeCompensation = 20;
		pt.ProbeID                           = 2;        // ProbeID

		USProbeParamsDB[2] = 	pt;

		// ID : 3 ---------------------------------------------------------------
		pt.probetype.PFDistance              = 4.94f;		 // pivot to face distance (mm)
		pt.probetype.FFDistance              = 3.15;		 // trancducer face to probe face distanc
		pt.probetype.Velocity                = 1532.0f;  // sound velocity (mm/us)  	  
		pt.probetype.NumVectors              = 255;		   // number of vectors taken
		pt.Samples                           = 2048;
		pt.probetype.PulseFrequency          = 16.0f;		 // current pulse frequency
		pt.Filter                            = 11;
		pt.probetype.SampleFrequency         = 40.0f;		 // in MHz
		pt.Amode                             = false;
		pt.Preamp                            = false;
		pt.probetype.DisplayAngle            = 			     // display angle from center (rad)
			vtkMath::RadiansFromDegrees(60.0f/2.0f);
		sprintf(pt.probetype.Name, "NoProbe");			     // Name of this probe
		pt.DisplayOffset                     = 0;
		pt.PluseVoltage                      = 100.0f;
		pt.probetype.OversampleRate          = 0.0f;     // Amount of oversampling in R
		pt.probetype.PivFaceSamples          = 0.0f;     // calculated pivot to face samples
		pt.probetype.MModeOffset             = 0.0f;     // Fudge Factor for MMode calibration (unused)
		pt.probetype.ArcScan                 = 0;			   // Is this an ArcScan probe
		pt.probetype.PFDOffset               = 0;        // offset added to FPD when delaying sampling
		pt.probeservo.JitterComp             = 25;
		pt.probeservo.PositionScale          = 14;
		pt.probeservo.SweepAngle             = 70.0f;
		pt.probeservo.ServoGain              = 40;
		pt.probeservo.Overscan               = 25;
		pt.probeservo.DerivativeCompensation = 30;
		pt.ProbeID                           = 3;        // ProbeID

		USProbeParamsDB[3]                   = 	pt;
	}

	/*! Retrive US probe parameters from pre-defined values */
	bool vtkCapistranoVideoSource::vtkInternal::SetUSProbeParamsFromDB(int probeID)
	{
		// Searching -----------------------------------------------------------
		std::map<int, ProbeParams>::iterator it;
		it = this->USProbeParamsDB.find(probeID);
		if( it == this->USProbeParamsDB.end())
			return false;

		// Check Capistrano US Hardware ----------------------------------------
		CheckCapistranoUSHW();
	  
		this->USProbeParams = it->second;

		// Set Capistrano US Probe ---------------------------------------------
		// Update the sample delay
		usbSetSampleDelay(this->USProbeParams.probetype.PFDOffset);
		// Update the number of Scan vectors
		usbSetProbeVectors(this->USProbeParams.probetype.NumVectors);
		this->USProbeParams.probetype.NumVectors = usbProbeNumVectors(NULL);
		// Update the filter
		switch(this->USProbeParams.Filter)
		{
		case 8:
			usbSetFilter(FILTER_8MHZ);
			break;
		case 11:
			usbSetFilter(FILTER_11MHZ);
			break;
		case 15:
			usbSetFilter(FILTER_15MHZ);
			break;
		case 18:
			usbSetFilter(FILTER_18MHZ);
			break;
		default:
		case 22:
			usbSetFilter(FILTER_22MHZ);
			break;
		case 25:
			usbSetFilter(FILTER_25MHZ);
			break;
		case 0:
			usbSetFilter(FILTER_OFF);
			break;
		}
		// Update the sample frequency
		int s = (int) (80 / this->USProbeParams.probetype.SampleFrequency);
		usbSetSampleClockDivider(s);
		// set AMode
		if(this->USProbeParams.Amode)
			usbSetAMode(ON);
		else
			usbSetAMode(OFF);
		// Update pulse voltage
		usbSetPulseVoltage(this->USProbeParams.PluseVoltage);
		// Update the value of jitter compensation
		usbSetProbeJitterComp((unsigned char)this->USProbeParams.probeservo.JitterComp);
		// Update the position scale value for the probe
		usbSetProbePositionScale((unsigned char)this->USProbeParams.probeservo.PositionScale);
		// Update the desired probe scan angle	
		usbSetProbeAngle(this->USProbeParams.probeservo.SweepAngle);	//vtkMath::RadiansFromDegrees
		//usbSetProbeAngle(vtkMath::RadiansFromDegrees(this->USProbeParams.probeservo.SweepAngle));	//vtkMath::RadiansFromDegrees
		// Update the gain value for the probe
		usbSetProbeServoGain((unsigned char)this->USProbeParams.probeservo.ServoGain);
		// Update the display offset value for the probe
	  
		// unsigned char doffset =  (unsigned char)(this->USProbeParams.DisplayOffset & 0xff);
		// usbSetProbeDisplayOffset(doffset);
      
		// Update the desired overscan multiplier
		int byteTemp = (int)(this->USProbeParams.probeservo.Overscan/ 6.25f) - 1;
		usbSetProbeOverscan(byteTemp);
		// Update the desired probe servo derivative compensation
		usbSetProbeDerivComp((unsigned char)this->USProbeParams.probeservo.DerivativeCompensation);

		return true;
	}

	/*! Create a data-base of PLUSER instead of unsing cliPluser.xml file */
	void vtkCapistranoVideoSource::vtkInternal::CreateUSProbePluserParamsDB()
	{
		USProbePluserParamsDB.clear();
		PULSER pluser;

		// Frequency = 10.0f ----------------------------------------------------
		pluser.MINDelay              = 93;
		pluser.MIDDelay              = 94;
		pluser.MAXDelay              = 184;

		USProbePluserParamsDB[10.0f] = pluser;

		// Frequency = 12.0f ----------------------------------------------------
		pluser.MINDelay              = 13;
		pluser.MIDDelay              = 13;
		pluser.MAXDelay              = 13;

		USProbePluserParamsDB[12.0f] = pluser;

		// Frequency = 16.0f ----------------------------------------------------
		pluser.MINDelay              = 55;
		pluser.MIDDelay              = 56;
		pluser.MAXDelay              = 109;

		USProbePluserParamsDB[16.0f] = pluser;

		// Frequency = 18.0f ----------------------------------------------------
		pluser.MINDelay              = 49;
		pluser.MIDDelay              = 50;
		pluser.MAXDelay              = 96;

		USProbePluserParamsDB[18.0f] = pluser;

		// Frequency = 20.0f ----------------------------------------------------
		pluser.MINDelay              = 43;
		pluser.MIDDelay              = 44;
		pluser.MAXDelay              = 85;

		USProbePluserParamsDB[20.0f] = pluser;

		// Frequency = 25.0f ----------------------------------------------------
		pluser.MINDelay              = 33;
		pluser.MIDDelay              = 34;
		pluser.MAXDelay              = 65;

		USProbePluserParamsDB[25.0f] = pluser;

		// Frequency = 30.0f ----------------------------------------------------
		pluser.MINDelay              = 27;
		pluser.MIDDelay              = 28;
		pluser.MAXDelay              = 52;

		USProbePluserParamsDB[30.0f] = pluser;

		// Frequency = 35.0f ----------------------------------------------------
		pluser.MINDelay              = 23;
		pluser.MIDDelay              = 24;
		pluser.MAXDelay              = 48;

		USProbePluserParamsDB[35.0f] = pluser;

		// Frequency = 45.0f ----------------------------------------------------
		pluser.MINDelay              = 19;
		pluser.MIDDelay              = 20;
		pluser.MAXDelay              = 35;

		USProbePluserParamsDB[45.0f] = pluser;

		// Frequency = 50.0f ----------------------------------------------------
		pluser.MINDelay              = 15;
		pluser.MIDDelay              = 16;
		pluser.MAXDelay              = 29;

		USProbePluserParamsDB[50.0f] = pluser;
	}

	/*! Retrive US pulser parameters from pre-defined values */
	bool vtkCapistranoVideoSource::vtkInternal::SetUSProbePluserParamsFromDB(float freq)
	{
		/// Searching -----------------------------------------------------------
		std::map<float, PULSER>::iterator it;
		it = this->USProbePluserParamsDB.find(freq);
		if( it == this->USProbePluserParamsDB.end())
		{
			return false;
		}

		this->USProbePluserParams = it->second;
		/// Update the pulse frequency for the probe ----------------------------	  
		usbSetPulseFrequency(&this->USProbePluserParams);
		return true;
	}

};


// ----------------------------------------------------------------------------
// Global functions -----------------------------------------------------------

/*! A callback function is called when probe is attached */
int CALLBACK ProbeAttached()
{
  LOG_INFO("Probe attached");
  return 0;
}

/*! A callback function is called when probe is dettached */
int CALLBACK ProbeDetached()
{
  LOG_INFO("Probe detached");
  return 0;
}

// ----------------------------------------------------------------------------
// Public member operators ----------------------------------------------------

// ----------------------------------------------------------------------------
void vtkCapistranoVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

// ---------------------------------------------------------------------------- 
PlusStatus vtkCapistranoVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkCapistranoVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  // Load US Probe's parameters -----------------------------------------------
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(             UpdateParameters,  deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(             BidirectionalMode, deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int,       CineBuffers,       deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float,     SampleFrequency,   deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float,     PluserFrequency,   deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float,     PluserVoltage,     deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float,     SpeedOfSound,      deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(float,     ScanDepth,         deviceConfig);

  // Load US Bmode's parameters -----------------------------------------------
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(             Interpolate,       deviceConfig);
  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(             AverageMode,       deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int,       BModeViewOption,   deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int,   2,  ImageSize,         deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int,       Intensity,         deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int,       Contrast,          deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double,    ZoomFactor,        deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double,    LutCenter,         deviceConfig);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double,    LutWindow,         deviceConfig);
  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(double, 3, GainPercent,       deviceConfig);
  
  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
// ToDo: Need to implement
PlusStatus vtkCapistranoVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  //deviceConfig->SetAttribute("EnableProbeButtonMonitoring", this->EnableProbeButtonMonitoring?"true":"false");

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkIntersonVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkIntersonVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
std::string vtkCapistranoVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;
  versionString << "Interson Bmode DLL v"<<bmDLLVer()<<", USB Probe DLL v"<<usbDLLVer()<< std::ends;
  return versionString.str();
}


// ------------------------------------------------------------------------
// Protected member operators ---------------------------------------------

// ----------------------------------------------------------------------------
vtkCapistranoVideoSource::vtkCapistranoVideoSource()
	: Frozen(true)
{
	this->SetDeviceId("VideoDevice");
	
	this->Internal                               = new vtkInternal(this);

	this->RequireImageOrientationInConfiguration = true;

	this->UpdateParameters                       = true;
	
	// Initialize US Probe	parameters ----------------------------------------
	this->BidirectionalScan                      = false;
	this->ProbeID                                = 0;
	this->ClockDivider                           = 2;       //1 
	this->CineBuffers                            = 32;
	this->SampleFrequency                        = 40.0f;
	this->PulseFrequency                         = 12.0f;
	this->PulseVoltage                           = 30.0f;   // 
	this->SpeedOfSound                           = 1532.0f; // 
	this->ScanDepth                              = 3.6;	    // cm

	// Initialize US B-Mode parameters ------------------------------------------
	this->ImagingParameters                      = 
		                                new vtkUsImagingParameters(this);	
	this->Interpolate                            = true;
	this->AverageMode                            = true;
	this->CurrentBModeViewOption                 = STANDARDVIEW;
	this->ImageSize[0]                           = 640;     // width of Image
	this->ImageSize[1]                           = 800;     // heigh of Image
	this->Brightness                             = 128;     //192
	this->Contrast                               = 256;     //128
	this->ZoomFactor                             = 1.0;
	this->LutCenter                              = 128;     //192
	this->LutWindow                              = 256;     //128
	this->InitialGain                            = -128;     // 0
	this->MidGain                                = -128;     // 0
	this->FarGain                                = -128;     // 0

	// No callback function provided by the device, 
	// so the data capture thread will be used 
	// to poll the hardware and add new items to the buffer
	this->StartThreadForInternalUpdates          = true;
	this->AcquisitionRate                        = 5;
	
}

// ----------------------------------------------------------------------------
vtkCapistranoVideoSource::~vtkCapistranoVideoSource()
{
  if( !this->Connected )
  {
    this->Disconnect();	
  }

  delete this->Internal;
  this->Internal = NULL;

  delete this->ImagingParameters;
  this->ImagingParameters = NULL;
}

// Initialize Capistrano US Probe ---------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InitializeCapistranoProbe()
{
	// Before any probe can be initialized with usbInitializeProbes, 
	// they must be detected. usbFindProbes()
	// will detect all attached probes and initialize the driver. 
	// After a successful call to usbFindProbes, other probe-related functions may be called. 
	// These include: usbInitializeProbes, usbProbeHandle, usbSelectProbe.
	usbErrorString errorStatus={0};
	ULONG status = usbFindProbes(errorStatus);
	LOG_DEBUG("Find USB probes: status="<<status<<", details: "<<errorStatus);
	if (status != ERROR_SUCCESS)
	{
		LOG_ERROR("Capistrano finding probes failed");
		return PLUS_FAIL;
	}

	// Setup Callback functions ----------------------------------------------
	usbSetProbeAttachCallback(&ProbeAttached);
	usbSetProbeDetachCallback(&ProbeDetached);

	// Check updateUSProbeParameters -----------------------------------------
	if (usbSetCineBuffers(this->CineBuffers) != this->CineBuffers)
	{
		LOG_ERROR("Could not allocate Cine buffers.");
		return PLUS_FAIL;
	}

	// Setup Capistrano US Probe ---------------------------------------------
	if (this->SetupProbe(0) == PLUS_FAIL)
	{
		LOG_ERROR("Failed to setup Capistrano US Probe");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetupProbe(int probeID)
{
	// Get ProbeID of an attached Capistrano US Probe ------------------------
	this->ProbeID   = usbAttachedProbeID();
	LOG_DEBUG("Probe ID ="<<ProbeID);

	if (this->ProbeID == 255) // no probe attached
	{
		this->ProbeID = 1;
		LOG_ERROR("Capistrano finding probes failed");
		return PLUS_FAIL;
	}

	// Check How many US probe are connected. --------------------------------
	int numberOfAttachedProbes = usbNumberAttachedProbes();
	LOG_DEBUG("Number of attached probes: "<<numberOfAttachedProbes);
	if (numberOfAttachedProbes==0)
	{
		LOG_ERROR("No Capistrano probes are attached");
		return PLUS_FAIL;
	}
	if (numberOfAttachedProbes>1)
	{
		LOG_WARNING("Multiple Capistrano probes are attached, using the first one");
	}
	
	// Set US Probe directional mode  -----------------------------------------
	usbSetUnidirectionalMode();
	
	// Turn on USB data synchronization checking ------------------------------
	usbTurnOnSync();

	// Read probe parameters from the DB
	// Update Probe structure with the detected ProbeID ----------------------
	this->Internal->SetUSProbeParamsFromDB(this->ProbeID);

	// Update pulserParams structure with the PulseFrequency of
  // the Updated Probe structure	-------------------------------------------
	this->Internal->SetUSProbePluserParamsFromDB(
		  this->Internal->USProbeParams.probetype.PulseFrequency);

	// Update the values of ProbeType structure -------------------------------
	Internal->USProbeParams.probetype.OversampleRate  = 2048.0f/ ImageSize[1];
	Internal->USProbeParams.probetype.SampleFrequency = 80.f / usbSampleClockDivider();
	Internal->USProbeParams.probetype.PivFaceSamples  = 
		Internal->USProbeParams.probetype.PFDistance * 
		1000.0f*Internal->USProbeParams.probetype.SampleFrequency 
		/ (0.5f * Internal->USProbeParams.probetype.Velocity);

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InitializeImageWindow()
{
	// Initialize Display -----------------------------------------------------
	HANDLE display = bmInitializeDisplay(this->ImageSize[0]*this->ImageSize[1], 0);
	if (display == NULL)
	{
		LOG_ERROR("Could not initialize the display");
		return PLUS_FAIL;
	}

	// Initialize DIB -----------------------------------------------------------
	this->Internal->InitializeDIB(this->ImageSize);

	// Initialize vtkPlusDataSource ---------------------------------------------
	vtkPlusDataSource* aSource = NULL;
	
	if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
	{
		LOG_ERROR("Unable to retrieve the video source in the IntersonVideo device.");
		return PLUS_FAIL;
	}

	// Create Window Handle ----------------------------------------------------- 
	HINSTANCE hInst               = GetModuleHandle(NULL);

	WNDCLASSEX           wndclass = {};
	wndclass.cbSize               = sizeof (wndclass);
	wndclass.style                = CS_CLASSDC;
	wndclass.lpfnWndProc          = vtkCapistranoVideoSource::vtkInternal::ImageWindowProc;
	wndclass.cbClsExtra           = 0;
	wndclass.cbWndExtra           = 0;
	wndclass.hInstance            = hInst;
	wndclass.hIcon                = NULL;
	wndclass.hCursor              = NULL;
	wndclass.hbrBackground        = NULL;
	wndclass.lpszMenuName         = NULL ;
	wndclass.lpszClassName        = TEXT("ImageWindow");
	wndclass.hIconSm              = NULL;
	RegisterClassEx(&wndclass);

	this->Internal->ImageWindowHandle = 
		CreateWindow( TEXT("ImageWindow"), TEXT("Ultrasound"),
		              WS_OVERLAPPEDWINDOW,
					        0, 0,
					        this->ImageSize[0],
					        this->ImageSize[1],
					        NULL, NULL, hInst, NULL);

	if (this->Internal->ImageWindowHandle==NULL)
	{
		LOG_ERROR("Failed to create capture window");
		return PLUS_FAIL;
	}

	// Create a bitmap for use in our DIB ---------------------------------------
	HDC  hdc                      = GetDC(this->Internal->ImageWindowHandle) ;
	RECT rect;
	GetClientRect (this->Internal->ImageWindowHandle, &rect) ;
	int  cx                       = ImageSize[0];//rect.right - rect.left;
	int  cy                       = ImageSize[1];//rect.bottom - rect.top;
	this->Internal->DataHandle    = CreateCompatibleBitmap (hdc, cx, cy);
	GetObject (this->Internal->DataHandle, sizeof (BITMAP), (LPVOID) &this->Internal->Bitmap) ;
	
	// zero indexed window including borders
	this->Internal->MemoryBitmapBuffer.resize(ImageSize[0]*ImageSize[1],0);
	this->Internal->Bitmap.bmBits = &this->Internal->MemoryBitmapBuffer[0]; 

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InitializeLUT()
{
	BYTE lut[256];
	double intensity           = -1;
	double contrast            = -1;
	this->ImagingParameters->GetIntensity(intensity);
	this->ImagingParameters->GetContrast(contrast);
	if (intensity >=0)
	{
		this->Brightness       = (int)intensity;
	}

	if (contrast >=0)
	{
		this->Contrast         = (int)contrast;
	}
	
	this->Internal->CreateLUT(lut, this->Brightness, this->Contrast, 
		                           this->LutCenter, this->LutWindow);
	
	bmCreatebLUT(lut);

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InitializeTGC()
{
	double gain[3]={-1,-1,-1};
	this->ImagingParameters->GetGainPercent(gain);
	if (gain[0]>=0 && gain[1]>=0 && gain[2]>=0)
	{
		this->SetGainPercentDevice(gain); 
	}

	return PLUS_SUCCESS;
}

// Device-specific functions --------------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalConnect()
{
	LOG_TRACE( "vtkCapistranoVideoSource::InternalConnect" );

	// Initialize vtkPlusDataSource ---------------------------------------------
	vtkPlusDataSource* aSource = NULL;
	
	if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
	{
		LOG_ERROR("Unable to retrieve the video source in the IntersonVideo device.");
		return PLUS_FAIL;
	}

	// Clear buffer on connect because the new frames that we will 
	// acquire might have a different size
	aSource->Clear();
  aSource->SetInputImageOrientation(US_IMG_ORIENT_NU);
	aSource->SetImageType(US_IMG_BRIGHTNESS);
	aSource->SetPixelType( VTK_UNSIGNED_CHAR );
	aSource->SetInputFrameSize(this->ImageSize[0], this->ImageSize[1], 1);

	LOG_DEBUG("Capistrano Bmode DLL version "<<bmDLLVer()<<", USB probe DLL version "<<usbDLLVer());

	// Initialize display ----------------------------------------------------
	if ( InitializeImageWindow() == PLUS_FAIL)
	{
		LOG_ERROR("Failed to initialize Image Window");
		return PLUS_FAIL;
	}

	// Initialize Capistrano US Probe ----------------------------------------
	if ( InitializeCapistranoProbe() == PLUS_FAIL)
	{
		LOG_ERROR("Failed to initialize Capistrano US Probe");
		return PLUS_FAIL;
	}

	// Update US parameters --------------------------------------------------
	if(this->UpdateParameters)
	{
		if(this->UpdateUSProbeParameters() == PLUS_FAIL)
		{
			LOG_ERROR("Failed to UpdateUSProbeParameters");
			return PLUS_FAIL;
		}
		
		if(this->UpdateUSBModeParameters() == PLUS_FAIL)
		{
			LOG_ERROR("Failed to UpdateUSBModeParameters");
			return PLUS_FAIL;
		}
	}
	else
	{
		// Initialize the gain and lut now that we have our default values ---	
		this->InitializeLUT();
		this->InitializeTGC();
		
		// Update the scan depth of B-Mode Image -----------------------------
		if (this->UpdateDepthMode(this->ClockDivider) == PLUS_FAIL)
		{					    
			LOG_ERROR("Failed to update Depth Mode");
			return PLUS_FAIL; 
		}
	
	}
	
	// Check the connection of the Capistrano US Hardware --------------------
	if(!usbHardwareDetected())
	{
		LOG_ERROR("No Capistrano US Hardware");
		return PLUS_FAIL;
	}

	
	usbSetUnidirectionalMode();
	usbSetPulseVoltage(this->PulseVoltage);								 
	
	// Set the Size of CineBuffer -----------------------------------------------
	// Setup the display offsets now that we have the probe and DISPLAY data
	bmSetDisplayOffset(0);

  
	this->Internal->RfDataBuffer = usbCurrentCineFrame();
	bmClearBitmap(this->Internal->ImageWindowHandle, this->Internal->Bitmap);

	// Set the sample Delay -----------------------------------------------------
	usbSetSampleDelay(0);

	// DEBUG LOG : Get the name of a current probe
	std::string probeName;
	GetProbeNameDevice(probeName);
	LOG_DEBUG("Capistrano probe name: "<<probeName<<", ID: "<<usbProbeID(this->Internal->ProbeHandle));

	
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalDisconnect()
{
	LOG_DEBUG("Disconnect from Capistrano");

	FreezeDevice(true); 
	bmCloseDisplay();

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalStartRecording()
{
	FreezeDevice(false);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalStopRecording()
{
	FreezeDevice(true);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalUpdate()
{
	if (!this->Recording)
	{
		// drop the frame, we are not recording data now
		return PLUS_SUCCESS;
	}

	WaitForFrame();
  
	int frameNum                 = usbCineFrameNumber();
	 
	this->Internal->RfDataBuffer = usbCurrentCineFrame();
	  
	this->Internal->DataHandle   = bmDrawImage(this->Internal->ImageWindowHandle,
				                                     this->Internal->RfDataBuffer,
				                                     this->Internal->Bitmap,
				                                     true, FALSE, 
				                                     NULL, bmDI_DRAW, 0, TRUE);

	GetObject(this->Internal->DataHandle, sizeof(BITMAP), &this->Internal->Bitmap);

	this->FrameNumber++;

	vtkPlusDataSource* aSource=NULL;
	if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
	{
		LOG_ERROR("Unable to retrieve the video source in the ICCapturing device.");
		return PLUS_FAIL;
	}

	int frameSizeInPx[3]={this->ImageSize[0],this->ImageSize[1], 1};

	// If the buffer is empty, set the pixel type and frame size 
	// to the first received properties
	if ( aSource->GetNumberOfItems() == 0 )
	{
		LOG_DEBUG("Set up image buffer for Interson");
		aSource->SetInputImageOrientation(US_IMG_ORIENT_NU);
		aSource->SetPixelType(VTK_UNSIGNED_CHAR);
		aSource->SetImageType(US_IMG_BRIGHTNESS);
		aSource->SetInputFrameSize(frameSizeInPx );

		float depthScale = -1;
		usbProbeDepthScale(this->Internal->ProbeHandle,&depthScale);

		std::string probeName;
		GetProbeNameDevice(probeName);

		LOG_INFO("Frame size: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
			<< ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetPixelType())
			<< ", probe sample frequency (Hz): " << this->Internal->USProbeParams.probetype.SampleFrequency,
			<< ", probe name: " << probeName
			<< ", display zoom: " << bmDisplayZoom()
			<< ", probe depth scale (mm/sample):" << depthScale
			<< ", buffer image orientation: " 
			<< PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetInputImageOrientation()));
	}

	TrackedFrame::FieldMapType customFields;

	if( aSource->AddItem((void*)this->Internal->Bitmap.bmBits,//(void*)&(this->Internal->MemoryBitmapBuffer[0]), 
		        aSource->GetInputImageOrientation(),//US_IMG_ORIENT_NU,
						frameSizeInPx, VTK_UNSIGNED_CHAR, 
						1, US_IMG_BRIGHTNESS, 0, 
						this->FrameNumber, 
						UNDEFINED_TIMESTAMP, 
						UNDEFINED_TIMESTAMP, 
						&customFields) != PLUS_SUCCESS )
	{
		LOG_ERROR("Error adding item to video source " << aSource->GetSourceId());
		return PLUS_FAIL;
	}

	this->Modified();
	
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::FreezeDevice(bool freeze)
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkIntersonVideoSource::FreezeDevice failed: device not connected");
		return PLUS_FAIL;
	}
	
	if (!usbHardwareDetected())
	{
		LOG_ERROR("Freeze failed, no hardware is detected");
		return PLUS_FAIL;
	}
	
	this->Frozen=freeze;
	if (this->Frozen)
	{
		usbProbe(STOP);
	}
	else
	{
		usbClearCineBuffers();
		this->FrameNumber = 0;
		if(this->UpdateUSParameters() == PLUS_FAIL)
		{
			LOG_ERROR("Failed to update US parameters");
			return PLUS_FAIL;
		}
		usbProbe(RUN);
	}

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::WaitForFrame() 
{
	bool  nextFrameReady = (usbWaitFrame()==1);
	DWORD usbErrorCode   = usbError();

	switch (usbErrorCode)
	{
		case USB_SUCCESS:
			break;
		case USB_FAILED:
			LOG_ERROR("USB: FAILURE. Probe was removed?");
			return PLUS_FAIL;
		case USB_TIMEOUT2A:
		case USB_TIMEOUT2B:
		case USB_TIMEOUT6A:
		case USB_TIMEOUT6B:
			if (nextFrameReady) // timeout is fine if we're in synchronized mode, so only log error if next frame is ready
			{
				LOG_WARNING("USB timeout");
			}
		break;
		case USB_NOTSEQ:
			LOG_ERROR("Lost Probe Synchronization. Please check probe cables and restart.");
			break;
		case USB_STOPPED:
			LOG_ERROR("USB: Stopped. Check probe and restart.");
			break;
		default:
			LOG_ERROR("USB: Unknown USB error: "<<usbErrorCode);
			break;
	}

	return PLUS_SUCCESS;
}

// Setup US Probe parameters --------------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetUpdateParameters(bool b)
{
	this->UpdateParameters  = b;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetBidirectionalMode(bool mode)
{
	this->BidirectionalScan = mode;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetCineBuffers(int cinebuffer)
{
	this->CineBuffers       = cinebuffer;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetSampleFrequency(float sf)
{
	this->SampleFrequency   = sf;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetPluserFrequency(float pf)
{
	this->PulseFrequency    = pf;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetPluserVoltage(float pv)
{
	this->PulseVoltage      = pv;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetSpeedOfSound(float ss)
{
	this->SpeedOfSound      = ss;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetScanDepth(float sd)
{
	this->ScanDepth         = sd;
	return PLUS_SUCCESS;
}

// setup USBmode parameters ---------------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetInterpolate(bool interpolate)
{
	this->Interpolate       = interpolate;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetAverageMode(bool averagemode)
{
	this->AverageMode       = averagemode;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetBModeViewOption(int bmodeviewoption)
{
	this->CurrentBModeViewOption 
		                      = (unsigned int) bmodeviewoption;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetImageSize(int imageSize[2])
{
	this->ImageSize[0]      = imageSize[0];
	this->ImageSize[1]      = imageSize[1];
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetIntensity(int value)
{
	this->ImagingParameters->SetIntensity(value);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetContrast(int value)
{
	this->ImagingParameters->SetContrast(value);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetZoomFactor(double zoomfactor)
{
	this->ZoomFactor        = zoomfactor;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetDisplayZoomDevice(double zoom)
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkIntersonVideoSource::SetDisplayZoomDevice failed: device not connected");
		return PLUS_FAIL;
	}
	
	bmSetDisplayZoom(zoom);
	LOG_TRACE("New zoom is " << bmDisplayZoom());
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetLutCenter(double lutcenter)
{
	this->LutCenter         = lutcenter;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetLutWindow(double lutwindow)
{
	this->LutWindow         = lutwindow;
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetGainPercent(double gainPercent[3])
{
	this->ImagingParameters->SetGainPercent(gainPercent);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetGainPercentDevice(double gainPercent[3])
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkIntersonVideoSource::SetGainPercentDevice failed: device not connected");
		return PLUS_FAIL;
	}
	SetGainPercent(gainPercent);
	/* The following commented code is useful when using an RF probe with an analog TGC control.
	It sets the value, in dB, for the gain at the  last sample taken.   */

	/* If the above code is executed the gain values are changed but it has no effect on the image. Probably it is beacause the probe
	does not have analog TGC control.
	The code below sets a linear TGC curve based on three values (initial, middle and end) of the curve.*/

	double maximumTGC       = 512;
	if (gainPercent[0]>=0 && gainPercent[1]>=0 && gainPercent[2]>=0)
	{
		this->InitialGain     = -255 + gainPercent[0] * maximumTGC /100 ;
		this->MidGain         = -255 + gainPercent[1] * maximumTGC /100 ;
		this->FarGain         = -255 + gainPercent[2] * maximumTGC /100 ;
	}

	this->Internal->CreateLinearTGC(this->InitialGain,this->MidGain,this->FarGain);

	bmTurnOnTGC();
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetDepthMmDevice(float depthMm)
{
	int temp = (int)(depthMm/1.8f);
	
	if(temp > 4 || temp < 1)
	{
		LOG_ERROR("Wrong Scan Depth");
		return PLUS_FAIL;
	}

	// Update the current scandepth with an available scandepth
	this->ScanDepth         = (float)temp * 1.8f;
	this->SetDepthMm(this->ScanDepth);
	
	// Update Sample clock divider 
	this->ClockDivider      = temp;
	
	// Update Depth Mode
	if(this->UpdateDepthMode(this->ClockDivider) == PLUS_FAIL)
	{
		LOG_ERROR("Wrong Scan Depth");
		return PLUS_FAIL;
	}

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetDepthMm(float depthMm)
{
  this->ImagingParameters->SetDepthMm(depthMm);
  return PLUS_SUCCESS;
}

// Update US parameters before acquring US-BMode images -----------------------

// ---------------------------------------------------------------------------- 
PlusStatus vtkCapistranoVideoSource::UpdateUSParameters()
{
  if(this->UpdateUSProbeParameters() == PLUS_FAIL)
	{
		LOG_ERROR("Failed to Update US probe parameters");
		return PLUS_FAIL;
	}
	
	if(this->UpdateUSBModeParameters() == PLUS_FAIL)
	{
		LOG_ERROR("Failed to Update US BMode parameters");
		return PLUS_FAIL;
	}
	
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::UpdateUSProbeParameters()
{
	// Set US Probe scan mode -------------------------------------------------
	if(this->BidirectionalScan)
		usbSetBidirectionalMode();
	else
		usbSetUnidirectionalMode();

	// set the size of CineBuffers -------------------------------------------
	if (usbSetCineBuffers(this->CineBuffers) != this->CineBuffers)
	{
		LOG_ERROR("Could not allocate Cine buffers.");
		return PLUS_FAIL;
	}

	// Update SoundVelocity	--------------------------------------------------
	this->Internal->USProbeParams.probetype.Velocity = this->SpeedOfSound;
	this->ImagingParameters->SetSoundVelocity(this->SpeedOfSound);	 // redundancy: delete this
	
	// Update PulseFrequency
	if(!this->Internal->SetUSProbePluserParamsFromDB(this->PulseFrequency))
	{
		LOG_ERROR("Wrong pulser frequency");
		LOG_ERROR("Possible pulser frequency : 10.0, 12.0, 16.0, 18.0, 20.0, 25.0, 30.0, 35.0, 45.0, 50");
		return PLUS_FAIL;
	}

	// Update PulseVoltage ----------------------------------------------------
	this->Internal->USProbeParams.PluseVoltage       = this->PulseVoltage;
	usbSetPulseVoltage(this->Internal->USProbeParams.PluseVoltage);

	
	// Setup ScanDpthe	-------------------------------------------------------
	if(this->SetDepthMmDevice(this->ScanDepth) == PLUS_FAIL) // call UpdateDepthMode
	{
		LOG_ERROR("Could not setup Scan Depth");
		return PLUS_FAIL;
	}
    
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::UpdateUSBModeParameters()
{
	this->InitializeLUT();
	this->InitializeTGC();

	// Set Zoomfactor
	if (SetDisplayZoomDevice(this->ZoomFactor) == PLUS_FAIL)
	{
		LOG_ERROR("SetDisplayZoomDevice failed");
		return PLUS_FAIL;
	}
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::CalculateDisplay()
{
	POINT ptCenter;        // Points for Zoomed Display
	ptCenter.x                   = this->ImageSize[0]/2;
	ptCenter.y                   = this->ImageSize[1]/2;

	this->CurrentBModeViewOption = OPTHALMICVIEW;

	this->Internal->USProbeParams.probetype.OversampleRate = 2048.0f / ImageSize[1];

    if (bmCalculateZoomDisplay(ImageSize[0], ImageSize[1],
                               ptCenter, 
							                 this->Internal->USProbeParams.probetype, 
							                 (unsigned int)ImageSize[0], STANDARDVIEW) 
							                 == ERROR)
	{
		LOG_ERROR("CalculateDisplay ERROR: Bad Theta Value");
		return PLUS_FAIL;
	} 

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::CalculateDisplay(unsigned int option)
{
	POINT ptCenter;        // Points for Zoomed Display
	ptCenter.x                   = this->ImageSize[0]/2;
	ptCenter.y                   = this->ImageSize[1]/2;

	this->CurrentBModeViewOption = option;

	this->Internal->USProbeParams.probetype.OversampleRate = 2048.0f / ImageSize[1];

    if (bmCalculateZoomDisplay(ImageSize[0], ImageSize[1],
                               ptCenter, 
							                 this->Internal->USProbeParams.probetype, 
							                 (unsigned int)ImageSize[0], this->CurrentBModeViewOption) 
							                 == ERROR)
	{
		LOG_ERROR("CalculateDisplay ERROR: Bad Theta Value");
		return PLUS_FAIL;
	} 

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::UpdateDepthMode()
{
	// Update the values of ProbeType structure
	Internal->USProbeParams.probetype.OversampleRate  = 2048.0f/ ImageSize[1];
	Internal->USProbeParams.probetype.SampleFrequency = 80.f / usbSampleClockDivider();
	Internal->USProbeParams.probetype.PivFaceSamples  = 
		Internal->USProbeParams.probetype.PFDistance * 1000.0f *
		Internal->USProbeParams.probetype.SampleFrequency / 
		(0.5f * Internal->USProbeParams.probetype.Velocity);


	this->SampleFrequency = Internal->USProbeParams.probetype.SampleFrequency;

	usbClearCineBuffers();
	if (this->CalculateDisplay(this->CurrentBModeViewOption) == PLUS_FAIL)
	{
		LOG_ERROR("CalculateDisplay ERROR: Bad Theta Value");
		return PLUS_FAIL;
	}

	this->Internal->ClearBitmap();

	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::UpdateDepthMode(int clockdivider)
{
	usbSetSampleClockDivider(clockdivider);
	return this->UpdateDepthMode();
}

// Get US probe/B-Mode parameters ---------------------------------------------

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::GetSampleFrequencyDevice(float& aFreq)
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkIntersonVideoSource::GetSampleFrequencyDevice failed: device not connected");
		return PLUS_FAIL;
	}
  
	aFreq = this->Internal->USProbeParams.probetype.SampleFrequency;
	LOG_TRACE("Current frequency is " << aFreq);
	return PLUS_SUCCESS;
}
 
// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::GetProbeVelocityDevice(float& aVel)
// ToDo: Check this function
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkIntersonVideoSource::GetProbeVelocityDevice failed: device not connected");
		return PLUS_FAIL;
	}
  
	aVel = this->Internal->USProbeParams.probetype.Velocity;
	LOG_TRACE("Current velocity is " << aVel);
	return PLUS_SUCCESS;
}

// ----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::GetProbeNameDevice(std::string& probeName)
{
	if (this->Internal->ProbeHandle==NULL)
	{
		LOG_ERROR("vtkCapistranoVideoSource::SetGainPercentDevice failed: device not connected");
		return PLUS_FAIL;
	}

	// usbProbeNameString supposed to be able to store the USB probe name
	// but if we use that buffer then it leads to stack corruption,
	// so we use a much larger buffer (the usbProbeNameString buffer size is 20)
	typedef TCHAR usbProbeNameStringSafe[1000];

	usbProbeNameStringSafe probeNameWideStringPtr    = {0};
	usbProbeName(this->Internal->ProbeHandle, probeNameWideStringPtr);

	// Probe name is stored in a wide-character string, convert it to a multi-byte character string
	char probeNamePrintable[usbProbeNameMaxLength+1] = {0};
	wcstombs(probeNamePrintable, (wchar_t*)probeNameWideStringPtr, usbProbeNameMaxLength);

	probeName=probeNamePrintable;

	return PLUS_SUCCESS;
}
