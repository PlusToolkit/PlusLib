/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

// Needed for proper singleton initialization 
// The vtkDebugLeaks singleton must be initialized before and
// destroyed after the vtkPlusConfig singleton.
#include "vtkDebugLeaksManager.h"
#include "vtkObjectFactory.h"

#include "vtkPlusConfig.h"
#include "vtkRecursiveCriticalSection.h"
#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"
#include "vtkMatrix4x4.h"

static const char APPLICATION_CONFIGURATION_FILE_NAME[]="PlusConfig.xml";

vtkCxxRevisionMacro(vtkPlusConfig, "$Revision: 1.0 $");

vtkPlusConfig *vtkPlusConfig::Instance = NULL;
vtkRecursiveCriticalSection* vtkPlusConfig::CriticalSection = NULL;

//----------------------------------------------------------------------------
class vtkPlusConfigCleanup
{
public:
  inline void Use()
  {
  }

  ~vtkPlusConfigCleanup()
  {
    if( vtkPlusConfig::GetInstance() )
    {
      vtkPlusConfig::SetInstance(NULL);
    }
  }
};
static vtkPlusConfigCleanup vtkPlusConfigCleanupGlobal;

//-----------------------------------------------------------------------------
vtkPlusConfig* vtkPlusConfig::New()
{
  return vtkPlusConfig::GetInstance();
}

