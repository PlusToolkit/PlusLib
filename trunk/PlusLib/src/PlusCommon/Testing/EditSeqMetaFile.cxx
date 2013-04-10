/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "PlusMath.h"
#include "vtkTransformRepository.h"
#include "TrackedFrame.h"
#include "vtkTrackedFrameList.h" 
#include "vtkMetaImageSequenceIO.h"

#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/RegularExpression.hxx"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

enum OperationType
{
  UPDATE_FRAME_FIELD_NAME, 
  UPDATE_FRAME_FIELD_VALUE, 
  DELETE_FRAME_FIELD, 
  UPDATE_FIELD_NAME, 
  UPDATE_FIELD_VALUE, 
  DELETE_FIELD, 
  ADD_TRANSFORM,
  TRIM, 
  MERGE,
  FILL_IMAGE_RECTANGLE,
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
    FrameTransformStart = NULL; 
    FrameTransformIncrement = NULL; 
  }

  std::string FieldName; 
  std::string UpdatedFieldName; 
  std::string UpdatedFieldValue; 
  vtkTrackedFrameList* TrackedFrameList; 
  double FrameScalarStart; 
  double FrameScalarIncrement; 
  int FrameScalarDecimalDigits; 
  vtkMatrix4x4* FrameTransformStart; 
  vtkMatrix4x4* FrameTransformIncrement; 
}; 

PlusStatus TrimSequenceMetafile( vtkTrackedFrameList* trackedFrameList, unsigned int firstFrameIndex, unsigned int lastFrameIndex ); 
PlusStatus UpdateFrameFieldValue( FrameFieldUpdate& fieldUpdate ); 
PlusStatus DeleteFrameField( vtkTrackedFrameList* trackedFrameList, std::string fieldName ); 
PlusStatus ConvertStringToMatrix( std::string &strMatrix, vtkMatrix4x4* matrix); 
PlusStatus AddTransform( vtkTrackedFrameList* trackedFrameList, std::string transformNameToAdd, std::string deviceSetConfigurationFileName );
PlusStatus FillRectangle( vtkTrackedFrameList* trackedFrameList, const std::vector<int> &fillRectOrigin, const std::vector<int> &fillRectSize, int fillGrayLevel);

const char* FIELD_VALUE_FRAME_SCALAR="{frame-scalar}"; 
const char* FIELD_VALUE_FRAME_TRANSFORM="{frame-transform}"; 

