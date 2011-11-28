/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"

PlusStatus TestValidTransformName( std::string from, std::string to)
{ 
  PlusTransformName transformName; 
  std::string strName = std::string(from) + std::string("To") + std::string(to); 
  if ( transformName.SetTransformName(strName.c_str()) != PLUS_SUCCESS ) 
  { 
    LOG_ERROR("Failed to create Plus transform name from " << strName ); 
    return PLUS_FAIL; 
  } 

  if ( STRCASECMP(transformName.From().c_str(), from.c_str()) != 0 ) 
  { 
    LOG_ERROR("Expected From coordinate frame name '" << from << "' differ from matched coordinate frame name '" << transformName.From() << "'."); 
  } 

  if ( STRCASECMP(transformName.To().c_str(), to.c_str()) != 0 ) 
  { 
    LOG_ERROR("Expected To coordinate frame name '" << to << "' differ from matched coordinate frame name '" << transformName.To() << "'."); 
  } 

  std::string outTransformName; 
  if ( transformName.GetTransformName(outTransformName) != PLUS_SUCCESS ) 
  { 
    LOG_ERROR("Failed to get transform name from PlusTransformName!"); 
    return PLUS_FAIL; 
  } 

  if ( STRCASECMP(strName.c_str(), outTransformName.c_str()) != 0 ) 
  { 
    LOG_ERROR("Expected transform name '" << strName << "' differ from generated transform name '" << outTransformName << "'."); 
  } 

  LOG_INFO("Input: " << strName << "  <From> coordinate frame: " << transformName.From() << "  <To> coordinate frame: " << transformName.To() << "  Output: " << outTransformName); 

  return PLUS_SUCCESS; 
} 

PlusStatus TestInvalidTransformName( std::string from, std::string to)
{ 
  PlusTransformName transformName; 
  std::string strName = std::string(from) + std::string("To") + std::string(to); 
  transformName.SetTransformName(strName.c_str()); 

  if ( !transformName.IsValid() )
  {
    LOG_INFO("Invalid transform input found: " << strName); 
  }
  else
  {
    LOG_ERROR("Invalid transform name expected!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
int main(int argc, char **argv)
{
  bool printHelp(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( TestValidTransformName("Image","Probe") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }
  if ( TestValidTransformName("Tool","Tool") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }
  
  if ( TestInvalidTransformName("To","To") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }
  if ( TestInvalidTransformName("ToTol","TolTo") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }
  if ( TestInvalidTransformName("TolTo","ToTol") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }
  if ( TestInvalidTransformName("to","to") != PLUS_SUCCESS ) { exit(EXIT_FAILURE); }

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS; 
}

