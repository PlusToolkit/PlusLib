/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkPlusGenericSensorTracker.h"

// Local includes
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

// Windows sensor api includes
#include <SensorsApi.h>
#include <Sensors.h>
#include <PortableDeviceTypes.h>
#include <stringapiset.h>

// Stl includes
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <array>
#include <algorithm>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusGenericSensorTracker);

namespace
{
  //
  // C++ memory management helpers
  //

  template <typename T>
  struct DefaultDeletor
  {
    void
    operator()(T* t)
    {
      if (t)
      {
        t->Release();
      }
    }
  };

  template <typename T>
  using UniquePtrType = std::unique_ptr<T, DefaultDeletor<T>>;

  template <typename T>
  auto createSafePointer(T* ptr)
  {
    UniquePtrType<T> safePtr;
    safePtr.reset(ptr);

    return safePtr;
  }

  // Rely on the fact the compiler is not allowed to optimize away
  // an automatic object whose dtors or ctors has side effects
  struct SafeComInitializer
  {
    SafeComInitializer()
    {
      CoInitialize(nullptr);
    }
    ~SafeComInitializer()
    {
      CoUninitialize();
    }
  };

  //
  // Base internal types
  //

  enum class SensorType
  {
    Accelerometer = 0,
    Gyrometer,
    Magnetometer // Enable in the future
  };

  struct SensorStreamConfig
  {
    SensorType Type;
    std::wstring FriendlyName;
    std::wstring SerialNumber;
    std::wstring Manufacturer;
    std::wstring Description;
    bool TrackerTimeToSystemTimeComputed{false};
    double TrackerTimeToSystemTimeSec{};
    UniquePtrType<ISensor> SensorHandle{nullptr};
    IStream* UpdateThreadSensorMarshallingStream{nullptr};
    UniquePtrType<ISensor> UpdateThreadSensorRef{nullptr};
    vtkPlusDataSource* Tool{nullptr};
  };

  struct SensorUserConfig
  {
    std::string ToolId;
    std::string SerialNumber;
  };

  //
  // Windows API helpers
  //

  std::string GetLastErrorAsString()
  {
    // Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
    {
      return std::string("No error message available");
    }

    LPSTR messageBuffer{nullptr};
    auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
  }

  uint64_t GetSystemTimeInMs(SYSTEMTIME* systemTime)
  {
    FILETIME filetime;
    SystemTimeToFileTime(systemTime, &filetime);

    return (static_cast<uint64_t>(filetime.dwLowDateTime) + (static_cast<uint64_t>(filetime.dwHighDateTime) << 32LL)) * 100 / 1000000;
  }

  std::string ConvertWideStr2Str(const std::wstring& wstr)
  {
    auto size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> buf(size);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &buf[0], size, nullptr, nullptr);
    return std::string(&buf[0]);
  }

  //
  // Type conversion
  //

  REFSENSOR_TYPE_ID ConvertCustomTypeToWindowsType(SensorType type)
  {
    switch (type)
    {
      case SensorType::Accelerometer:
        return SENSOR_TYPE_ACCELEROMETER_3D;
      case SensorType::Gyrometer:
        return SENSOR_TYPE_GYROMETER_3D;
      case SensorType::Magnetometer:
        return SENSOR_TYPE_COMPASS_3D;
      default:
        break;
    }

    return SENSOR_TYPE_UNKNOWN;
  }

  std::string ToString(SensorType type)
  {
    switch (type)
    {
      case SensorType::Accelerometer:
        return "Accelerometer";
      case SensorType::Gyrometer:
        return "Gyrometer";
      case SensorType::Magnetometer:
        return "Magnetometer";
      default:
        break;
    }

    return "";
  }

  std::string ToString(SensorState state)
  {
    switch (state)
    {
      case SensorState::SENSOR_STATE_READY:
        return "Ready to send sensor data";
      case SensorState::SENSOR_STATE_NOT_AVAILABLE:
        return "The sensor is not available for use";
      case SensorState::SENSOR_STATE_NO_DATA:
        return "The sensor is available but does not have data";
      case SensorState::SENSOR_STATE_INITIALIZING:
        return "The sensor is available, but performing initialization. Try again later.";
      case SensorState::SENSOR_STATE_ACCESS_DENIED:
        return "The sensor is available, but the user account does not have permission to access the sensor data";
      case SensorState::SENSOR_STATE_ERROR:
        return "The sensor has raised an error";
      default:
        break;
    }

    return "";
  }
}