int main(int argc, char **argv)
{
  // Parse command-line arguments
  bool printHelp = false;
  int verboseLevel(vtkPlusLogger::LOG_LEVEL_UNDEFINED);
  vtksys::CommandLineArguments args;

  std::string inputFileName; // Sequence metafile name with path to edit 
  std::vector<std::string> inputFileNames; // Sequence metafile name list with path to edit 
  std::string outputFileName; // Sequence metafile name with path to save the result
  std::string strOperation; 
  OperationType operation; 
  bool useCompression = false; 
  bool incrementTimestamps = false; 

  int firstFrameIndex = -1; // First frame index used for trimming the sequence metafile.
  int lastFrameIndex = -1; // Last frame index used for trimming the sequence metafile.

  std::string fieldName; // Field name to edit
  std::string updatedFieldName;  // Updated field name after edit
  std::string updatedFieldValue;  // Updated field value after edit

  int frameScalarDecimalDigits = 5;  // Number of digits saved for frame field value into sequence metafile (Default: 5)

  double frameScalarStart = 0.0;  // Frame scalar field value starting index (Default: 0.0)
  double frameScalarIncrement = 1.0;  // Frame scalar field value increment (Default: 1.0)
  
  std::string strFrameTransformStart; // Frame transform field starting 4x4 transform matrix (Default: identity)
  vtkSmartPointer<vtkMatrix4x4> frameTransformStart = vtkSmartPointer<vtkMatrix4x4>::New();  // Frame transform field starting 4x4 transform matrix (Default: identity)
  
  std::string strFrameTransformIncrement; // Frame transform increment 4x4 transform matrix
  vtkSmartPointer<vtkMatrix4x4> frameTransformIncrement = vtkSmartPointer<vtkMatrix4x4>::New();  // Frame transform increment 4x4 transform matrix

  std::string strUpdatedReferenceTransformName; 

  std::string transformNameToAdd;  // Name of the transform to add to each frame
  std::string deviceSetConfigurationFileName; // Used device set configuration file path and name

  std::vector<int> fillRectOriginPix; // Fill rectangle top-left corner position in MF coordinate frame, in pixels
  std::vector<int> fillRectSizePix; // Fil rectangle size in MF coordinate frame, in pixels
  int fillGrayLevel=0; // Rectangle fill color

  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "Input sequence metafile name with path to edit");	
  args.AddArgument("--source-seq-files", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputFileNames, "Input sequence metafile name list with path to edit");	
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Output sequence metafile name with path to save the result");	

  args.AddArgument("--operation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strOperation, "Operation to modify sequence file (Available operations: UPDATE_FRAME_FIELD_NAME, UPDATE_FRAME_FIELD_VALUE, DELETE_FRAME_FIELD, UPDATE_FIELD_NAME, UPDATE_FIELD_VALUE, DELETE_FIELD, TRIM, MERGE, FILL_IMAGE_RECTANGLE)." );	

  // Trimming parameters 
  args.AddArgument("--first-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &firstFrameIndex, "First frame index used for trimming the sequence metafile. Index of the first frame of the sequence is 0.");	
  args.AddArgument("--last-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &lastFrameIndex, "Last frame index used for trimming the sequence metafile.");	

  args.AddArgument("--field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fieldName, "Field name to edit");	
  args.AddArgument("--updated-field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldName, "Updated field name after edit");	
  args.AddArgument("--updated-field-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldValue, "Updated field value after edit");	
  
  args.AddArgument("--frame-scalar-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarStart, "Frame scalar field value starting index (Default: 0.0)");	
  args.AddArgument("--frame-scalar-increment", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarIncrement, "Frame scalar field value increment (Default: 1.0)");	
  args.AddArgument("--frame-scalar-decimal-digits", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarDecimalDigits, "Number of digits saved for frame scalar field value into sequence metafile (Default: 5)");	

  args.AddArgument("--frame-transform-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strFrameTransformStart, "Frame transform field starting 4x4 transform matrix (Default: identity)");	
  args.AddArgument("--frame-transform-increment", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strFrameTransformIncrement, "Frame transform increment 4x4 transform matrix (Default: identity)");	
 
  args.AddArgument("--update-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strUpdatedReferenceTransformName, "Set the reference transform name to update old metafiles by changing all ToolToReference transforms to ToolToTracker transform.");	
 
  args.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &useCompression, "Compress sequence metafile images.");	
  args.AddArgument("--increment-timestamps", vtksys::CommandLineArguments::NO_ARGUMENT, &incrementTimestamps, "Increment timestamps in the order of the input-file-names");	

  args.AddArgument("--add-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformNameToAdd, "Name of the transform to add to each frame (eg. 'StylusTipToTracker')");	
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationFileName, "Used device set configuration file path and name");	

  args.AddArgument("--fill-rect-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &fillRectOriginPix, "Fill rectangle top-left corner position in MF coordinate frame, in pixels. Required for FILL_IMAGE_RECTANGLE operation.");
  args.AddArgument("--fill-rect-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &fillRectSizePix, "Fill rectangle size in MF coordinate frame, in pixels. Required for FILL_IMAGE_RECTANGLE operation.");
  args.AddArgument("--fill-gray-level", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fillGrayLevel, "Rectangle fill gray level. 0 = black, 255 = white. (Default: 0)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  
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
    std::cout << "- ADD_TRANSFORM: add specified transform." << std::endl; 

    std::cout << "- TRIM: Trim sequence metafile." << std::endl; 
    std::cout << "- MERGE: Merge multiple sequence metafiles into one." << std::endl; 
    
    std::cout << "- FILL_IMAGE_RECTANGLE: Fill a rectangle in the image (useful for removing patient data from sequences)." << std::endl;
    std::cout << "  Requires specification of the rectangle (e.g., --fill-rect-origin 12 34 --fill-rect-size 56 78)" << std::endl;

    return EXIT_SUCCESS; 

  }  

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
    LOG_INFO("No modification operation has been specified (specify --operation parameter to change the input sequence).");
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
  else if ( STRCASECMP(strOperation.c_str(), "ADD_TRANSFORM" ) == 0 )
  {
    operation = ADD_TRANSFORM; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "TRIM" ) == 0 )
  {
    operation = TRIM; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "MERGE" ) == 0 )
  {
    operation = MERGE; 
  }
  else if ( STRCASECMP(strOperation.c_str(), "FILL_IMAGE_RECTANGLE" ) == 0 )
  {
    operation = FILL_IMAGE_RECTANGLE; 
  }
  else
  {
    LOG_ERROR("Invalid operation selected: " << strOperation ); 
    return EXIT_FAILURE; 
  }

  // Convert strings transforms to vtkMatrix 
  if ( ConvertStringToMatrix(strFrameTransformStart, frameTransformStart) != PLUS_SUCCESS )
  {
    LOG_ERROR("String to matrix conversion failed for transform start matrix!"); 
    return EXIT_FAILURE;
  }

  if ( ConvertStringToMatrix(strFrameTransformIncrement, frameTransformIncrement) != PLUS_SUCCESS )
  {
    LOG_ERROR("String to matrix conversion failed for transform increment matrix!"); 
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

  double lastTimestamp = 0; 
  for ( unsigned int i = 0; i < inputFileNames.size(); i++ )
  {
    vtkSmartPointer<vtkMetaImageSequenceIO> reader = vtkSmartPointer<vtkMetaImageSequenceIO>::New();				
    reader->SetFileName(inputFileNames[i].c_str());

    LOG_INFO("Read input sequence metafile: " << inputFileNames[i] ); 

    if (reader->Read()!=PLUS_SUCCESS)
    {		
      LOG_ERROR("Couldn't read sequence metafile: " <<  inputFileName ); 
      return EXIT_FAILURE;
    }	

    if ( incrementTimestamps )
    {
      vtkTrackedFrameList * tfList = reader->GetTrackedFrameList(); 
      for ( unsigned int f = 0; f < tfList->GetNumberOfTrackedFrames(); ++f )
      {
        TrackedFrame * tf = tfList->GetTrackedFrame(f); 
        tf->SetTimestamp( lastTimestamp + tf->GetTimestamp() ); 
      }

      lastTimestamp = tfList->GetTrackedFrame(tfList->GetNumberOfTrackedFrames() - 1 )->GetTimestamp();
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
      fieldUpdate.FrameScalarDecimalDigits = frameScalarDecimalDigits; 
      fieldUpdate.FrameScalarIncrement = frameScalarIncrement; 
      fieldUpdate.FrameScalarStart = frameScalarStart; 
      fieldUpdate.FrameTransformStart = frameTransformStart; 
      fieldUpdate.FrameTransformIncrement = frameTransformIncrement; 

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
  case ADD_TRANSFORM: 
    {
      // Add transform
      LOG_INFO("Add transform '" << transformNameToAdd << "' using device set configuration file '" << deviceSetConfigurationFileName << "'"); 
      if ( AddTransform(trackedFrameList, transformNameToAdd, deviceSetConfigurationFileName ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to add transform '" << transformNameToAdd << "' using device set configuration file '" << deviceSetConfigurationFileName << "'"); 
        return EXIT_FAILURE; 
      }
    }
    break; 
  case FILL_IMAGE_RECTANGLE: 
    {
      // Fill a rectangular region in the image with a solid color
      if (FillRectangle(trackedFrameList,fillRectOriginPix,fillRectSizePix,fillGrayLevel)!=PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to fill rectangle");
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


  //////////////////////////////////////////////////////////////////
  // Convert metafiles to the new metafile format 

  if ( !strUpdatedReferenceTransformName.empty() )
  {
    PlusTransformName referenceTransformName; 
    if ( referenceTransformName.SetTransformName(strUpdatedReferenceTransformName.c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Reference transform name is invalid: " << strUpdatedReferenceTransformName );
      return EXIT_FAILURE; 
    }

    for ( unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
    {
      TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i); 

      vtkSmartPointer<vtkMatrix4x4> referenceToTrackerMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
      if ( trackedFrame->GetCustomFrameTransform(referenceTransformName, referenceToTrackerMatrix) != PLUS_SUCCESS )
      {
        LOG_WARNING("Couldn't get reference transform with name: " << strUpdatedReferenceTransformName ); 
        continue; 
      }

      std::vector<PlusTransformName> transformNameList; 
      trackedFrame->GetCustomFrameTransformNameList(transformNameList); 

      vtkSmartPointer<vtkTransform> toolToTrackerTransform = vtkSmartPointer<vtkTransform>::New(); 
      vtkSmartPointer<vtkMatrix4x4> toolToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
      for ( unsigned int n = 0; n < transformNameList.size(); ++n )
      {
        // No need to change the reference transform
        if ( transformNameList[n] == referenceTransformName )
        {
          continue; 
        }

        TrackedFrameFieldStatus status = FIELD_INVALID; 
        if ( trackedFrame->GetCustomFrameTransform(transformNameList[n], toolToReferenceMatrix) != PLUS_SUCCESS )
        {
          std::string strTransformName; 
          transformNameList[i].GetTransformName(strTransformName); 
          LOG_ERROR("Failed to get custom frame transform: " << strTransformName); 
          continue; 
        }

        if (trackedFrame->GetCustomFrameTransformStatus(transformNameList[n], status) != PLUS_SUCCESS )
        {
          std::string strTransformName; 
          transformNameList[i].GetTransformName(strTransformName); 
          LOG_ERROR("Failed to get custom frame transform status: " << strTransformName); 
          continue; 
        }

        // Compute ToolToTracker transform from ToolToReference  
        toolToTrackerTransform->Identity(); 
        toolToTrackerTransform->Concatenate(referenceToTrackerMatrix); 
        toolToTrackerTransform->Concatenate(toolToReferenceMatrix); 

        // Update the name to ToolToTracker
        PlusTransformName toolToTracker(transformNameList[n].From().c_str(), "Tracker"); 
        // Set the new custom transoform
        if ( trackedFrame->SetCustomFrameTransform(toolToTracker, toolToTrackerTransform->GetMatrix()) != PLUS_SUCCESS )
        {
          std::string strTransformName; 
          transformNameList[i].GetTransformName(strTransformName); 
          LOG_ERROR("Failed to set custom frame transform: " << strTransformName); 
          continue; 
        }
        
        // Use the same status as it was before 
        if ( trackedFrame->SetCustomFrameTransformStatus(toolToTracker, status) != PLUS_SUCCESS )
        {
          std::string strTransformName; 
          transformNameList[i].GetTransformName(strTransformName); 
          LOG_ERROR("Failed to set custom frame transform status: " << strTransformName); 
          continue; 
        }
        
        // Delete old transform and status fields 
        std::string oldTransformName, oldTransformStatus; 
        transformNameList[n].GetTransformName(oldTransformName); 
        // Append Transform to the end of the transform name
        vtksys::RegularExpression isTransform("Transform$"); 
        if ( !isTransform.find(oldTransformName) )
        {
          oldTransformName.append("Transform"); 
        }
        oldTransformStatus = oldTransformName; 
        oldTransformStatus.append("Status"); 
        trackedFrame->DeleteCustomFrameField(oldTransformName.c_str());
        trackedFrame->DeleteCustomFrameField(oldTransformStatus.c_str());

      }
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
    LOG_ERROR("Couldn't write sequence metafile: " <<  outputFileName ); 
    return EXIT_FAILURE;
  }	

  LOG_INFO("Sequence metafile editing was successful!"); 
  return EXIT_SUCCESS; 
}

//-------------------------------------------------------
PlusStatus TrimSequenceMetafile( vtkTrackedFrameList* aTrackedFrameList, unsigned int aFirstFrameIndex, unsigned int aLastFrameIndex )
{
  LOG_INFO("Trim sequence metafile from frame #: " << aFirstFrameIndex << " to frame #" << aLastFrameIndex ); 
  if ( aFirstFrameIndex < 0 || aLastFrameIndex >= aTrackedFrameList->GetNumberOfTrackedFrames() || aFirstFrameIndex > aLastFrameIndex)
  {
    LOG_ERROR("Invalid input range: (" << aFirstFrameIndex << ", " << aLastFrameIndex << ")" << " Permitted range within (0, " << aTrackedFrameList->GetNumberOfTrackedFrames() - 1 << ")");
    return PLUS_FAIL;
  }

  if (aLastFrameIndex != aTrackedFrameList->GetNumberOfTrackedFrames()-1)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(aLastFrameIndex+1, aTrackedFrameList->GetNumberOfTrackedFrames()-1);
  }

  if (aFirstFrameIndex != 0)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(0, aFirstFrameIndex-1);
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
  for ( unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
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

  // Set the start scalar value 
  double scalarVariable = fieldUpdate.FrameScalarStart; 

  // Set the start transform matrix 
  vtkSmartPointer<vtkTransform> frameTransform = vtkSmartPointer<vtkTransform>::New(); 
  if ( fieldUpdate.FrameTransformStart != NULL )
  {
    frameTransform->SetMatrix(fieldUpdate.FrameTransformStart); 
  }

  for ( unsigned int i = 0; i < fieldUpdate.TrackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* trackedFrame = fieldUpdate.TrackedFrameList->GetTrackedFrame(i); 

    /////////////////////////////////
    // Update field name 
    if ( !fieldUpdate.FieldName.empty() && !fieldUpdate.UpdatedFieldName.empty() )
    {
      const char* fieldValue = trackedFrame->GetCustomFrameField(fieldUpdate.FieldName.c_str()); 
      if ( fieldValue != NULL )
      {
        std::string copyOfFieldValue(fieldValue); 
        trackedFrame->DeleteCustomFrameField(fieldUpdate.FieldName.c_str());
        trackedFrame->SetCustomFrameField(fieldUpdate.UpdatedFieldName.c_str(), copyOfFieldValue.c_str() ); 
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
      if ( STRCASECMP(fieldUpdate.UpdatedFieldValue.c_str(), FIELD_VALUE_FRAME_SCALAR) == 0 )
      { // Update it as a scalar variable 
        
        std::ostringstream fieldValue; 
        fieldValue << std::fixed << std::setprecision(fieldUpdate.FrameScalarDecimalDigits) << scalarVariable; 

        trackedFrame->SetCustomFrameField(fieldName.c_str(), fieldValue.str().c_str() ); 
        scalarVariable += fieldUpdate.FrameScalarIncrement; 

      }
      else if ( STRCASECMP(fieldUpdate.UpdatedFieldValue.c_str(), FIELD_VALUE_FRAME_TRANSFORM) == 0 )
      { // Update it as a transform variable 

        double transformMatrix[16]={0}; 
        vtkMatrix4x4::DeepCopy(transformMatrix, frameTransform->GetMatrix()); 
        std::ostringstream strTransform; 
        strTransform	<< std::fixed << std::setprecision(fieldUpdate.FrameScalarDecimalDigits) 
          << transformMatrix[0]  << " " << transformMatrix[1]  << " " << transformMatrix[2]  << " " << transformMatrix[3]  << " " 
          << transformMatrix[4]  << " " << transformMatrix[5]  << " " << transformMatrix[6]  << " " << transformMatrix[7]  << " " 
          << transformMatrix[8]  << " " << transformMatrix[9]  << " " << transformMatrix[10] << " " << transformMatrix[11] << " " 
          << transformMatrix[12] << " " << transformMatrix[13] << " " << transformMatrix[14] << " " << transformMatrix[15] << " "; 

        trackedFrame->SetCustomFrameField(fieldName.c_str(), strTransform.str().c_str() ); 
        
        frameTransform->Concatenate(fieldUpdate.FrameTransformIncrement); 

      }
      else // Update only as a string value 
      {
        trackedFrame->SetCustomFrameField(fieldName.c_str(), fieldUpdate.UpdatedFieldValue.c_str() ); 
      }
    }
  
  }

   return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL ); 
}

//-------------------------------------------------------
PlusStatus ConvertStringToMatrix(std::string& strMatrix, vtkMatrix4x4* matrix)
{
  if ( matrix == NULL )
  {
    LOG_ERROR("Failed to convert string to matrix - output matrix is NULL!"); 
    return PLUS_FAIL; 
  }

  if( !strMatrix.empty() )
  {
    double transformMatrix[16]={0}; 
    std::istringstream transform(strMatrix); 
    double item; 
    int i = 0; 
    while ( transform >> item && i < 16)
    {
      transformMatrix[i++] = item; 
    }
    matrix->DeepCopy(transformMatrix); 
  }

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------
PlusStatus AddTransform( vtkTrackedFrameList* trackedFrameList, std::string transformNameToAdd, std::string deviceSetConfigurationFileName )
{
  if ( trackedFrameList == NULL )
  {
    LOG_ERROR("Tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( transformNameToAdd.empty() )
  {
    LOG_ERROR("Added transform name is empty!"); 
    return PLUS_FAIL; 
  }

  if ( deviceSetConfigurationFileName.empty() )
  {
    LOG_ERROR("Used device set configuration file name is empty!"); 
    return PLUS_FAIL; 
  }

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(deviceSetConfigurationFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read device set configuration from file " << deviceSetConfigurationFileName); 
    return PLUS_FAIL;
  }

  // Create transform name
  PlusTransformName transformName;
  transformName.SetTransformName(transformNameToAdd.c_str());

  for ( unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i); 

    // Set up transform repository
    vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
    if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to set device set configuration to transform repository!");
      return PLUS_FAIL;
    }
    if (transformRepository->SetTransforms(*trackedFrame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to set transforms from tracked frame " << i << " to transform repository!");
      return PLUS_FAIL;
    }

    // Get transform matrix
    bool valid = false;
    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if ( transformRepository->GetTransform(transformName, transformMatrix, &valid) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get transform " << transformNameToAdd << " from tracked frame " << i);
      continue;
    }

    trackedFrame->SetCustomFrameTransform(transformName, transformMatrix);
  }

  return PLUS_SUCCESS; 
}

//-------------------------------------------------------
PlusStatus FillRectangle(vtkTrackedFrameList* trackedFrameList, const std::vector<int> &fillRectOrigin, const std::vector<int> &fillRectSize, int fillGrayLevel)
{
  if ( trackedFrameList == NULL )
  {
    LOG_ERROR("Tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }
  if (fillRectOrigin.size()!=2 || fillRectSize.size()!=2)
  {
    LOG_ERROR("Fill rectangle origin or size is not specified correctly");
    return PLUS_FAIL;
  }

  for ( unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    TrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i); 
    PlusVideoFrame* videoFrame = trackedFrame->GetImageData();
    int frameSize[2]={0,0};
    if (videoFrame == NULL || videoFrame->GetFrameSize(frameSize)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve pixel data from frame "<<i<<". Fill rectangle failed.");
      continue;
    }       
    if (fillRectOrigin[0]<0 || fillRectOrigin[0]>=frameSize[0] ||
      fillRectOrigin[1]<0 || fillRectOrigin[1]>=frameSize[1])
    {
      LOG_ERROR("Invalid fill rectangle origin is specified ("<<fillRectOrigin[0]<<", "<<fillRectOrigin[1]<<"). The image size is ("
        <<frameSize[0]<<", "<<frameSize[1]<<").");
      continue;
    }
    if (fillRectSize[0]<=0 || fillRectOrigin[0]+fillRectSize[0]>frameSize[0] ||
      fillRectSize[1]<=0 || fillRectOrigin[1]+fillRectSize[1]>frameSize[1])
    {
      LOG_ERROR("Invalid fill rectangle size is specified ("<<fillRectSize[0]<<", "<<fillRectSize[1]<<"). The specified fill rectangle origin is ("
        <<fillRectOrigin[0]<<", "<<fillRectOrigin[1]<<") and the image size is ("<<frameSize[0]<<", "<<frameSize[1]<<").");
      continue;
    }
    if (videoFrame->GetVTKScalarPixelType()!=VTK_UNSIGNED_CHAR)
    {
      LOG_ERROR("Fill rectangle is supported only for B-mode images (unsigned char type)");
      continue;
    }
    unsigned char fillData=0;
    if (fillGrayLevel<0)
    {
      fillData=0;
    }
    else if (fillGrayLevel>255)
    {
      fillData=255;
    }
    else
    {
      fillData=fillGrayLevel;
    }          
    for (int y=0; y<fillRectSize[1]; y++)
    {
      memset(static_cast<unsigned char*>(videoFrame->GetBufferPointer())+(fillRectOrigin[1]+y)*frameSize[0]+fillRectOrigin[0],fillData,fillRectSize[0]);     
    }    
  }
  return PLUS_SUCCESS; 
}
