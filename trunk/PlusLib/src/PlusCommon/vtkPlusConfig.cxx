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

#include "vtksys/SystemTools.hxx" 
#include "vtkDirectory.h"
#include "vtkXMLUtilities.h"
#include "vtkMatrix4x4.h"

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

vtkCxxRevisionMacro(vtkPlusConfig, "$Revision: 1.0 $");

vtkPlusConfig *vtkPlusConfig::Instance = 0;

//-----------------------------------------------------------------------------

vtkPlusConfig* vtkPlusConfig::New()
{
  return vtkPlusConfig::GetInstance();
}

//-----------------------------------------------------------------------------

vtkPlusConfig* vtkPlusConfig::GetInstance()
{
  if(!vtkPlusConfig::Instance) 
  {
    if(!vtkPlusConfig::Instance) 
    {
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

    }
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
    return;
  }
  // user will call ->Delete() after setting instance
  instance->Register(NULL);
}

//-----------------------------------------------------------------------------

vtkPlusConfig::vtkPlusConfig()
{
  this->DeviceSetConfigurationDirectory = NULL;
  this->DeviceSetConfigurationFileName = NULL;
  this->DeviceSetConfigurationData = NULL;
  this->ApplicationConfigurationData = NULL;
  this->ApplicationConfigurationFileName = NULL;
  this->EditorApplicationExecutable = NULL;
  this->OutputDirectory = NULL;
  this->ProgramDirectory = NULL;
  this->OriginalWorkingDirectory = NULL;
  this->ImageDirectory = NULL;
  this->ModelDirectory = NULL;
  this->GnuplotDirectory = NULL;
  this->ScriptsDirectory = NULL;
  this->ApplicationStartTimestamp = NULL;

  this->SetDeviceSetConfigurationDirectory("");
  this->SetDeviceSetConfigurationFileName("");
  this->SetApplicationConfigurationFileName("PlusConfig.xml");
  this->SetEditorApplicationExecutable("notepad.exe");

  this->SetApplicationStartTimestamp(vtkAccurateTimer::GetInstance()->GetDateAndTimeString().c_str()); 

#ifdef WIN32
  char cProgramPath[2048]={'\0'};
  GetModuleFileName ( NULL, cProgramPath, 2048 ); 
  this->SetProgramPath(vtksys::SystemTools::GetProgramPath(cProgramPath).c_str()); 
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
    this->SetProgramPath(dirName.c_str()); 
  }
  else
  {      
    // non-linux systems
    // currently simply the working directory is used instead of the executable path
    // see http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
    std::string path=vtksys::SystemTools::CollapseFullPath("./");
    LOG_WARNING("Cannot get program path. PlusConfig.xml will be read from  "<<path);
    this->SetProgramPath(path.c_str());       
  }
#endif

  this->SetOriginalWorkingDirectory(vtksys::SystemTools::GetCurrentWorkingDirectory().c_str());
}

//-----------------------------------------------------------------------------

