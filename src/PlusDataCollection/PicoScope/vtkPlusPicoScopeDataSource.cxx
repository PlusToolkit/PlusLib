/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

// Local includes
#include "vtkIGSIOAccurateTimer.h"
#include "vtkPlusPicoScopeDataSource.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

// STL includes
#include <string>
#include <sstream>

// PicoScope includes
#include "ps2000.h"

vtkStandardNewMacro(vtkPlusPicoScopeDataSource);

static const int MAX_NUM_SAMPLES_PER_FRAME = 50000;

//-------------------------------------------------------------------------------------------------
class vtkPlusPicoScopeDataSource::vtkInternal
{
public:
	friend class vtkPlusPicoScopeDataSource;

  vtkPlusPicoScopeDataSource* External;

	vtkInternal(vtkPlusPicoScopeDataSource* external)
		: External(external)
	{
		ChannelA.channelNumber = 0;
		ChannelB.channelNumber = 1;
  }

  virtual ~vtkInternal()
  {
  }

  // PICO Scope internals

  typedef enum {
    MODEL_NONE = 0,
    MODEL_PS2104 = 2104,
    MODEL_PS2105 = 2105,
    MODEL_PS2202 = 2202,
    MODEL_PS2203 = 2203,
    MODEL_PS2204 = 2204,
    MODEL_PS2205 = 2205,
    MODEL_PS2204A = 0xA204,
    MODEL_PS2205A = 0xA205
  } MODEL_TYPE;

  typedef struct
  {
    PS2000_THRESHOLD_DIRECTION channelA;
    PS2000_THRESHOLD_DIRECTION channelB;
    PS2000_THRESHOLD_DIRECTION channelC;
    PS2000_THRESHOLD_DIRECTION channelD;
    PS2000_THRESHOLD_DIRECTION ext;
  } DIRECTIONS;

  typedef struct
  {
    PS2000_PWQ_CONDITIONS   * conditions;
    int16_t       nConditions;
    PS2000_THRESHOLD_DIRECTION  direction;
    uint32_t      lower;
    uint32_t      upper;
    PS2000_PULSE_WIDTH_TYPE   type;
  } PULSE_WIDTH_QUALIFIER;

  typedef struct
  {
    PS2000_CHANNEL channel;
    float threshold;
    int16_t direction;
    float delay;
  } SIMPLE;

  typedef struct
  {
    int16_t hysteresis;
    DIRECTIONS directions;
    int16_t nProperties;
    PS2000_TRIGGER_CONDITIONS * conditions;
    PS2000_TRIGGER_CHANNEL_PROPERTIES * channelProperties;
    PULSE_WIDTH_QUALIFIER pwq;
    uint32_t totalSamples;
    int16_t autoStop;
    int16_t triggered;
  } ADVANCED;

  typedef struct
  {
    SIMPLE simple;
    ADVANCED advanced;
  } TRIGGER_CHANNEL;

  // struct to hold scope handle
  typedef struct {
    int16_t   handle;
    MODEL_TYPE  model;
    PS2000_RANGE firstRange;
    PS2000_RANGE lastRange;
    TRIGGER_CHANNEL trigger;
    int16_t   maxTimebase;
    int16_t   timebases;
    int16_t   noOfChannels;
    int16_t   hasAdvancedTriggering;
    int16_t   hasFastStreaming;
    int16_t   hasEts;
    int16_t   hasSignalGenerator;
    int16_t   awgBufferSize;
  } UNIT_MODEL;

  // PLUS structs
  typedef enum {
    AC = 0,
    DC = 1
  } COUPLING;

  typedef struct {
		bool enabled = false;
		int16_t channelNumber = -1; // 0=A, 1=B
    COUPLING coupling = DC;
    PS2000_RANGE voltageRange = PS2000_20V; 
  } PICO_CHANNEL_SETTINGS;

  // METHODS

  // Get info about the connected PicoScope device
  std::string GetPicoScopeInfo(void);

  // Convert integer mv amount into PS2000_RANGE
	PlusStatus RangeIntToEnum(const int voltageRangeMv, PS2000_RANGE& voltRange);

	int RangeEnumToInt(const PS2000_RANGE voltRange);

	std::string RangeEnumToStr(const PS2000_RANGE voltRange);