//----------------------------------------------------------------------------
class vtkPlusGenericSensorTracker::vtkInternal
{
public:
  vtkPlusGenericSensorTracker* External;
  UniquePtrType<ISensorManager> SensorManager{nullptr};
  std::vector<SensorStreamConfig> SensorStreams;
  std::vector<SensorUserConfig> SensorUserConfigs;
  bool UseReportedTimestamp{false};

  vtkInternal(vtkPlusGenericSensorTracker* external)
    : External(external)
  {
  }

  vtkInternal::~vtkInternal()
  {
    this->ReleaseStreams();
    this->ReleaseManager();
  }

  PlusStatus RetrieveSensorManager()
  {
    // Initializing the appartment with COINIT_MULTITHREADED is useless
    // as the calling thread might be of type APTTYPE_MAINSTA. Thus,
    // we rely later on COM interface inter-thread marshalling
    CoInitialize(nullptr);

    ISensorManager* sensorManager{nullptr};
    auto hr = CoCreateInstance(CLSID_SensorManager,
                               nullptr, CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS(&sensorManager));

    if (hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DISABLED_BY_POLICY))
    {
      // Unable to retrieve sensor manager due to
      // group policy settings. Alert the user.
      LOG_ERROR("Group policiy settings prevent access to sensor manager: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve sensor manager: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    this->SensorManager.reset(sensorManager);
    return PLUS_SUCCESS;
  }

  PlusStatus PopulateStreams()
  {
    std::set<SensorType> sensorTypes = {SensorType::Accelerometer, SensorType::Gyrometer};

    for (const auto& sensorType : sensorTypes)
    {
      vtkPlusDataSource* tool{nullptr};
      this->External->GetToolByPortName(ToString(sensorType), tool);

      if (tool)
      {
        SensorStreamConfig config;
        config.Type = sensorType;
        config.Tool = tool;
        this->SensorStreams.emplace_back(std::move(config));
      }
    }

    if (this->SensorStreams.empty())
    {
      LOG_ERROR("No data source available: required at least one Accelerometer or Gyrometer source");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  PlusStatus InitializeStream(SensorStreamConfig& sensorStream)
  {
    ISensorCollection* collection{nullptr};
    auto hr = this->SensorManager->GetSensorsByType(ConvertCustomTypeToWindowsType(sensorStream.Type), &collection);

    LOG_INFO("Initializing stream for sensor of type: " << ToString(sensorStream.Type));

    if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
    {
      LOG_ERROR("No sensor found");
      return PLUS_FAIL;
    }

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve sensor collection: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    auto safeCollection = createSafePointer(collection);

    ULONG collectionSize{};
    hr = safeCollection->GetCount(&collectionSize);
    if (FAILED(hr) || collectionSize == 0)
    {
      LOG_ERROR("Failed to retrieve sensor: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    auto userConfig = std::find_if(this->SensorUserConfigs.cbegin(), this->SensorUserConfigs.cend(), [&sensorStream](const SensorUserConfig & userConfig)
    {
      auto referencedToolId = sensorStream.Tool->GetId();
      auto refIdx = referencedToolId.find("To");
      if (refIdx != std::string::npos)
      {
        referencedToolId = referencedToolId.substr(0, refIdx);
      }
      return userConfig.ToolId == referencedToolId;
    });

    if (collectionSize > 1 && userConfig == this->SensorUserConfigs.cend())
    {
      LOG_WARNING("Found more than one sensor of this type. Connecting to the first one.");
    }
    else if (userConfig != this->SensorUserConfigs.cend())
    {
      LOG_INFO("Searching for device with serial number: " << userConfig->SerialNumber);
    }

    bool found{false};
    for (ULONG i = 0; i < collectionSize; i++)
    {
      ISensor* sensor{nullptr};
      auto hr = safeCollection->GetAt(i, &sensor);
      if (FAILED(hr))
      {
        LOG_ERROR("Failed to retrieve sensor: " << GetLastErrorAsString());
        return PLUS_FAIL;
      }
      sensorStream.SensorHandle.reset(sensor);
      InitializeProperties(sensorStream);

      if ((userConfig == this->SensorUserConfigs.cend()) || ConvertWideStr2Str(sensorStream.SerialNumber) == userConfig->SerialNumber)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      LOG_ERROR("Failed to find a matching device");
      return PLUS_FAIL;
    }

    // Needs some marshalling to access com interface from another thread as we cannot ensure
    // an MTA is used
    IStream* sensorMarshallingStream{nullptr};
    hr = CoMarshalInterThreadInterfaceInStream(IID_ISensor, sensorStream.SensorHandle.get(), &sensorMarshallingStream);

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to marshal interface for multithreading: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    sensorStream.UpdateThreadSensorMarshallingStream = sensorMarshallingStream;
    sensorStream.SensorHandle->SetEventSink(nullptr);
    return CheckForSupportedData(sensorStream);
  }

  void InitializeProperties(SensorStreamConfig& sensorStream)
  {
    const PROPERTYKEY SensorProperties[] =
    {
      SENSOR_PROPERTY_MANUFACTURER,
      SENSOR_PROPERTY_SERIAL_NUMBER,
      SENSOR_PROPERTY_DESCRIPTION,
      SENSOR_PROPERTY_FRIENDLY_NAME
    };

    sensorStream.Manufacturer = L"";
    sensorStream.SerialNumber = L"";
    sensorStream.FriendlyName = L"";
    sensorStream.Description = L"";

    IPortableDeviceKeyCollection* keys{nullptr};

    LOG_DEBUG("Retrieving metadata for sensor of type: " << ToString(sensorStream.Type));

    auto hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS(&keys));

    if (FAILED(hr))
    {
      LOG_WARNING("Failed to retrieve metadata: " << GetLastErrorAsString());
      return;
    }

    auto safeKeys = createSafePointer(keys);

    for (DWORD dwIndex = 0; dwIndex < ARRAYSIZE(SensorProperties); dwIndex++)
    {
      safeKeys->Add(SensorProperties[dwIndex]);
    }

    IPortableDeviceValues* values{nullptr};
    hr = sensorStream.SensorHandle->GetProperties(safeKeys.get(), &values);

    if (FAILED(hr))
    {
      LOG_WARNING("Failed to retrieve metadata: " << GetLastErrorAsString());
      return;
    }

    auto safeValues = createSafePointer(values);

    DWORD valsCount{};
    safeValues->GetCount(&valsCount);

    PROPERTYKEY pk;
    PROPVARIANT pv = {};
    for (DWORD i = 0; i < valsCount; i++)
    {
      // Get the value at the current index.
      hr = safeValues->GetAt(i, &pk, &pv);

      if (SUCCEEDED(hr))
      {
        // Find and print the property.
        if (IsEqualPropertyKey(pk, SENSOR_PROPERTY_MANUFACTURER))
        {
          sensorStream.Manufacturer = pv.pwszVal;
        }
        else if (IsEqualPropertyKey(pk, SENSOR_PROPERTY_SERIAL_NUMBER))
        {
          sensorStream.SerialNumber = pv.pwszVal;
        }
        else if (IsEqualPropertyKey(pk, SENSOR_PROPERTY_FRIENDLY_NAME))
        {
          sensorStream.FriendlyName = pv.pwszVal;
        }
        else if (IsEqualPropertyKey(pk, SENSOR_PROPERTY_DESCRIPTION))
        {
          sensorStream.Description = pv.pwszVal;
        }
      }

      PropVariantClear(&pv);
    }

    LOG_DEBUG("Manufacturer: " << ConvertWideStr2Str(sensorStream.Manufacturer));
    LOG_DEBUG("Serial Number: " << ConvertWideStr2Str(sensorStream.SerialNumber));
    LOG_DEBUG("FriendlyName: " << ConvertWideStr2Str(sensorStream.FriendlyName));
    LOG_DEBUG("Description: " << ConvertWideStr2Str(sensorStream.Description));
  }

  PlusStatus
  CheckForSupportedData(SensorStreamConfig& sensorStream)
  {
    LOG_INFO("Checking for supported data for sensor of type: " << ToString(sensorStream.Type));

    IPortableDeviceKeyCollection* keys{nullptr};
    auto hr = CoCreateInstance(CLSID_PortableDeviceKeyCollection,
                               NULL,
                               CLSCTX_INPROC_SERVER,
                               IID_PPV_ARGS(&keys));

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve supported data: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    hr = sensorStream.SensorHandle->GetSupportedDataFields(&keys);
    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve supported data: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    auto safeKeys = createSafePointer(keys);

    DWORD keysCount{};
    hr = safeKeys->GetCount(&keysCount);
    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve supported data: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    if (sensorStream.Type == SensorType::Accelerometer && !CheckForSupportedDataAcc(safeKeys.get(), keysCount))
    {
      LOG_ERROR("The accelerometer sensor cannot provide the expected data");
      return PLUS_FAIL;
    }
    else if (sensorStream.Type == SensorType::Gyrometer && !CheckForSupportedDataGyr(safeKeys.get(), keysCount))
    {
      LOG_ERROR("The gyrometer sensor cannot provide the expected data");
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS;
  }

  void ReleaseStreams()
  {
    for (auto& sensorStream : this->SensorStreams)
    {
      if (sensorStream.UpdateThreadSensorRef)
      {
        sensorStream.UpdateThreadSensorRef.reset(nullptr);
      }
      else if (sensorStream.UpdateThreadSensorMarshallingStream)
      {
        sensorStream.UpdateThreadSensorMarshallingStream->Release();
      }
      sensorStream.SensorHandle.reset(nullptr);
    }
  }

  void ReleaseManager()
  {
    this->SensorManager.reset(nullptr);
    CoUninitialize();
  }

  PlusStatus CheckSensorState(const SensorType& sensorType, ISensor* sensorHandle)
  {
    SensorState state;
    auto hr = sensorHandle->GetState(&state);

    LOG_TRACE("Retrieving state for sensor of type: " << ToString(sensorType));

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve state for sensor: " << GetLastErrorAsString());
      return PLUS_FAIL;
    }

    if (state == SensorState::SENSOR_STATE_READY || state == SensorState::SENSOR_STATE_NO_DATA)
    {
      LOG_TRACE("Sensor state: " << ToString(state));
    }
    else
    {
      LOG_ERROR("Sensor is not ready: " << ToString(state));
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //
  // Accelerometer specific callback
  //

  bool CheckForSupportedDataAcc(IPortableDeviceKeyCollection* keys, DWORD keysCount)
  {
    PROPERTYKEY pk;
    std::bitset<3> supported = 0b000;

    for (DWORD i = 0; i < keysCount; i++)
    {
      auto hr = keys->GetAt(i, &pk);

      if (FAILED(hr))
      {
        continue;
      }

      if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ACCELERATION_X_G))
      {
        supported[0] = 1;
      }
      else if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ACCELERATION_Y_G))
      {
        supported[1] = 1;
      }
      else if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ACCELERATION_Z_G))
      {
        supported[2] = 1;
      }
    }