vtkPlusConfig::~vtkPlusConfig()
{
  this->SetDeviceSetConfigurationDirectory(NULL);
  this->SetDeviceSetConfigurationFileName(NULL);
  this->SetDeviceSetConfigurationData(NULL);
  this->SetApplicationConfigurationData(NULL);
  this->SetApplicationConfigurationFileName(NULL);
  this->SetEditorApplicationExecutable(NULL);
  this->SetOriginalWorkingDirectory(NULL);
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::SetProgramPath(const char* aProgramDirectory)
{
  LOG_TRACE("vtkPlusConfig::SetProgramPath(" << aProgramDirectory << ")");

  this->SetProgramDirectory(aProgramDirectory);

  // Change application configuration file to be in the program path
  std::string newApplicationConfigFileName = vtksys::SystemTools::CollapseFullPath(this->ApplicationConfigurationFileName, aProgramDirectory);
  this->SetApplicationConfigurationFileName(newApplicationConfigFileName.c_str());

  // Read application configuration from its new location
  return ReadApplicationConfiguration();
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
    LOG_ERROR("Invalid application configuration file (root XML element of the file '" << this->ApplicationConfigurationFileName << "' should be 'PlusConfig')");
    return PLUS_FAIL;
  }

  // Save date
  applicationConfigurationRoot->SetAttribute("Date", vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str());

  // Save log level
  applicationConfigurationRoot->SetIntAttribute("LogLevel", vtkPlusLogger::Instance()->GetLogLevel());

  // Save device set directory
  applicationConfigurationRoot->SetAttribute("DeviceSetConfigurationDirectory", this->DeviceSetConfigurationDirectory);

  // Save last device set config file
  applicationConfigurationRoot->SetAttribute("LastDeviceSetConfigurationFileName", this->DeviceSetConfigurationFileName);

  // Save editor application
  applicationConfigurationRoot->SetAttribute("EditorApplicationExecutable", this->EditorApplicationExecutable);

  // Save output path
  applicationConfigurationRoot->SetAttribute("OutputDirectory", this->OutputDirectory);

  // Save image directory path
  applicationConfigurationRoot->SetAttribute("ImageDirectory", this->ImageDirectory);

  // Save model directory path
  applicationConfigurationRoot->SetAttribute("ModelDirectory", this->ModelDirectory);

  // Save gnuplot directory path
  applicationConfigurationRoot->SetAttribute("GnuplotDirectory", this->GnuplotDirectory);

  // Save scripts directory path
  applicationConfigurationRoot->SetAttribute("ScriptsDirectory", this->ScriptsDirectory);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::ReadApplicationConfiguration()
{
  LOG_TRACE("vtkPlusConfig::ReadApplicationConfiguration");

  if (this->ProgramDirectory == NULL)
  {
    LOG_ERROR("Unable to read configuration - program directory has to be set first!");
    return PLUS_FAIL;
  }

  bool saveNeeded = false;

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> applicationConfigurationRoot;
  if (vtksys::SystemTools::FileExists(this->ApplicationConfigurationFileName, true))
  {
    applicationConfigurationRoot.TakeReference(vtkXMLUtilities::ReadElementFromFile(this->ApplicationConfigurationFileName));
  }
  if (applicationConfigurationRoot == NULL)
  {
    LOG_INFO("Application configuration file is not found'" << this->ApplicationConfigurationFileName << "' - default values will be used and the file created"); 
    applicationConfigurationRoot = vtkSmartPointer<vtkXMLDataElement>::New();
    applicationConfigurationRoot->SetName("PlusConfig");
    saveNeeded = true;
  }
  
  this->SetApplicationConfigurationData(applicationConfigurationRoot); 

  // Verify root element name
  if (STRCASECMP(applicationConfigurationRoot->GetName(), "PlusConfig") != 0)
  {
    LOG_ERROR("Invalid application configuration file (root XML element of the file '" << this->ApplicationConfigurationFileName << "' should be 'PlusConfig' instead of '" << applicationConfigurationRoot->GetName() << "')");
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

  // Make device set configuraiton directory
  if (! vtksys::SystemTools::MakeDirectory(this->DeviceSetConfigurationDirectory))
  {
    LOG_ERROR("Unable to create device set configuration directory '" << this->DeviceSetConfigurationDirectory << "'");
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
    this->SetModelDirectory(modelDirectory);
  }
  else
  {
    LOG_INFO("Model directory is not set - default '../config' will be used");
    this->SetModelDirectory("../config");
    saveNeeded = true;
  }

  // Read gnuplot directory
  const char* gnuplotDirectory = applicationConfigurationRoot->GetAttribute("GnuplotDirectory");
  if ((gnuplotDirectory != NULL) && (STRCASECMP(gnuplotDirectory, "") != 0))
  {
    this->SetGnuplotDirectory(gnuplotDirectory);
  }
  else
  {
    LOG_INFO("Gnuplot directory is not set - default '../gnuplot' will be used");
    this->SetGnuplotDirectory("../gnuplot");
    saveNeeded = true;
  }

  // Read scripts directory
  const char* scriptsDirectory = applicationConfigurationRoot->GetAttribute("ScriptsDirectory");
  if ((scriptsDirectory != NULL) && (STRCASECMP(scriptsDirectory, "") != 0))
  {
    this->SetScriptsDirectory(scriptsDirectory);
  }
  else
  {
    LOG_INFO("Scripts directory is not set - default '../scripts' will be used");
    this->SetScriptsDirectory("../scripts");
    saveNeeded = true;
  }

  if (saveNeeded)
  {
    return SaveApplicationConfigurationToFile();
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

std::string vtkPlusConfig::GetNewDeviceSetConfigurationFileName()
{
  LOG_TRACE("vtkPlusConfig::GetNewDeviceSetConfigurationFileName");

  std::string resultFileName; 
  if ((this->DeviceSetConfigurationFileName == NULL) || (STRCASECMP(this->DeviceSetConfigurationFileName, "") == 0))
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

  PlusCommon::PrintXML(this->ApplicationConfigurationFileName, this->ApplicationConfigurationData); 

  LOG_INFO("Application configuration file '" << this->ApplicationConfigurationFileName << "' saved");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

// static
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


  vtkXMLDataElement* deviceSetConfigRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
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

// static
PlusStatus vtkPlusConfig::ReadTransformToCoordinateDefinition(const char* aFromCoordinateFrame, const char* aToCoordinateFrame, vtkMatrix4x4* aMatrix, double* aError/*=NULL*/, std::string* aDate/*=NULL*/)
{
  LOG_TRACE("vtkPlusConfig::ReadTransformToCoordinateDefinition(" << aFromCoordinateFrame << ", " << aToCoordinateFrame << ")");

  vtkXMLDataElement* configRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData(); 
  return vtkPlusConfig::GetInstance()->ReadTransformToCoordinateDefinition(configRootElement, aFromCoordinateFrame, aToCoordinateFrame, aMatrix, aError, aDate); 
}

//-----------------------------------------------------------------------------

// static
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

  vtkXMLDataElement* deviceSetConfigRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
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

  if ((vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory() == NULL) || (STRCASECMP(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory(), "") == 0))
  {
    std::string configurationDirectory = vtksys::SystemTools::GetFilenamePath(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName());
    return vtkPlusConfig::GetFirstFileFoundInDirectory(aFileName, configurationDirectory.c_str());
  }
  else
  {
    return GetFirstFileFoundInDirectory(aFileName, vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
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

void vtkPlusConfig::SetOutputDirectory(const char* outputDir)
{
  LOG_TRACE("vtkPlusConfig::SetOutputDirectory(" << outputDir << ")"); 

  if (outputDir == NULL && this->OutputDirectory)
  { 
    return;
  }
  if ( this->OutputDirectory && outputDir && (!strcmp(this->OutputDirectory,outputDir)))
  { 
    return;
  }

  if (this->OutputDirectory)
  { 
    delete [] this->OutputDirectory; 
    this->OutputDirectory = NULL; 
  } 

  if ( outputDir )
  {
    size_t n = strlen(outputDir) + 1;
    char *cp1 =  new char[n];
    const char *cp2 = (outputDir);
    this->OutputDirectory = cp1;
    do { *cp1++ = *cp2++; } while ( --n ); 

    // Make output directory
    if (! vtksys::SystemTools::MakeDirectory(this->OutputDirectory))
  {
      LOG_ERROR("Unable to create output directory '" << this->OutputDirectory << "'");
      return;
    }

    // Set log file name and path to the output directory 
    std::ostringstream logfilename;
    logfilename << this->OutputDirectory << "/" << this->ApplicationStartTimestamp << "_PlusLog.txt";
    vtkPlusLogger::Instance()->SetLogFileName(logfilename.str().c_str()); 
  } 

  this->Modified(); 

}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::GetAbsoluteImagePath(const char* aImagePath, std::string &aFoundAbsolutePath)
{
  LOG_TRACE("vtkPlusConfig::GetAbsoluteImagePath(" << aImagePath << ")");

  // Make sure the image path is absolute
  std::string absoluteImageDirectoryPath = vtksys::SystemTools::CollapseFullPath(vtkPlusConfig::GetInstance()->GetImageDirectory());

  // Check file relative to the image directory
  aFoundAbsolutePath = vtksys::SystemTools::CollapseFullPath(aImagePath, absoluteImageDirectoryPath.c_str());
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);
  // Make sure the device set configuration path is absolute
  std::string absoluteDeviceSetConfigurationDirectoryPath = vtksys::SystemTools::CollapseFullPath(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());

  // Check file relative to the device set configuration directory
  aFoundAbsolutePath = vtksys::SystemTools::CollapseFullPath(aImagePath, absoluteDeviceSetConfigurationDirectoryPath.c_str());
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);

  aFoundAbsolutePath = "";
  LOG_ERROR("Image with relative path '" << aImagePath << "' cannot be found neither relative to image directory ("<<absoluteImageDirectoryPath<<") nor to device set configuration directory ("<<absoluteDeviceSetConfigurationDirectoryPath<<")!");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

PlusStatus vtkPlusConfig::GetAbsoluteModelPath(const char* aModelPath, std::string &aFoundAbsolutePath)
{
  LOG_TRACE("vtkPlusConfig::GetAbsoluteModelPath(" << aModelPath << ")");

  // Check if the file exists in the specified absolute path
  if (vtksys::SystemTools::FileExists(aModelPath))
  {
    // found
    aFoundAbsolutePath=aModelPath;
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aModelPath);

  // Check recursively in the model directory
  std::string absoluteModelDirectoryPath = vtksys::SystemTools::CollapseFullPath(vtkPlusConfig::GetInstance()->GetModelDirectory(), vtkPlusConfig::GetInstance()->GetProgramDirectory());
  aFoundAbsolutePath = FindFileRecursivelyInDirectory(aModelPath, absoluteModelDirectoryPath.c_str());
  if (!aFoundAbsolutePath.empty())
  {
    // found
    LOG_DEBUG("Absolute path found at: " << aFoundAbsolutePath);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found in subdirectories: " << absoluteModelDirectoryPath);

  // Check file relative to the device set configuration directory
  std::string absoluteDeviceSetConfigurationDirectoryPath = vtksys::SystemTools::CollapseFullPath(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
  aFoundAbsolutePath = vtksys::SystemTools::CollapseFullPath(aModelPath, absoluteDeviceSetConfigurationDirectoryPath.c_str());
  if (vtksys::SystemTools::FileExists(aFoundAbsolutePath.c_str()))
  {
    // found
    LOG_DEBUG("Absolute path found at: " << aFoundAbsolutePath);
    return PLUS_SUCCESS;
  }
  LOG_DEBUG("Absolute path not found at: " << aFoundAbsolutePath);

  aFoundAbsolutePath = "";
  LOG_ERROR("Model with relative path '" << aModelPath << "' cannot be found neither within the model directory ("<<absoluteModelDirectoryPath<<") nor in device set configuration directory ("<<absoluteDeviceSetConfigurationDirectoryPath<<")!");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------

const char* vtkPlusConfig::GetOutputDirectory()
{
  if( this->OutputDirectory == NULL )
  {
    return NULL;
  }
  if( vtksys::SystemTools::FileIsFullPath(this->OutputDirectory) )
  {
    return this->OutputDirectory;
  }

  std::string cwd = vtksys::SystemTools::GetCurrentWorkingDirectory();
  vtksys::SystemTools::ChangeDirectory(this->OriginalWorkingDirectory);
  std::string result = vtksys::SystemTools::GetRealPath(this->OutputDirectory);
  vtksys::SystemTools::ChangeDirectory(cwd.c_str());
  return result.c_str();
}