  // Get the units the ADC is set to
  std::string GetAdcUnits(int16_t time_units);
  
  // Convert ADC counts to volts / mV
  float AdcToMv(int32_t rawCount, int32_t rangeMv);

  // Initalize a PicoScope channel
  PlusStatus SetupPicoScopeChannel(PICO_CHANNEL_SETTINGS channel);

	// Print channel settings
	void PrintChannelInfo(PICO_CHANNEL_SETTINGS channelSettings);

protected:
	// MEMBER VARIABLES

  UNIT_MODEL Scope;  // scope handle

  int PSInputRanges[PS2000_MAX_RANGES] = { 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000 };

	// channel handles
  PICO_CHANNEL_SETTINGS ChannelA;
  PICO_CHANNEL_SETTINGS ChannelB;

	// scope acquisition timing variables
	int AppliedNumberOfSamples;
	int AppliedTimeUnits;
	int AppliedTimebase;

	int16_t SignalARaw[MAX_NUM_SAMPLES_PER_FRAME];
	int16_t SignalBRaw[MAX_NUM_SAMPLES_PER_FRAME];
	int32_t Times[MAX_NUM_SAMPLES_PER_FRAME];
	float SignalBuffer[3 * MAX_NUM_SAMPLES_PER_FRAME];

	// PLUS channel members
	std::string PlusSourceId = "";
	vtkPlusDataSource* PlusSource = nullptr;
};