    return supported.all();
  }

  static PlusStatus RetrieveAccData(ISensorDataReport* report, vtkSmartPointer<vtkMatrix4x4> matrix)
  {
    std::array<const PROPERTYKEY, 3> fields = {SENSOR_DATA_TYPE_ACCELERATION_X_G, SENSOR_DATA_TYPE_ACCELERATION_Y_G, SENSOR_DATA_TYPE_ACCELERATION_Z_G};
    matrix->Identity();
    for (int i = 0; i < 3; ++i)
    {
      PROPVARIANT var = {};
      auto hr = report->GetSensorValue(fields[i], &var);

      if (!SUCCEEDED(hr) || var.vt != VT_R8)
      {
        PropVariantClear(&var);
        LOG_ERROR("Failed to retrieve acceleration in " << ((i == 0) ? "x" : ((i == 1) ? "y" : "z")));
        return PLUS_FAIL;
      }

      matrix->SetElement(i, 3, var.dblVal);
      LOG_TRACE("Retrieve acceleration component " << ((i == 0) ? "x" : ((i == 1) ? "y" : "z")) << ": " << var.dblVal);
      PropVariantClear(&var);
    }

    return PLUS_SUCCESS;
  }

  //
  // Gyrometer specific callback
  //

  bool CheckForSupportedDataGyr(IPortableDeviceKeyCollection* keys, DWORD keysCount)
  {
    PROPERTYKEY pk;
    std::bitset<3> supported = 0b000;

    for (DWORD i = 0; i < keysCount; i++)
    {
      auto hr = keys->GetAt(i, &pk);

      if (FAILED(hr))
      {
        continue;
      }

      if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ANGULAR_VELOCITY_X_DEGREES_PER_SECOND))
      {
        supported[0] = 1;
      }
      else if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Y_DEGREES_PER_SECOND))
      {
        supported[1] = 1;
      }
      else if (IsEqualPropertyKey(pk, SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Z_DEGREES_PER_SECOND))
      {
        supported[2] = 1;
      }
    }

    return supported.all();
  }

  static PlusStatus RetrieveGyrData(ISensorDataReport* report, vtkSmartPointer<vtkMatrix4x4> matrix)
  {
    std::array<const PROPERTYKEY, 3> fields = {SENSOR_DATA_TYPE_ANGULAR_VELOCITY_X_DEGREES_PER_SECOND, SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Y_DEGREES_PER_SECOND, SENSOR_DATA_TYPE_ANGULAR_VELOCITY_Z_DEGREES_PER_SECOND};
    matrix->Identity();
    for (int i = 0; i < 3; ++i)
    {
      PROPVARIANT var = {};
      auto hr = report->GetSensorValue(fields[i], &var);

      if (!SUCCEEDED(hr) || var.vt != VT_R8)
      {
        PropVariantClear(&var);
        LOG_ERROR("Failed to retrieve angular velocity in " << ((i == 0) ? "x" : ((i == 1) ? "y" : "z")));
        return PLUS_FAIL;
      }

      matrix->SetElement(i, 3, var.dblVal);
      LOG_TRACE("Retrieve angular velocity component " << ((i == 0) ? "x" : ((i == 1) ? "y" : "z")) << ": " << var.dblVal);
      PropVariantClear(&var);
    }

    return PLUS_SUCCESS;
  }

  //
  // TODO: Compass, etc. implementations
  //
};