//-----------------------------------------------------------------------------
vtkPlusConfig* vtkPlusConfig::GetInstance()
{
  if( vtkPlusConfig::CriticalSection == NULL )
  {
    vtkPlusConfig::CriticalSection = vtkRecursiveCriticalSection::New();
  }

  if(!vtkPlusConfig::Instance) 
  {
    vtkPlusConfig::CriticalSection->Lock();

    if( vtkPlusConfig::Instance != NULL )
    {
      return vtkPlusConfig::Instance;
    }

    vtkPlusConfigCleanupGlobal.Use();

    // Need to call vtkObjectFactory::CreateInstance method because this
    // registers the class in the vtkDebugLeaks::MemoryTable.
    // Without this we would get a "Deleting unknown object" VTK warning on application exit
    // (in debug mode, with debug leak checking enabled).
    vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPlusConfig");
    if(ret)
    {
      vtkPlusConfig::Instance = static_cast<vtkPlusConfig*>(ret);
    }
    else
    {
      vtkPlusConfig::Instance = new vtkPlusConfig();   
    }
    vtkPlusConfig::CriticalSection->Unlock();
  }

  // return the instance
  return vtkPlusConfig::Instance;
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetInstance(vtkPlusConfig* instance)
{
  if (vtkPlusConfig::Instance==instance)
  {
    return;
  }
  // preferably this will be NULL
  if (vtkPlusConfig::Instance)
  {
    vtkPlusConfig::Instance->Delete();
  }
  vtkPlusConfig::Instance = instance;
  if (!instance)
  {
    vtkPlusConfig::CriticalSection->Delete();
    vtkPlusConfig::CriticalSection = NULL;
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//-----------------------------------------------------------------------------
vtkPlusConfig::vtkPlusConfig()
{
  this->DeviceSetConfigurationData = NULL;
  this->ApplicationConfigurationData = NULL;
  this->EditorApplicationExecutable = NULL;

  this->ApplicationStartTimestamp=vtkAccurateTimer::GetInstance()->GetDateAndTimeString(); 
  
  // Retrieve the program directory (where the exe file is located)
  SetProgramDirectory();

  // Load settings from PlusConfig.xml
  LoadApplicationConfiguration();
}

//-----------------------------------------------------------------------------
vtkPlusConfig::~vtkPlusConfig()
{
  this->SetDeviceSetConfigurationData(NULL);
  this->SetApplicationConfigurationData(NULL);
  this->SetEditorApplicationExecutable(NULL);
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetProgramDirectory()
{
#ifdef WIN32
  char cProgramPath[2048]={'\0'};
  GetModuleFileName ( NULL, cProgramPath, 2048 ); 
  this->ProgramDirectory=vtksys::SystemTools::GetProgramPath(cProgramPath); 
#else
  const unsigned int pathBuffSize=1000;
  char pathBuff[pathBuffSize+1];
  pathBuff[pathBuffSize]=0;    
  // linux
  if (readlink ("/proc/self/exe", pathBuff, pathBuffSize) != -1)
  {
    // linux
    std::string path=vtksys::SystemTools::CollapseFullPath(pathBuff);
    std::string dirName;
    std::string fileName;
    vtksys::SystemTools::SplitProgramPath(path.c_str(), dirName, fileName);
    this->ProgramDirectory=dirName; 
  }
  else
  {      
    // non-linux systems
    // currently simply the working directory is used instead of the executable path
    // see http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
    std::string path=vtksys::SystemTools::CollapseFullPath("./");
    LOG_WARNING("Cannot get program path. PlusConfig.xml will be read from  "<<path);
    this->ProgramDirectory=path;       
  }
#endif
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetOutputDirectory(const std::string& aDir)
{
  this->OutputDirectory = aDir;
  
  // Set log file name and path to the output directory 
  std::string logfilename=std::string(this->ApplicationStartTimestamp)+"_PlusLog.txt";
  vtkPlusLogger::Instance()->SetLogFileName(GetOutputPath(logfilename).c_str());   
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetImageDirectory(const std::string& aDir)
{
  this->ImageDirectory = aDir;
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetDeviceSetConfigurationDirectory(const std::string& aDir)
{
  this->DeviceSetConfigurationDirectory = aDir;
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetDeviceSetConfigurationFileName(const std::string& aFilePath)
{
  this->DeviceSetConfigurationFileName = aFilePath;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::LoadApplicationConfiguration()
{
  LOG_TRACE("vtkPlusConfig::LoadApplicationConfiguration");

  if (this->ProgramDirectory.empty())
  {
    LOG_ERROR("Unable to read configuration - program directory has to be set first!");
    return PLUS_FAIL;
  }
  std::string applicationConfigurationFilePath = vtksys::SystemTools::CollapseFullPath(APPLICATION_CONFIGURATION_FILE_NAME, this->ProgramDirectory.c_str());  

  bool saveNeeded = false;

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> applicationConfigurationRoot;    
  if (vtksys::SystemTools::FileExists(applicationConfigurationFilePath.c_str(), true))
  {
    applicationConfigurationRoot.TakeReference(vtkXMLUtilities::ReadElementFromFile(applicationConfigurationFilePath.c_str()));
  }
  if (applicationConfigurationRoot == NULL)
  {
    LOG_INFO("Application configuration file is not found at '" << applicationConfigurationFilePath << "' - file will be created with default values"); 
    applicationConfigurationRoot = vtkSmartPointer<vtkXMLDataElement>::New();
    applicationConfigurationRoot->SetName("PlusConfig");
    saveNeeded = true;
  }
  
  this->SetApplicationConfigurationData(applicationConfigurationRoot); 

  // Verify root element name
  if (STRCASECMP(applicationConfigurationRoot->GetName(), "PlusConfig") != 0)
  {
    LOG_ERROR("Invalid application configuration file (root XML element of the file '" << applicationConfigurationFilePath << "' should be 'PlusConfig' instead of '" << applicationConfigurationRoot->GetName() << "')");
    return PLUS_FAIL;
  }

  // Read log level
  int logLevel = 0;
  if (applicationConfigurationRoot->GetScalarAttribute("LogLevel", logLevel) )
  {
    vtkPlusLogger::Instance()->SetLogLevel(logLevel);
  }
  else
  {
    LOG_INFO("LogLevel attribute is not found - default 'Info' log level will be used");
    vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO);
    saveNeeded = true;
  }

  // Read last device set config file
  const char* lastDeviceSetConfigFile = applicationConfigurationRoot->GetAttribute("LastDeviceSetConfigurationFileName");
  if ((lastDeviceSetConfigFile != NULL) && (STRCASECMP(lastDeviceSetConfigFile, "") != 0))
  {
    this->SetDeviceSetConfigurationFileName(lastDeviceSetConfigFile);
  }
  else
  {
    LOG_DEBUG("Cannot read last used device set config file until you connect to one first");
  }

  // Read device set configuration directory
  const char* deviceSetDirectory = applicationConfigurationRoot->GetAttribute("DeviceSetConfigurationDirectory");
  if ((deviceSetDirectory != NULL) && (STRCASECMP(deviceSetDirectory, "") != 0))
  {
    this->SetDeviceSetConfigurationDirectory(deviceSetDirectory);
  }
  else
  {
    LOG_INFO("Device set configuration directory is not set - default '../config' will be used");
    this->SetDeviceSetConfigurationDirectory("../config");
    saveNeeded = true;
  }
  // Make device set configuration directory
  if (! vtksys::SystemTools::MakeDirectory(GetDeviceSetConfigurationDirectory().c_str()))
  {
    LOG_ERROR("Unable to create device set configuration directory '" << GetDeviceSetConfigurationDirectory() << "'");
    return PLUS_FAIL;
  }

  // Read editor application
  const char* editorApplicationExecutable = applicationConfigurationRoot->GetAttribute("EditorApplicationExecutable");
  if ((editorApplicationExecutable != NULL) && (STRCASECMP(editorApplicationExecutable, "") != 0))
  {
    this->SetEditorApplicationExecutable(editorApplicationExecutable);
  }
  else
  {
    LOG_INFO("Editor application executable is not set - default 'notepad.exe' will be used");
    this->SetEditorApplicationExecutable("notepad.exe");
    saveNeeded = true;
  }

  // Read output directory
  const char* outputDirectory = applicationConfigurationRoot->GetAttribute("OutputDirectory");
  if ((outputDirectory != NULL) && (STRCASECMP(outputDirectory, "") != 0))
  {
    this->SetOutputDirectory(outputDirectory);
  }
  else
  {
    LOG_INFO("Output directory is not set - default './Output' will be used");
    this->SetOutputDirectory("./Output");
    saveNeeded = true;
  }

  // Make device set configuration directory
  if (! vtksys::SystemTools::MakeDirectory(GetOutputDirectory().c_str()))
  {
    LOG_ERROR("Unable to create device set configuration directory '" << GetOutputDirectory() << "'");
    return PLUS_FAIL;
  }
  
  // Read image directory
  const char* imageDirectory = applicationConfigurationRoot->GetAttribute("ImageDirectory");
  if ((imageDirectory != NULL) && (STRCASECMP(imageDirectory, "") != 0))
  {
    this->SetImageDirectory(imageDirectory);
  }
  else
  {
    LOG_INFO("Image directory is not set - default '../data' will be used");
    this->SetImageDirectory("../data");
    saveNeeded = true;
  }

  // Read model directory
  const char* modelDirectory = applicationConfigurationRoot->GetAttribute("ModelDirectory");
  if ((modelDirectory != NULL) && (STRCASECMP(modelDirectory, "") != 0))
  {
    this->ModelDirectory=modelDirectory;
  }
  else
  {
    LOG_INFO("Model directory is not set - default '../config' will be used");
    this->ModelDirectory="../config";
    saveNeeded = true;
  }

  // Read gnuplot directory
  const char* gnuplotDirectory = applicationConfigurationRoot->GetAttribute("GnuplotDirectory");
  if ((gnuplotDirectory != NULL) && (STRCASECMP(gnuplotDirectory, "") != 0))
  {
    this->GnuplotDirectory=gnuplotDirectory;
  }
  else
  {
    LOG_INFO("Gnuplot directory is not set - default '../gnuplot' will be used");
    this->GnuplotDirectory="../gnuplot";
    saveNeeded = true;
  }

  // Read scripts directory
  const char* scriptsDirectory = applicationConfigurationRoot->GetAttribute("ScriptsDirectory");
  if ((scriptsDirectory != NULL) && (STRCASECMP(scriptsDirectory, "") != 0))
  {
    this->ScriptsDirectory=scriptsDirectory;
  }
  else
  {
    LOG_INFO("Scripts directory is not set - default '../scripts' will be used");
    this->ScriptsDirectory="../scripts";
    saveNeeded = true;
  }

  if (saveNeeded)
  {
    return SaveApplicationConfigurationToFile();
  }

  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusConfig::WriteApplicationConfiguration()
{
  LOG_TRACE("vtkPlusConfig::WriteApplicationConfiguration");

  vtkXMLDataElement* applicationConfigurationRoot = this->ApplicationConfigurationData;
  if (applicationConfigurationRoot == NULL)
  {
    // Configuration root does not exist yet, create it now
    vtkSmartPointer<vtkXMLDataElement> newApplicationConfigurationRoot = vtkSmartPointer<vtkXMLDataElement>::New();
    newApplicationConfigurationRoot->SetName("PlusConfig");
    this->SetApplicationConfigurationData(newApplicationConfigurationRoot);
    applicationConfigurationRoot=newApplicationConfigurationRoot;
  }

  // Verify root element name
  if (STRCASECMP(applicationConfigurationRoot->GetName(), "PlusConfig") != 0)
  {
    LOG_ERROR("Invalid application configuration file (root XML element should be 'PlusConfig' found '"<<applicationConfigurationRoot->GetName()<<"' instead)");
    return PLUS_FAIL;
  }

  // Save date
  applicationConfigurationRoot->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());

  // Save log level
  applicationConfigurationRoot->SetIntAttribute("LogLevel", vtkPlusLogger::Instance()->GetLogLevel());

  // Save device set directory
  applicationConfigurationRoot->SetAttribute("DeviceSetConfigurationDirectory", this->DeviceSetConfigurationDirectory.c_str());

  // Save last device set config file
  applicationConfigurationRoot->SetAttribute("LastDeviceSetConfigurationFileName", this->DeviceSetConfigurationFileName.c_str());

  // Save editor application
  applicationConfigurationRoot->SetAttribute("EditorApplicationExecutable", this->EditorApplicationExecutable);

  // Save output path
  applicationConfigurationRoot->SetAttribute("OutputDirectory", this->OutputDirectory.c_str());

  // Save image directory path
  applicationConfigurationRoot->SetAttribute("ImageDirectory", this->ImageDirectory.c_str());

  // Save model directory path
  applicationConfigurationRoot->SetAttribute("ModelDirectory", this->ModelDirectory.c_str());

  // Save gnuplot directory path
  applicationConfigurationRoot->SetAttribute("GnuplotDirectory", this->GnuplotDirectory.c_str());

  // Save scripts directory path
  applicationConfigurationRoot->SetAttribute("ScriptsDirectory", this->ScriptsDirectory.c_str());

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetNewDeviceSetConfigurationFileName()
{
  LOG_TRACE("vtkPlusConfig::GetNewDeviceSetConfigurationFileName");

  std::string resultFileName; 
  if (this->DeviceSetConfigurationFileName.empty())
  {
    LOG_WARNING("New configuration file name cannot be assembled due to absence of input configuration file name");
    resultFileName = "PlusConfiguration";
  }
  else
  {
    resultFileName = this->DeviceSetConfigurationFileName;
    resultFileName = resultFileName.substr(0, resultFileName.find(".xml"));
    resultFileName = resultFileName.substr(resultFileName.find_last_of("/\\") + 1);
  }

  // Detect if date is already in the filename and remove it if it is there
  if (resultFileName.length() > 16)
  {
    std::string possibleDate = resultFileName.substr(resultFileName.length() - 15, 15);
    std::string possibleDay = possibleDate.substr(0, 8);
    std::string possibleTime = possibleDate.substr(9, 6);
    if (atoi(possibleDay.c_str()) && atoi(possibleTime.c_str()))
    {
      resultFileName = resultFileName.substr(0, resultFileName.length() - 16);
    }
  }

  // Construct new file name with date and time
  resultFileName.append("_");
  resultFileName.append(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S"));
  resultFileName.append(".xml");

  return resultFileName;
}

//------------------------------------------------------------------------------
PlusStatus vtkPlusConfig::SaveApplicationConfigurationToFile()
{
  LOG_TRACE("vtkPlusConfig::SaveApplicationConfigurationToFile");

  if (WriteApplicationConfiguration() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to save application configuration to XML data!"); 
    return PLUS_FAIL; 
  }

  std::string applicationConfigurationFilePath = vtksys::SystemTools::CollapseFullPath(APPLICATION_CONFIGURATION_FILE_NAME, this->ProgramDirectory.c_str());  
  PlusCommon::PrintXML(applicationConfigurationFilePath.c_str(), this->ApplicationConfigurationData); 

  LOG_INFO("Application configuration file '" << applicationConfigurationFilePath << "' saved");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::WriteTransformToCoordinateDefinition(const char* aFromCoordinateFrame, const char* aToCoordinateFrame, vtkMatrix4x4* aMatrix, double aError/*=-1*/, const char* aDate/*=NULL*/)
{
  LOG_TRACE("vtkPlusConfig::WriteTransformToCoordinateDefinition(" << aFromCoordinateFrame << ", " << aToCoordinateFrame << ")");

  if ( aFromCoordinateFrame == NULL )
  {
    LOG_ERROR("Failed to write transform to CoordinateDefinitions - 'From' coordinate frame name is NULL"); 
    return PLUS_FAIL; 
  }

  if ( aToCoordinateFrame == NULL )
  {
    LOG_ERROR("Failed to write transform to CoordinateDefinitions - 'To' coordinate frame name is NULL"); 
    return PLUS_FAIL; 
  }

  if ( aMatrix == NULL )
  {
    LOG_ERROR("Failed to write transform to CoordinateDefinitions - matrix is NULL"); 
    return PLUS_FAIL; 
  }


  vtkXMLDataElement* deviceSetConfigRootElement = GetDeviceSetConfigurationData();
  if ( deviceSetConfigRootElement == NULL )
  {
    LOG_ERROR("Failed to write transform to CoordinateDefinitions - config root element is NULL"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* coordinateDefinitions = deviceSetConfigRootElement->FindNestedElementWithName("CoordinateDefinitions");
  if ( coordinateDefinitions == NULL )
  {
    vtkSmartPointer<vtkXMLDataElement> newCoordinateDefinitions = vtkSmartPointer<vtkXMLDataElement>::New(); 
    newCoordinateDefinitions->SetName("CoordinateDefinitions"); 
    deviceSetConfigRootElement->AddNestedElement(newCoordinateDefinitions); 
    coordinateDefinitions=newCoordinateDefinitions;
  }

  // Check if we already have this entry in the config file 
  vtkXMLDataElement* transformElement=NULL; 
  int nestedElementIndex(0); 
  while ( nestedElementIndex < coordinateDefinitions->GetNumberOfNestedElements() && transformElement == NULL )
  {
    vtkXMLDataElement* nestedElement = coordinateDefinitions->GetNestedElement(nestedElementIndex); 
    const char* fromAttribute = nestedElement->GetAttribute("From"); 
    const char* toAttribute = nestedElement->GetAttribute("To"); 

    if ( fromAttribute && toAttribute 
      && STRCASECMP(fromAttribute, aFromCoordinateFrame) == 0
      && STRCASECMP(toAttribute, aToCoordinateFrame) == 0 )
    {
      transformElement = nestedElement; 
    }
  }

  if ( transformElement == NULL )
  {
    vtkSmartPointer<vtkXMLDataElement> newElement=vtkSmartPointer<vtkXMLDataElement>::New();
    newElement->SetName("Transform"); 
    newElement->SetAttribute("From", aFromCoordinateFrame); 
    newElement->SetAttribute("To", aToCoordinateFrame); 
    coordinateDefinitions->AddNestedElement(newElement); 
    transformElement=newElement;    
  }

  double vectorMatrix[16]={0}; 
  vtkMatrix4x4::DeepCopy(vectorMatrix,aMatrix); 
  transformElement->SetVectorAttribute("Matrix", 16, vectorMatrix); 

  if ( aError > 0 ) 
  {
    transformElement->SetDoubleAttribute("Error", aError); 
  }

  if ( aDate != NULL )
  {
    transformElement->SetAttribute("Date", aDate); 
  }
  else // Add current date if it was not explicitly specified
  {
    transformElement->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str() );
  }

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::ReadTransformToCoordinateDefinition(const char* aFromCoordinateFrame, const char* aToCoordinateFrame, vtkMatrix4x4* aMatrix, double* aError/*=NULL*/, std::string* aDate/*=NULL*/)
{
  LOG_TRACE("vtkPlusConfig::ReadTransformToCoordinateDefinition(" << aFromCoordinateFrame << ", " << aToCoordinateFrame << ")");

  vtkXMLDataElement* configRootElement = GetDeviceSetConfigurationData(); 
  return ReadTransformToCoordinateDefinition(configRootElement, aFromCoordinateFrame, aToCoordinateFrame, aMatrix, aError, aDate); 
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::ReadTransformToCoordinateDefinition(vtkXMLDataElement* aDeviceSetConfigRootElement, const char* aFromCoordinateFrame, const char* aToCoordinateFrame, vtkMatrix4x4* aMatrix, double* aError/*=NULL*/, std::string* aDate/*=NULL*/)
{
  LOG_TRACE("vtkPlusConfig::ReadTransformToCoordinateDefinition(" << aFromCoordinateFrame << ", " << aToCoordinateFrame << ")");

  if ( aDeviceSetConfigRootElement == NULL )
  {
    LOG_ERROR("Failed read transform from CoordinateDefinitions - config root element is NULL"); 
    return PLUS_FAIL; 
  }

  if ( aFromCoordinateFrame == NULL )
  {
    LOG_ERROR("Failed to read transform from CoordinateDefinitions - 'From' coordinate frame name is NULL"); 
    return PLUS_FAIL; 
  }

  if ( aToCoordinateFrame == NULL )
  {
    LOG_ERROR("Failed to read transform from CoordinateDefinitions - 'To' coordinate frame name is NULL"); 
    return PLUS_FAIL; 
  }

  if ( aMatrix == NULL )
  {
    LOG_ERROR("Failed to read transform from CoordinateDefinitions - matrix is NULL"); 
    return PLUS_FAIL; 
  }

  vtkXMLDataElement* coordinateDefinitions = aDeviceSetConfigRootElement->FindNestedElementWithName("CoordinateDefinitions");
  if ( coordinateDefinitions == NULL )
  {
    LOG_ERROR("Failed read transform from CoordinateDefinitions - CoordinateDefinitions element not found"); 
    return PLUS_FAIL;  
  }

  for ( int nestedElementIndex = 0; nestedElementIndex < coordinateDefinitions->GetNumberOfNestedElements(); ++nestedElementIndex )
  {
    vtkXMLDataElement* nestedElement = coordinateDefinitions->GetNestedElement(nestedElementIndex); 
    if ( STRCASECMP(nestedElement->GetName(), "Transform" ) != 0 )
    {
      // Not a transform element, skip it
      continue; 
    }

    const char* fromAttribute = nestedElement->GetAttribute("From"); 
    const char* toAttribute = nestedElement->GetAttribute("To"); 

    if ( fromAttribute && toAttribute 
      && STRCASECMP(fromAttribute, aFromCoordinateFrame) == 0
      && STRCASECMP(toAttribute, aToCoordinateFrame) == 0 )
    {
      double vectorMatrix[16]={0}; 
      if ( nestedElement->GetVectorAttribute("Matrix", 16, vectorMatrix) )
      {
        aMatrix->DeepCopy(vectorMatrix); 
      }
      else
      {
        LOG_ERROR("Unable to find 'Matrix' attribute of '" << aFromCoordinateFrame << "' to '" << aToCoordinateFrame << "' transform among the CoordinateDefinitions in the configuration file"); 
        return PLUS_FAIL; 
      }

      if ( aError != NULL )
      {
        double error(0); 
        if ( nestedElement->GetScalarAttribute("Error", error) )
        {
          *aError = error; 
        }
        else
        {
          LOG_WARNING("Unable to find 'Error' attribute of '" << aFromCoordinateFrame << "' to '" << aToCoordinateFrame << "' transform among the CoordinateDefinitions in the configuration file"); 
        }
      }

      if ( aDate != NULL )
      {
        const char* date =  nestedElement->GetAttribute("Date"); 
        if ( date != NULL )
        {
          *aDate = date; 
        }
        else
        {
          LOG_WARNING("Unable to find 'Date' attribute of '" << aFromCoordinateFrame << "' to '" << aToCoordinateFrame << "' transform among the CoordinateDefinitions in the configuration file"); 
        }
      }

      return PLUS_SUCCESS; 
    }
  }

  LOG_DEBUG("Unable to find from '" << aFromCoordinateFrame << "' to '" << aToCoordinateFrame << "' transform among the CoordinateDefinitions in the configuration file"); 

  return PLUS_FAIL; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::ReplaceElementInDeviceSetConfiguration(const char* aElementName, vtkXMLDataElement* aNewRootElement)
{
  LOG_TRACE("vtkPlusConfig::ReplaceElementInDeviceSetConfiguration(" << aElementName << ")");

  vtkXMLDataElement* deviceSetConfigRootElement = GetDeviceSetConfigurationData();
  vtkXMLDataElement* orginalElement = deviceSetConfigRootElement->FindNestedElementWithName(aElementName);
  if (orginalElement != NULL)
  {
    deviceSetConfigRootElement->RemoveNestedElement(orginalElement);
  }

  vtkXMLDataElement* newElement = aNewRootElement->FindNestedElementWithName(aElementName);
  if (newElement != NULL)
  {
    deviceSetConfigRootElement->AddNestedElement(newElement);
  }
  else
  {
    // Re-add deleted element if there was one
    if (orginalElement != NULL)
  {
      deviceSetConfigRootElement->AddNestedElement(orginalElement);
    }
    LOG_ERROR("No element with the name '" << aElementName << "' can be found in the given XML data element!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
vtkXMLDataElement* vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(vtkXMLDataElement* aConfig, const char* aElementName, const char* aChildName, const char* aChildAttributeName, const char* aChildAttributeValue)
{
  LOG_TRACE("vtkPlusConfig::LookupElementWithNameContainingChildWithNameAndAttribute(" << aElementName << ", " << aChildName << ", " << (aChildAttributeName==NULL ? "" : aChildAttributeName) << ", " << (aChildAttributeValue==NULL ? "" : aChildAttributeValue) << ")");

  if (aConfig == NULL)
  {
    LOG_ERROR("No input XML data element is specified!");
    return NULL;
  }

  vtkXMLDataElement* firstElement = aConfig->LookupElementWithName(aElementName);
  if (firstElement == NULL)
  {
    return NULL;
  }
  else
  {
    if (aChildAttributeName && aChildAttributeValue)
    {
      return firstElement->FindNestedElementWithNameAndAttribute(aChildName, aChildAttributeName, aChildAttributeValue);
    }
    else
    {
      return firstElement->FindNestedElementWithName(aChildName);
    }
  }
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(const char* aFileName)
{
  LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInConfigurationDirectory(" << aFileName << ")");

  if (this->DeviceSetConfigurationDirectory.empty())
  {
    std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(GetDeviceSetConfigurationFileName());
    return vtkPlusConfig::GetFirstFileFoundInDirectory(aFileName, configurationDirectory.c_str());
  }
  else
  {
    return GetFirstFileFoundInDirectory(aFileName, this->DeviceSetConfigurationDirectory.c_str());
  }
};

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetFirstFileFoundInDirectory(const char* aFileName, const char* aDirectory)
{
  LOG_TRACE("vtkPlusConfig::GetFirstFileFoundInDirectory(" << aFileName << ", " << aDirectory << ")"); 

  std::string result = FindFileRecursivelyInDirectory(aFileName, aDirectory);
  if (STRCASECMP("", result.c_str()) == 0)
  {
    LOG_WARNING("File " << aFileName << " was not found in directory " << aDirectory);
  }

  return result;
};

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::FindFileRecursivelyInDirectory(const char* aFileName, const char* aDirectory)
{
  LOG_TRACE("vtkPlusConfig::FindFileRecursivelyInDirectory(" << aFileName << ", " << aDirectory << ")"); 

  std::vector<std::string> directoryList;
  directoryList.push_back(aDirectory);

  // Search for the file in the input directory first (no system paths allowed)
  std::string result = vtksys::SystemTools::FindFile(aFileName, directoryList, true);
  if (STRCASECMP("", result.c_str()))
  {
    return result;
  }
  else // If not found then call this function recursively for the subdirectories of the input directory
  {
    vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
    if (dir->Open(aDirectory))
    {        
      for (int i=0; i<dir->GetNumberOfFiles(); ++i)
      {
        const char* fileOrDirectory = dir->GetFile(i);
        if ((! dir->FileIsDirectory(fileOrDirectory)) || (STRCASECMP(".", fileOrDirectory) == 0) || (STRCASECMP("..", fileOrDirectory) == 0))
        {
          continue;
        }

        std::string fullPath(aDirectory);
        fullPath.append("/");
        fullPath.append(fileOrDirectory);

        result = FindFileRecursivelyInDirectory(aFileName, fullPath.c_str());
        if (STRCASECMP("", result.c_str()))
        {
          return result;
        }
      }
    }
    else
    {
      LOG_DEBUG("Directory " << aDirectory << " cannot be opened");
      return "";
    }
  }

  return "";
};

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::FindImagePath(const std::string& aImagePath, std::string &aFoundAbsolutePath)
{
  LOG_TRACE("vtkPlusConfig::FindImagePath(" << aImagePath << ")");

  // Check file relative to the image directory
  aFoundAbsolutePath = GetImagePath(aImagePath);
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);

  // Check file relative to the device set configuration directory
  aFoundAbsolutePath = GetDeviceSetConfigurationPath(aImagePath);
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);

  aFoundAbsolutePath = "";
  LOG_ERROR("Image with relative path '" << aImagePath << "' cannot be found neither relative to image directory ("<<GetImageDirectory()<<") nor to device set configuration directory ("<<GetDeviceSetConfigurationDirectory()<<")!");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusConfig::FindModelPath(const std::string& aModelPath, std::string &aFoundAbsolutePath)
{
  LOG_TRACE("vtkPlusConfig::FindModelPath(" << aModelPath << ")");

  // Check if the file exists in the specified absolute path
  if (vtksys::SystemTools::FileExists(aModelPath.c_str()))
  {
    // found
    aFoundAbsolutePath=aModelPath;
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aModelPath);

  // Check recursively in the model directory
  std::string absoluteModelDirectoryPath = GetModelPath(".");
  aFoundAbsolutePath = FindFileRecursivelyInDirectory(aModelPath.c_str(), absoluteModelDirectoryPath.c_str());
  if (!aFoundAbsolutePath.empty())
  {
    // found
    LOG_DEBUG("Absolute path found at: " << aFoundAbsolutePath);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found in subdirectories: " << absoluteModelDirectoryPath);

  // Check file relative to the device set configuration directory
  aFoundAbsolutePath = GetDeviceSetConfigurationPath(aModelPath);
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    // found
    LOG_DEBUG("Absolute path found at: " << aFoundAbsolutePath);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);

  aFoundAbsolutePath = "";
  LOG_ERROR("Model with relative path '" << aModelPath << "' cannot be found neither within the model directory ("<<absoluteModelDirectoryPath<<") nor in device set configuration directory ("<<GetDeviceSetConfigurationDirectory()<<")!");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetAbsolutePath(const std::string& aPath, const std::string& aBasePath)
{
  if (aPath.empty())
  {
    // empty
    return aBasePath;
  }  
  if( vtksys::SystemTools::FileIsFullPath(aPath.c_str()) )
  {
    // already absolute
    return aPath;
  }

  // relative to the ProgramDirectory
  std::string absolutePath=vtksys::SystemTools::CollapseFullPath(aPath.c_str(), aBasePath.c_str());  
  return absolutePath;
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetModelPath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->ModelDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetDeviceSetConfigurationPath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->DeviceSetConfigurationDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetOutputPath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->OutputDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetOutputDirectory()
{
  return GetOutputPath(".");
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetDeviceSetConfigurationDirectory()
{
  return GetDeviceSetConfigurationPath(".");
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetImageDirectory()
{
  return GetImagePath(".");
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetImagePath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->ImageDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetGnuplotPath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->GnuplotDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetScriptPath(const std::string& subPath)
{
  return GetAbsolutePath(subPath, GetAbsolutePath(this->ScriptsDirectory, this->ProgramDirectory) );
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetDeviceSetConfigurationFileName()
{
  return GetDeviceSetConfigurationPath(this->DeviceSetConfigurationFileName);
}

//-----------------------------------------------------------------------------
std::string vtkPlusConfig::GetApplicationStartTimestamp()
{
  return this->ApplicationStartTimestamp;
}

//-----------------------------------------------------------------------------
void vtkPlusConfig::SetDeviceSetConfigurationData(vtkXMLDataElement* deviceSetConfigurationData)
{
  vtkSetObjectBodyMacro(DeviceSetConfigurationData,vtkXMLDataElement,deviceSetConfigurationData);
  if (this->DeviceSetConfigurationData!=NULL)
  {    
    std::string plusLibVersion=PlusCommon::GetPlusLibVersionString();
    this->DeviceSetConfigurationData->SetAttribute("PlusRevision", plusLibVersion.c_str());
  }
}