//-------------------------------------------------------------------------------------------------
std::string vtkPlusPicoScopeDataSource::vtkInternal::GetPicoScopeInfo()
{
  int8_t description[8][25] = { "Driver Version   ",
                "USB Version      ",
                "Hardware Version ",
                "Variant Info     ",
                "Serial           ",
                "Cal Date         ",
                "Error Code       ",
                "Kernel Driver    "
  };
  int16_t i;
  int8_t line[80];
  int32_t variant;

  // stringstream to contain device info
  std::stringstream deviceInfoSS;

  if (this->Scope.handle)
  {
    for (i = 0; i < 8; i++)
    {
      ps2000_get_unit_info(this->Scope.handle, line, sizeof(line), i);

      if (i == 3)
      {
        variant = atoi((const char*)line);

        if (strlen((const char*)line) == 5) // Identify if 2204A or 2205A
        {
          line[4] = toupper(line[4]);

          if (line[1] == '2' && line[4] == 'A')  // i.e 2204A -> 0xA204
          {
            variant += 0x9968;
          }
        }
      }

      if (i != 6) // No need to print error code
      {
        deviceInfoSS << description[i] << " " << line << std::endl;
      }
    }

    switch (variant)
    {
    case MODEL_PS2104:
      this->Scope.model = MODEL_PS2104;
      this->Scope.firstRange = PS2000_100MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2104_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 1;
      this->Scope.hasAdvancedTriggering = false;
      this->Scope.hasSignalGenerator = false;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = false;
      break;

    case MODEL_PS2105:
      this->Scope.model = MODEL_PS2105;
      this->Scope.firstRange = PS2000_100MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2105_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 1;
      this->Scope.hasAdvancedTriggering = false;
      this->Scope.hasSignalGenerator = false;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = false;
      break;

    case MODEL_PS2202:
      this->Scope.model = MODEL_PS2202;
      this->Scope.firstRange = PS2000_100MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = true;
      this->Scope.hasSignalGenerator = false;
      this->Scope.hasEts = false;
      this->Scope.hasFastStreaming = true;
      break;

    case MODEL_PS2203:
      this->Scope.model = MODEL_PS2203;
      this->Scope.firstRange = PS2000_50MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = false;
      this->Scope.hasSignalGenerator = true;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = true;
      break;

    case MODEL_PS2204:
      this->Scope.model = MODEL_PS2204;
      this->Scope.firstRange = PS2000_50MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = true;
      this->Scope.hasSignalGenerator = true;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = true;
      break;

    case MODEL_PS2204A:
      this->Scope.model = MODEL_PS2204A;
      this->Scope.firstRange = PS2000_50MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = true;
      this->Scope.hasSignalGenerator = true;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = true;
      this->Scope.awgBufferSize = 4096;
      break;

    case MODEL_PS2205:
      this->Scope.model = MODEL_PS2205;
      this->Scope.firstRange = PS2000_50MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = true;
      this->Scope.hasSignalGenerator = true;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = true;
      break;

    case MODEL_PS2205A:
      this->Scope.model = MODEL_PS2205A;
      this->Scope.firstRange = PS2000_50MV;
      this->Scope.lastRange = PS2000_20V;
      this->Scope.maxTimebase = PS2200_MAX_TIMEBASE;
      this->Scope.timebases = this->Scope.maxTimebase;
      this->Scope.noOfChannels = 2;
      this->Scope.hasAdvancedTriggering = true;
      this->Scope.hasSignalGenerator = true;
      this->Scope.hasEts = true;
      this->Scope.hasFastStreaming = true;
      this->Scope.awgBufferSize = 4096;
      break;

    default:
      LOG_ERROR("PicoScope model not supported.");
    }
  }
  else
  {
    LOG_ERROR("Failed to open PicoScope device.");

    ps2000_get_unit_info(this->Scope.handle, line, sizeof(line), 5);

    deviceInfoSS << description[5] << " " << line;
    this->Scope.model = MODEL_NONE;
    this->Scope.firstRange = PS2000_100MV;
    this->Scope.lastRange = PS2000_20V;
    this->Scope.timebases = PS2105_MAX_TIMEBASE;
    this->Scope.noOfChannels = 1;
  }

  return deviceInfoSS.str();
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::vtkInternal::RangeIntToEnum(const int voltageRangeMv, PS2000_RANGE& voltRange)
{
  switch (voltageRangeMv)
  {
  case 10:
		voltRange = PS2000_10MV; return PLUS_SUCCESS;
  case 20:
		voltRange = PS2000_20MV; return PLUS_SUCCESS;
  case 50:
		voltRange = PS2000_50MV; return PLUS_SUCCESS;
  case 100:
		voltRange = PS2000_100MV; return PLUS_SUCCESS;
  case 200:
		voltRange = PS2000_200MV; return PLUS_SUCCESS;
  case 500:
		voltRange = PS2000_500MV; return PLUS_SUCCESS;
  case 1000:
		voltRange = PS2000_1V; return PLUS_SUCCESS;
  case 2000:
		voltRange = PS2000_2V; return PLUS_SUCCESS;
  case 5000:
		voltRange = PS2000_5V; return PLUS_SUCCESS;
  case 10000:
		voltRange = PS2000_10V; return PLUS_SUCCESS;
  case 20000:
		voltRange = PS2000_20V; return PLUS_SUCCESS;
  case 50000:
		voltRange = PS2000_50V; return PLUS_SUCCESS;
  default:
    LOG_ERROR("Invalid voltage range: " << voltageRangeMv << ". Ensure you are providing the voltage range in mV.");
		return PLUS_FAIL;
  }
}

//-------------------------------------------------------------------------------------------------
int vtkPlusPicoScopeDataSource::vtkInternal::RangeEnumToInt(const PS2000_RANGE voltRange)
{
	switch (voltRange)
	{
	case PS2000_10MV:
		return 10;
	case PS2000_20MV:
		return 20;
	case PS2000_50MV:
		return 50;
	case PS2000_100MV:
		return 100;
	case PS2000_200MV:
		return 200;
	case PS2000_500MV:
		return 500;
	case PS2000_1V:
		return 1000;
	case PS2000_2V:
		return 2000;
	case PS2000_5V:
		return 5000;
	case PS2000_10V:
		return 10000;
	case PS2000_20V:
		return 20000;
	case PS2000_50V:
		return 50000;
	}
}

//-------------------------------------------------------------------------------------------------
std::string vtkPlusPicoScopeDataSource::vtkInternal::RangeEnumToStr(const PS2000_RANGE voltRange)
{
	switch (voltRange)
	{
	case PS2000_10MV:
		return "PS2000_10MV";
	case PS2000_20MV:
		return "PS2000_20MV";
	case PS2000_50MV:
		return "PS2000_50MV";
	case PS2000_100MV:
		return "PS2000_100MV";
	case PS2000_200MV:
		return "PS2000_200MV";
	case PS2000_500MV:
		return "PS2000_500MV";
	case PS2000_1V:
		return "PS2000_1V";
	case PS2000_2V:
		return "PS2000_2V";
	case PS2000_5V:
		return "PS2000_5V";
	case PS2000_10V:
		return "PS2000_10V";
	case PS2000_20V:
		return "PS2000_20V";
	case PS2000_50V:
		return "PS2000_20V";
	}
}

//-------------------------------------------------------------------------------------------------
std::string vtkPlusPicoScopeDataSource::vtkInternal::GetAdcUnits(int16_t time_units)
{
  time_units++;
  //printf ( "time unit:  %d\n", time_units ) ;
  switch (time_units)
  {
  case 0:
    return "ADC";
  case 1:
    return "fs";
  case 2:
    return "ps";
  case 3:
    return "ns";
  case 4:
    return "us";
  case 5:
    return "ms";
  }

  return "Not Known";
}

//-------------------------------------------------------------------------------------------------
float vtkPlusPicoScopeDataSource::vtkInternal::AdcToMv(int32_t rawCount, int32_t rangeMv)
{
	return ((float) (rawCount * rangeMv)) / (PS2000_MAX_VALUE - 1);
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::vtkInternal::SetupPicoScopeChannel(vtkInternal::PICO_CHANNEL_SETTINGS settings)
{
  // check this channel is enabled
  if (!settings.enabled)
  {
    return PLUS_SUCCESS;
  }

  // check channel is valid
  if (!(settings.channelNumber == 0 || (settings.channelNumber == 1 && this->Scope.noOfChannels == 2)))
  {
    LOG_ERROR("Unsupported channel number: " << settings.channelNumber << ". Check the number of channels your oscilloscope supports. Note: channel numbers start at 0.");
    return PLUS_FAIL;
  }
  
  // set channel values
  if (!ps2000_set_channel(this->Scope.handle, settings.channelNumber, TRUE, settings.coupling, settings.voltageRange))
  {
    LOG_ERROR("Failed to set PicoScope channel settings for channel: " << settings.channelNumber << ".");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
void vtkPlusPicoScopeDataSource::vtkInternal::PrintChannelInfo(PICO_CHANNEL_SETTINGS channelSettings)
{
	if (channelSettings.channelNumber == 0)
	{
		LOG_INFO("CHANNEL A Settings:");
	}
	else if (channelSettings.channelNumber == 1)
	{
		LOG_INFO("CHANNEL B Settings:");
	}
	LOG_INFO("Enabled: " << (channelSettings.enabled ? "TRUE" : "FALSE"));
	LOG_INFO("Coupling: " << ((channelSettings.coupling == vtkInternal::DC) ? "DC" : "AC"));
	LOG_INFO("Voltage Range: " << this->RangeEnumToStr(channelSettings.voltageRange));
}

//-------------------------------------------------------------------------------------------------
vtkPlusPicoScopeDataSource::vtkPlusPicoScopeDataSource()
  : vtkPlusDevice()
  , Internal(new vtkInternal(this))
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::vtkPlusPicoScopeDataSource()");

  this->FrameNumber = 0;
  this->StartThreadForInternalUpdates = true;
  this->InternalUpdateRate = 30;
}

//-------------------------------------------------------------------------------------------------
vtkPlusPicoScopeDataSource::~vtkPlusPicoScopeDataSource()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::~vtkPlusPicoScopeDataSource()");

  delete Internal;
  Internal = nullptr;
}

//-------------------------------------------------------------------------------------------------
void vtkPlusPicoScopeDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::ReadConfiguration");

  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

	XML_READ_SCALAR_ATTRIBUTE_REQUIRED(int, NumberOfSamples, deviceConfig);
	if (this->NumberOfSamples > MAX_NUM_SAMPLES_PER_FRAME)
	{
		LOG_ERROR("Requested number of samples (" << this->NumberOfSamples << "( is larger than the maximum number of samples per frame (" << MAX_NUM_SAMPLES_PER_FRAME << ").");
		return PLUS_FAIL;
	}

  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); nestedElementIndex++)
  {
    vtkXMLDataElement* channelDataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(channelDataElement->GetName(), "DataSource") != 0)
    {
      // if this is not a data source element, skip it
      continue;
    }
    if (channelDataElement->GetAttribute("Type") != NULL && STRCASECMP(channelDataElement->GetAttribute("Type"), "Video") != 0)
    {
      // if this is not a Video element, skip it, we use the image to store the scope values
      continue;
    }
    std::string sourceId(channelDataElement->GetAttribute("Id"));
    if (sourceId.empty())
    {
      // Channel doesn't have ID needed to generate signal output
      LOG_ERROR("Failed to initialize PicoScope channel: DataSource Id is missing.");
      continue;
    }

	// set output source id
	this->Internal->PlusSourceId = sourceId;

	// read Channel A settings
	vtkInternal::COUPLING chA_coupling = vtkInternal::DC;
	XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_REQUIRED(ChACoupling, chA_coupling, channelDataElement, "AC", vtkInternal::AC, "DC", vtkInternal::DC);
	int chA_voltageRangeMv;
	XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, ChAVoltageRangeMv, chA_voltageRangeMv, channelDataElement);
	PS2000_RANGE chA_voltageRange;
	if (this->Internal->RangeIntToEnum(chA_voltageRangeMv, chA_voltageRange) != PLUS_SUCCESS)
	{
		return PLUS_FAIL;
	}
	this->Internal->ChannelA.enabled = true;
	this->Internal->ChannelA.coupling = chA_coupling;
	this->Internal->ChannelA.voltageRange = chA_voltageRange;

	// read Channel B settings
	vtkInternal::COUPLING chB_coupling = vtkInternal::DC;
	XML_READ_ENUM2_ATTRIBUTE_NONMEMBER_REQUIRED(ChBCoupling, chB_coupling, channelDataElement, "AC", vtkInternal::AC, "DC", vtkInternal::DC);
	int chB_voltageRangeMv;
	XML_READ_SCALAR_ATTRIBUTE_NONMEMBER_REQUIRED(int, ChBVoltageRangeMv, chB_voltageRangeMv, channelDataElement);
	PS2000_RANGE chB_voltageRange;
	if (this->Internal->RangeIntToEnum(chB_voltageRangeMv, chB_voltageRange) != PLUS_SUCCESS)
	{
		return PLUS_FAIL;
	}
	this->Internal->ChannelB.enabled = true;
	this->Internal->ChannelB.coupling = chB_coupling;
	this->Internal->ChannelB.voltageRange = chB_voltageRange;
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::WriteConfiguration");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::Probe()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::Probe");
	if (!this->Internal->Scope.handle)
	{
		return PLUS_FAIL;
	}
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::InternalConnect()
{
	LOG_TRACE("vtkPlusPicoScopeDataSource::InternalConnect");

	// connect to scope
	this->Internal->Scope.handle = ps2000_open_unit();

	if (!this->Internal->Scope.handle)
	{
		// scope is not connected
		LOG_ERROR("Failed to connect to PicoScope PS2000.");
		return PLUS_FAIL;
	}

	// print device info
	std::string psDeviceInfo;
	psDeviceInfo += this->Internal->GetPicoScopeInfo();
	LOG_INFO("PicoScope device info: " << std::endl << psDeviceInfo);

	// print channel info
	LOG_INFO("");
	this->Internal->PrintChannelInfo(this->Internal->ChannelA);
	LOG_INFO("");
	this->Internal->PrintChannelInfo(this->Internal->ChannelB);

	// configure channels
	if (this->Internal->SetupPicoScopeChannel(this->Internal->ChannelA) != PLUS_SUCCESS)
	{
		return PLUS_FAIL;
	}
	if (this->Internal->SetupPicoScopeChannel(this->Internal->ChannelB) != PLUS_SUCCESS)
	{
		return PLUS_FAIL;
	}

	// disable PicoScope trigger
	ps2000_set_trigger(this->Internal->Scope.handle, PS2000_NONE, 0, PS2000_RISING, 0, 0);

	// disable ETS (equivalent time sampling) mode
	ps2000_set_ets(this->Internal->Scope.handle, PS2000_ETS_OFF, 0, 0);

	/*  Find the maximum number of samples, the time interval (in time_units),
	*   the most suitable time units, and the maximum oversample at the current timebase
	*/

	bool timebaseSelected = false;
	int16_t timebase;
	int32_t timeInterval;
	int16_t timeUnits;
	int16_t oversample;
	int32_t maxSamples;
	for (timebase = 0; timebase <= this->Internal->Scope.maxTimebase; ++timebase)
	{
		int16_t timebaseResult = ps2000_get_timebase(
			this->Internal->Scope.handle,
			timebase,
			this->NumberOfSamples,
			&timeInterval,
			&timeUnits,
			0, // no oversampling
			&maxSamples
		);

		if (timeInterval == NULL || timeUnits == NULL || maxSamples == NULL)
		{
			continue;
		}

		if (timebaseResult != 0 && maxSamples > this->NumberOfSamples)
		{
			timebaseSelected = true;
			this->Internal->AppliedNumberOfSamples = maxSamples;
			this->Internal->AppliedTimeUnits = timeUnits;
			this->Internal->AppliedTimebase = timebase;
			break;
		}
	}

	if (!timebaseSelected)
	{
		LOG_ERROR("Failed to select PicoScope timebase. Please choose a different number of samples.");
		return PLUS_FAIL;
	}

	// configure the output source
	this->GetVideoSource(this->Internal->PlusSourceId.c_str(), this->Internal->PlusSource);
	if (this->Internal->PlusSource->GetNumberOfItems() == 0)
	{
		this - Internal->PlusSource->SetInputImageOrientation(US_IMG_ORIENT_MF);
		this->Internal->PlusSource->SetImageType(US_IMG_BRIGHTNESS);
		this->Internal->PlusSource->SetPixelType(VTK_FLOAT);
		this->Internal->PlusSource->SetNumberOfScalarComponents(1);
		this->Internal->PlusSource->SetInputFrameSize(this->NumberOfSamples, 3, 1);
	}

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::InternalDisconnect");
  ps2000_close_unit(this->Internal->Scope.handle);
  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::InternalStartRecording()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::InternalStartRecording");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::InternalStopRecording()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::InternalStopRecording");

  return PLUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
