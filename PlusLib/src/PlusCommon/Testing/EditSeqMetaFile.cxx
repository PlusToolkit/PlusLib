/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "PlusMath.h"
#include "TrackedFrame.h"
#include "vtkTrackedFrameList.h" 
#include "vtkMetaImageSequenceIO.h"

enum OperationType
{
  UPDATE_FRAME_FIELD_NAME, 
  UPDATE_FRAME_FIELD_VALUE, 
  DELETE_FRAME_FIELD, 
  UPDATE_FIELD_NAME, 
  UPDATE_FIELD_VALUE, 
  DELETE_FIELD, 
  TRIM, 
  MERGE, 
  NO_OPERATION
}; 

class FrameFieldUpdate
{
public: 
  FrameFieldUpdate()
  {
    TrackedFrameList = NULL; 
    FrameScalarStart = 0; 
    FrameScalarIncrement = 0; 
    FrameScalarDecimalDigits = 5; 
    FrameTransformTranslateIncrement[0] = 0; 
    FrameTransformTranslateIncrement[1] = 0; 
    FrameTransformTranslateIncrement[2] = 0; 

    FrameTransformRotateDegIncrement[0] = 0; 
    FrameTransformRotateDegIncrement[1] = 0; 
    FrameTransformRotateDegIncrement[2] = 0;

    for ( int i = 0; i < 16; i++ )
    {
      FrameTransformStart[i] = 0.0; 
    }
  }

  vtkTrackedFrameList* TrackedFrameList; 
  std::string FieldName; 
  std::string UpdatedFieldName; 
  std::string UpdatedFieldValue; 
  int FrameScalarStart; 
  int FrameScalarIncrement; 
  int FrameScalarDecimalDigits; 
  double FrameTransformTranslateIncrement[3]; 
  double FrameTransformRotateDegIncrement[3]; 
  double FrameTransformStart[16]; 
}; 

PlusStatus TrimSequenceMetafile( vtkTrackedFrameList* trackedFrameList, int firstFrameIndex, int lastFrameIndex ); 
PlusStatus UpdateFrameFieldValue( FrameFieldUpdate& fieldUpdate ); 
PlusStatus DeleteFrameField( vtkTrackedFrameList* trackedFrameList, std::string fieldName ); 