//----------------------------------------------------------------------------
vtkPlusGenericSensorTracker::vtkPlusGenericSensorTracker()
  : Internal(new vtkInternal(this))
{
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkPlusGenericSensorTracker::~vtkPlusGenericSensorTracker()
{
  delete this->Internal;
  this->Internal = nullptr;
}

//----------------------------------------------------------------------------
void vtkPlusGenericSensorTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Generic Sensor Configuration:" << std::endl;
  os << indent << "UseReportedTimestamp: " << this->Internal->UseReportedTimestamp << std::endl;
  for (auto& sensorStream : this->Internal->SensorStreams)
  {
    os << "Sensor type: " << ToString(sensorStream.Type);
    os << indent << "Serial Number: " << ConvertWideStr2Str(sensorStream.SerialNumber);
    os << indent << "Serial Manufaturer: " << ConvertWideStr2Str(sensorStream.Manufacturer);
    os << indent << "Serial Friendly Name: " << ConvertWideStr2Str(sensorStream.FriendlyName);
    os << indent << "Serial Description: " << ConvertWideStr2Str(sensorStream.Description);
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);
  XML_READ_BOOL_ATTRIBUTE_NONMEMBER_OPTIONAL(UseReportedTimestamp, this->Internal->UseReportedTimestamp, deviceConfig);
  XML_FIND_NESTED_ELEMENT_REQUIRED(dataSourcesElement, deviceConfig, "DataSources");

  LOG_TRACE("Reading custom configuration fields in " << dataSourcesElement->GetNumberOfNestedElements() << " nested elements");
  for (int nestedElementIndex = 0; nestedElementIndex < dataSourcesElement->GetNumberOfNestedElements(); ++nestedElementIndex)
  {
    vtkXMLDataElement* dataElement = dataSourcesElement->GetNestedElement(nestedElementIndex);
    if (STRCASECMP(dataElement->GetName(), "DataSource") != 0)
    {
      continue;
    }

    LOG_TRACE("Found a new data source");

    if (dataElement->GetAttribute("Type") != NULL && STRCASECMP(dataElement->GetAttribute("Type"), "Tool") == 0)
    {
      const char* toolId = dataElement->GetAttribute("Id");
      if (toolId == nullptr)
      {
        // tool doesn't have ID needed to generate transform
        LOG_ERROR("Failed to initialize tool: Id is missing");
        continue;
      }

      LOG_TRACE("Data source name: " << toolId);
      SensorUserConfig config;
      config.ToolId = toolId;
      XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(SerialNumber, config.SerialNumber, dataElement);

      if (!config.SerialNumber.empty())
      {
        this->Internal->SensorUserConfigs.emplace_back(config);
      }
    }
    else
    {
      LOG_ERROR("DataSource with unknown Type.");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::InternalConnect()
{
  if (this->Internal->PopulateStreams() != PLUS_SUCCESS ||
      this->Internal->RetrieveSensorManager() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::InternalDisconnect()
{
  this->Internal->ReleaseManager();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::Probe()
{
  for (const auto& sensorStream : this->Internal->SensorStreams)
  {
    if (this->Internal->CheckSensorState(sensorStream.Type, sensorStream.SensorHandle.get()) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::InternalStartRecording()
{
  for (auto& sensorStream : this->Internal->SensorStreams)
  {
    if (this->Internal->InitializeStream(sensorStream) != PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
  }

  this->FrameNumber = 0;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::InternalStopRecording()
{
  this->Internal->ReleaseStreams();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusGenericSensorTracker::InternalUpdate()
{
  static SafeComInitializer safeComInitializer;
  for (auto& sensorStream : this->Internal->SensorStreams)
  {
    LOG_TRACE("Retrieving sensor data for sensor of type: " << ToString(sensorStream.Type));

    if (!sensorStream.UpdateThreadSensorRef)
    {
      ISensor* sensorRef{nullptr};
      auto hr = CoGetInterfaceAndReleaseStream(sensorStream.UpdateThreadSensorMarshallingStream, IID_ISensor, (LPVOID*)&sensorRef);

      if (FAILED(hr))
      {
        LOG_ERROR("Failed to retrieve sensor handle: " << GetLastErrorAsString());
        return PLUS_FAIL;
      }

      sensorStream.UpdateThreadSensorRef.reset(sensorRef);
      sensorStream.UpdateThreadSensorMarshallingStream = nullptr;
    }

    if (Internal->CheckSensorState(sensorStream.Type, sensorStream.UpdateThreadSensorRef.get()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Sensor state not valid");
      return PLUS_FAIL;
    }

    ISensorDataReport* report{nullptr};
    auto hr = sensorStream.UpdateThreadSensorRef->GetData(&report);

    if (hr == HRESULT_FROM_WIN32(ERROR_NO_DATA))
    {
      LOG_ERROR("No data is available yet");
      continue;
    }

    if (FAILED(hr))
    {
      LOG_ERROR("Failed to retrieve data: " << GetLastErrorAsString() << hr);
      return PLUS_FAIL;
    }

    auto safeReport = createSafePointer(report);

    vtkNew<vtkMatrix4x4> transform;
    if (sensorStream.Type == SensorType::Accelerometer)
    {
      if (vtkInternal::RetrieveAccData(safeReport.get(), transform) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get accelerometer data: " << GetLastErrorAsString());
        return PLUS_FAIL;
      }
    }
    else if (sensorStream.Type == SensorType::Gyrometer)
    {
      if (vtkInternal::RetrieveGyrData(safeReport.get(), transform) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to get gyrometer data: " << GetLastErrorAsString());
        return PLUS_FAIL;
      }
    }

    if (this->Internal->UseReportedTimestamp)
    {
      SYSTEMTIME sysTime;
      hr = safeReport->GetTimestamp(&sysTime);

      double timestamp{};
      if (FAILED(hr))
      {
        LOG_TRACE("Failed to get timestamp. Falling back on current system time.");
        timestamp = vtkIGSIOAccurateTimer::GetSystemTime();
      }
      else
      {
        auto sensorTimestampMs = GetSystemTimeInMs(&sysTime);
        LOG_TRACE("Sensor reported timestamp is ms: " << sensorTimestampMs);
        if (!sensorStream.TrackerTimeToSystemTimeComputed)
        {
          const double timeSystemSec = vtkIGSIOAccurateTimer::GetSystemTime();
          LOG_TRACE("System time in s: " << timeSystemSec);
          sensorStream.TrackerTimeToSystemTimeSec = timeSystemSec - sensorTimestampMs / 1000.0;
          sensorStream.TrackerTimeToSystemTimeComputed = true;

          LOG_TRACE("Timestamp offset for this sensor: " << sensorStream.TrackerTimeToSystemTimeSec);
        }

        timestamp = sensorTimestampMs / 1000.0 + sensorStream.TrackerTimeToSystemTimeSec;
      }

      LOG_TRACE("Final timestamp in s: " << timestamp);
      ToolTimeStampedUpdateWithoutFiltering(sensorStream.Tool->GetId(), transform, TOOL_OK, timestamp, timestamp);
    }
    else
    {
      ToolTimeStampedUpdate(sensorStream.Tool->GetId(), transform, TOOL_OK, this->FrameNumber, vtkIGSIOAccurateTimer::GetSystemTime());
    }
  }
  this->FrameNumber++;

  return PLUS_SUCCESS;
}