PlusStatus vtkPlusPicoScopeDataSource::InternalUpdate()
{
  LOG_TRACE("vtkPlusPicoScopeDataSource::InternalUpdate");
  
  // Start scope collecting
	int32_t timeIndisposedMs;
  ps2000_run_block(
		this->Internal->Scope.handle,
		this->Internal->AppliedNumberOfSamples,
		this->Internal->AppliedTimebase,
		0, // no oversampling
		&timeIndisposedMs
	);

  while (!ps2000_ready(this->Internal->Scope.handle))
  {
		// no fresh data yet
  }

	const double unfilteredTimestamp = vtkIGSIOAccurateTimer::GetSystemTime();

  ps2000_stop(this->Internal->Scope.handle);

	int16_t overflow;
  ps2000_get_times_and_values(
    this->Internal->Scope.handle,
    this->Internal->Times,
    this->Internal->SignalARaw,
    this->Internal->SignalBRaw,
    NULL,
    NULL,
    &overflow,
		this->Internal->AppliedTimeUnits,
		this->Internal->AppliedNumberOfSamples
	);
	
	if (overflow != 0)
	{
		LOG_WARNING("PicoScope overflow occurred in InternalUpdate.");
	}

	int channelARangeMv = this->Internal->RangeEnumToInt(this->Internal->ChannelA.voltageRange);
	int channelBRangeMv = this->Internal->RangeEnumToInt(this->Internal->ChannelB.voltageRange);

	// update buffers
	for (int i = 0; i < this->NumberOfSamples; i++)
	{
		this->Internal->SignalBuffer[i] = (float)this->Internal->Times[i];
		this->Internal->SignalBuffer[i + this->NumberOfSamples] = this->Internal->AdcToMv(this->Internal->SignalARaw[i], channelARangeMv);
		this->Internal->SignalBuffer[i + 2*this->NumberOfSamples] = this->Internal->AdcToMv(this->Internal->SignalBRaw[i], channelBRangeMv);
	}

	vtkPlusDataSource* source;
	if (this->GetVideoSource(this->Internal->PlusSourceId.c_str(), source) != PLUS_SUCCESS)
	{
		LOG_ERROR("couldnt find source");
	}

	FrameSizeType frameSize = { this->NumberOfSamples, 3, 1 };
	PlusStatus res = source->AddItem(
		this->Internal->SignalBuffer,
		US_IMG_ORIENT_MF,
		frameSize,
		VTK_FLOAT,
		1,
		US_IMG_BRIGHTNESS,
		0,
		this->FrameNumber,
		unfilteredTimestamp
	);

  this->FrameNumber++;
 
  return PLUS_FAIL;
}