int main(int argc, char **argv)
{
  // Parse command-line arguments
  bool printHelp(false);
  int verboseLevel(vtkPlusLogger::LOG_LEVEL_DEFAULT);
  vtksys::CommandLineArguments args;

  std::string inputFileName; // Sequence metafile name with path to edit 
  std::vector<std::string> inputFileNames; // Sequence metafile name list with path to edit 
  std::string outputFileName; // Sequence metafile name with path to save the result
  std::string strOperation; 
  OperationType operation; 
  bool useCompression(false); 

  int firstFrameIndex(-1); // First frame index used for trimming the sequence metafile.
  int lastFrameIndex(-1); // Last frame index used for trimming the sequence metafile.

  std::string fieldName; // Field name to edit
  std::string updatedFieldName;  // Updated field name after edit
  std::string updatedFieldValue;  // Updated field value after edit


  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  args.AddArgument("--input-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "Input sequence metafile name with path to edit");	
  args.AddArgument("--input-file-names", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputFileNames, "Input sequence metafile name list with path to edit");	
  args.AddArgument("--output-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Output sequence metafile name with path to save the result");	

  args.AddArgument("--operation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strOperation, "Operation to modify sequence file (Available operations: UPDATE_FRAME_FIELD_NAME, UPDATE_FRAME_FIELD_VALUE, DELETE_FRAME_FIELD, UPDATE_FIELD_NAME, UPDATE_FIELD_VALUE, DELETE_FIELD, TRIM, MERGE)." );	

  // Trimming parameters 
  args.AddArgument("--first-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &firstFrameIndex, "First frame index used for trimming the sequence metafile.");	
  args.AddArgument("--last-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &lastFrameIndex, "Last frame index used for trimming the sequence metafile.");	

  args.AddArgument("--field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fieldName, "Field name to edit");	
  args.AddArgument("--updated-field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldName, "Updated field name after edit");	
  args.AddArgument("--updated-field-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldValue, "Updated field value after edit");	
  

  args.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &useCompression, "Compress sequence metafile images.");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    std::cout << std::endl << "Operations: " << std::endl << std::endl; 

    std::cout << "- UPDATE_FRAME_FIELD_NAME: update field names for each frame, if not exists add it." << std::endl; 
    std::cout << "- UPDATE_FRAME_FIELD_VALUE: update field values for each frame, if not exists add it." << std::endl; 
    std::cout << "- DELETE_FRAME_FIELD: delete fields with name specified from each frame." << std::endl; 

    std::cout << "- UPDATE_FIELD_NAME: update field name, if not exists add it." << std::endl; 
    std::cout << "- UPDATE_FIELD_VALUE: update field value, if not exists add it." << std::endl; 
    std::cout << "- DELETE_FIELD: delete field with name specified." << std::endl; 

    std::cout << "- TRIM: Trim sequence metafile." << std::endl; 
    std::cout << "- MERGE: Merge multiple sequence metafiles into one." << std::endl; 

    return EXIT_SUCCESS; 

  }
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  ///////////////////////////////////////////////////////////////////
  // Check command line arguments 
  if ( inputFileName.empty() && inputFileNames.empty() )
  {
    LOG_ERROR("At least one input file name is mandantory!"); 
    return EXIT_FAILURE; 
  }

  if ( outputFileName.empty() )
  {
    LOG_ERROR("Please set output file name!"); 
    return EXIT_FAILURE; 
  }

  // Set operation
  if ( strOperation.empty() )
  {
    operation = NO_OPERATION; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "UPDATE_FRAME_FIELD_NAME" ) == 0 )
  {
    operation = UPDATE_FRAME_FIELD_NAME; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "UPDATE_FRAME_FIELD_VALUE" ) == 0 )
  {
    operation = UPDATE_FRAME_FIELD_VALUE; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "DELETE_FRAME_FIELD" ) == 0 )
  {
    operation = DELETE_FRAME_FIELD; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "UPDATE_FIELD_NAME" ) == 0 )
  {
    operation = UPDATE_FIELD_NAME; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "UPDATE_FIELD_VALUE" ) == 0 )
  {
    operation = UPDATE_FIELD_VALUE; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "DELETE_FIELD" ) == 0 )
  {
    operation = DELETE_FIELD; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "TRIM" ) == 0 )
  {
    operation = TRIM; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "MERGE" ) == 0 )
  {
    operation = MERGE; 
  }
  else
  {
    LOG_ERROR("Invalid operation selected: " << strOperation ); 
    return EXIT_FAILURE; 
  }

  ///////////////////////////////////////////////////////////////////
  // Read input files 

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  if ( !inputFileName.empty() )
  {
    // Insert file name to the beginning of the list 
    inputFileNames.insert(inputFileNames.begin(), inputFileName); 
  }

  for ( int i = 0; i < inputFileNames.size(); i++ )
  {
    vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();				
    reader->SetFileName(inputFileNames[i].c_str());

    LOG_INFO("Read input sequence metafile: " << inputFileNames[i] ); 

    if (reader->Read()!=PLUS_SUCCESS)
    {		
      LOG_ERROR("Couldn't read sequence metafile: " <<  inputFileName ); 
      return EXIT_FAILURE;
    }	

    if ( trackedFrameList->AddTrackedFrameList(reader->GetTrackedFrameList()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to append tracked frame list!");
      return EXIT_SUCCESS; 
    }
  }


  ///////////////////////////////////////////////////////////////////
  // Make the operation 

  


  switch ( operation )
  {
  case NO_OPERATION:
  case MERGE: 
    {
      // No need to do anything just save into output file 
    }
    break; 
  case TRIM: 
    {
      if ( TrimSequenceMetafile( trackedFrameList, firstFrameIndex, lastFrameIndex ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to trim sequence metafile!"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case UPDATE_FRAME_FIELD_NAME:
    {
      FrameFieldUpdate fieldUpdate; 
      fieldUpdate.TrackedFrameList = trackedFrameList; 
      fieldUpdate.FieldName = fieldName; 
      fieldUpdate.UpdatedFieldName = updatedFieldName; 

      if ( UpdateFrameFieldValue( fieldUpdate ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to update frame field name '"<<fieldName<<"' to '"<<updatedFieldName<<"'"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case UPDATE_FRAME_FIELD_VALUE:
    {
      FrameFieldUpdate fieldUpdate; 
      fieldUpdate.TrackedFrameList = trackedFrameList; 
      fieldUpdate.FieldName = fieldName; 
      fieldUpdate.UpdatedFieldName = updatedFieldName; 
      fieldUpdate.UpdatedFieldValue = updatedFieldValue; 

      if ( UpdateFrameFieldValue( fieldUpdate ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to update frame field value!"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case DELETE_FRAME_FIELD: 
    {
      if ( DeleteFrameField( trackedFrameList, fieldName ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to delete frame field!"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case DELETE_FIELD: 
    {
      // Delete field
      LOG_INFO("Delete field: " << fieldName ); 
      if ( trackedFrameList->SetCustomString(fieldName.c_str(), NULL ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to delete field: " << fieldName ); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case UPDATE_FIELD_NAME: 
    {
      // Update field name
      LOG_INFO("Update field name '" << fieldName << "' to  '"<<updatedFieldName<<"'"); 
      const char* fieldValue = trackedFrameList->GetCustomString(fieldName.c_str()); 
      if ( fieldValue != NULL ) 
      { 
        // Delete field
        if ( trackedFrameList->SetCustomString(fieldName.c_str(), NULL ) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to delete field: " << fieldName ); 
          return EXIT_FAILURE; 
        }

        // Add new field 
        if ( trackedFrameList->SetCustomString(updatedFieldName.c_str(), fieldValue ) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to update field '" << updatedFieldName << "' with value '"<<fieldValue<<"'"); 
          return EXIT_FAILURE; 
        }
      }
    }
    break; 
  case UPDATE_FIELD_VALUE: 
    {
      // Update field value
      LOG_INFO("Update field '" << fieldName << "' with value '"<<updatedFieldValue<<"'"); 
      if ( trackedFrameList->SetCustomString(fieldName.c_str(), updatedFieldValue.c_str() ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to update field '" << fieldName << "' with value '"<<updatedFieldValue<<"'"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  default: 
    {
      LOG_WARNING("Selected operation not yet implemented!"); 
      return EXIT_FAILURE; 
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Save output file to metafile 

  LOG_INFO("Save output sequence metafile to: " << outputFileName ); 
  vtkSmartPointer<vtkMetaImageSequenceIO> writer=vtkSmartPointer<vtkMetaImageSequenceIO>::New();			
  writer->SetFileName(outputFileName.c_str());
  writer->SetTrackedFrameList(trackedFrameList); 
  writer->SetUseCompression(useCompression);

  if (writer->Write() != PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't write sequence metafile: " <<  writer->GetFileName() ); 
    return EXIT_FAILURE;
  }	

  LOG_INFO("Sequence metafile editing was successful!"); 
  return EXIT_SUCCESS; 
}

//-------------------------------------------------------
PlusStatus TrimSequenceMetafile( vtkTrackedFrameList* aTrackedFrameList, int aFirstFrameIndex, int aLastFrameIndex )
{
  LOG_INFO("Trim sequence metafile from frame #: " << aFirstFrameIndex << " to frame #" << aLastFrameIndex ); 
  if ( aLastFrameIndex > aTrackedFrameList->GetNumberOfTrackedFrames() - 1 ) 
  {
    LOG_WARNING("Last frame index is less then desired: " << aTrackedFrameList->GetNumberOfTrackedFrames() - 1 ); 
  }

  for ( int i = aLastFrameIndex + 1; i < aTrackedFrameList->GetNumberOfTrackedFrames(); i++ )
  {
    aTrackedFrameList->RemoveTrackedFrame(i); 
  }

  for ( int i = 0; i < aFirstFrameIndex; i++ )
  {
    aTrackedFrameList->RemoveTrackedFrame(i); 
  }

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------
PlusStatus DeleteFrameField( vtkTrackedFrameList* trackedFrameList, std::string fieldName )
{
  if ( trackedFrameList == NULL )
  {
    LOG_ERROR("Tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( fieldName.empty() )
  {
    LOG_ERROR("Field name is empty!"); 
    return PLUS_FAIL; 
  }

  LOG_INFO("Delete frame field: " << fieldName ); 
  int numberOfErrors(0); 
  for ( int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i); 

    /////////////////////////////////
    // Delete field name 
    const char* fieldValue = trackedFrame->GetCustomFrameField(fieldName.c_str()); 
    if ( fieldValue != NULL 
      && trackedFrame->DeleteCustomFrameField(fieldName.c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to delete custom frame field '"<< fieldName << "' for frame #" << i ); 
      numberOfErrors++; 
    }
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}


//-------------------------------------------------------
PlusStatus UpdateFrameFieldValue( FrameFieldUpdate& fieldUpdate )
{
  LOG_INFO("Update frame field" ); 
  int numberOfErrors(0);
  for ( int i = 0; i < fieldUpdate.TrackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* trackedFrame = fieldUpdate.TrackedFrameList->GetTrackedFrame(i); 

    /////////////////////////////////
    // Update field name 
    if ( !fieldUpdate.FieldName.empty() && !fieldUpdate.UpdatedFieldName.empty() )
    {
      const char* fieldValue = trackedFrame->GetCustomFrameField(fieldUpdate.FieldName.c_str()); 
      if ( fieldValue != NULL )
      {
        trackedFrame->DeleteCustomFrameField(fieldUpdate.FieldName.c_str());
        trackedFrame->SetCustomFrameField(fieldUpdate.UpdatedFieldName.c_str(), fieldValue ); 
      }
    }

    std::string fieldName = fieldUpdate.FieldName; 
    if ( !fieldUpdate.UpdatedFieldName.empty() )
    {
      fieldName = fieldUpdate.UpdatedFieldName; 
    }

    /////////////////////////////////
    // Update field value 
    if ( !fieldName.empty() && !fieldUpdate.UpdatedFieldValue.empty() )
    {
      trackedFrame->SetCustomFrameField(fieldName.c_str(), fieldUpdate.UpdatedFieldValue.c_str() ); 
    }
  
  }

   return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}