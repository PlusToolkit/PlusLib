/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "PlusMath.h"
#include "igsioTrackedFrame.h"
#include "vtkPlusSequenceIO.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkIGSIOTransformRepository.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkXMLDataElement.h>
#include <vtkXMLUtilities.h>
#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/RegularExpression.hxx>

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
  APPEND,
  MIX,
  FILL_IMAGE_RECTANGLE,
  CROP,
  REMOVE_IMAGE_DATA,
  DECIMATE,
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

  std::string               FieldName;
  std::string               UpdatedFieldName;
  std::string               UpdatedFieldValue;
  vtkIGSIOTrackedFrameList*  TrackedFrameList;
  double                    FrameScalarStart;
  double                    FrameScalarIncrement;
  int                       FrameScalarDecimalDigits;
  vtkMatrix4x4*             FrameTransformStart;
  vtkMatrix4x4*             FrameTransformIncrement;
  std::string               FrameTransformIndexFieldName;
};

PlusStatus TrimSequenceFile(vtkIGSIOTrackedFrameList* trackedFrameList, unsigned int firstFrameIndex, unsigned int lastFrameIndex);
PlusStatus DecimateSequenceFile(vtkIGSIOTrackedFrameList* trackedFrameList, unsigned int decimationFactor);
PlusStatus UpdateFrameFieldValue(FrameFieldUpdate& fieldUpdate);
PlusStatus DeleteFrameField(vtkIGSIOTrackedFrameList* trackedFrameList, std::string fieldName);
PlusStatus ConvertStringToMatrix(std::string& strMatrix, vtkMatrix4x4* matrix);
PlusStatus AddTransform(vtkIGSIOTrackedFrameList* trackedFrameList, std::vector<std::string> transformNamesToAdd, std::string deviceSetConfigurationFileName);
PlusStatus FillRectangle(vtkIGSIOTrackedFrameList* trackedFrameList, const std::vector<unsigned int>& fillRectOrigin, const std::vector<unsigned int>& fillRectSize, int fillGrayLevel);
PlusStatus CropRectangle(vtkIGSIOTrackedFrameList* trackedFrameList, igsioVideoFrame::FlipInfoType& flipInfo, const std::vector<int>& cropRectOrigin, const std::vector<int>& cropRectSize);

namespace
{
  const std::string FIELD_VALUE_FRAME_SCALAR = "{frame-scalar}";
  const std::string FIELD_VALUE_FRAME_TRANSFORM = "{frame-transform}";
}

// Fuse all fields in sequence files into the first sequence
//----------------------------------------------------------------------------
PlusStatus MixTrackedFrameLists(vtkIGSIOTrackedFrameList* trackedFrameList, std::vector<std::string> inputFileNames)
{
  if (inputFileNames.size() == 0)
  {
    LOG_ERROR("No --source-seq-files specified");
    return PLUS_FAIL;
  }

  LOG_INFO("Read master sequence file: " << inputFileNames[0]);
  if (vtkPlusSequenceIO::Read(inputFileNames[0], trackedFrameList) != PLUS_SUCCESS)
  {
    LOG_ERROR("Couldn't read sequence file: " << inputFileNames[0]);
    return PLUS_FAIL;
  }
  if (trackedFrameList->GetNumberOfTrackedFrames() == 0)
  {
    LOG_ERROR("No frames in sequence file: " << inputFileNames[0]);
    return PLUS_FAIL;
  }

  for (unsigned int i = 1; i < inputFileNames.size(); i++)
  {
    LOG_INFO("Read input sequence file: " << inputFileNames[i]);
    vtkSmartPointer<vtkIGSIOTrackedFrameList> additionalTrackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
    if (vtkPlusSequenceIO::Read(inputFileNames[i], additionalTrackedFrameList) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't read sequence file: " << inputFileNames[0]);
      return PLUS_FAIL;
    }
    if (additionalTrackedFrameList->GetNumberOfTrackedFrames() == 0)
    {
      continue;
    }

    unsigned int additionalFrameIndex = 0;
    double maxTimestampValueForCurrentAdditionalFrame = additionalTrackedFrameList->GetTrackedFrame(0)->GetTimestamp();
    if (additionalTrackedFrameList->GetNumberOfTrackedFrames() >= 2)
    {
      maxTimestampValueForCurrentAdditionalFrame = (additionalTrackedFrameList->GetTrackedFrame(additionalFrameIndex + 1)->GetTimestamp() + additionalTrackedFrameList->GetTrackedFrame(additionalFrameIndex)->GetTimestamp()) / 2.0;
    }
    for (unsigned int f = 0; f < trackedFrameList->GetNumberOfTrackedFrames(); ++f)
    {
      igsioTrackedFrame* masterTrackedFrame = trackedFrameList->GetTrackedFrame(f);

      // Determine which additional frame belongs to this master frame
      while (masterTrackedFrame->GetTimestamp() > maxTimestampValueForCurrentAdditionalFrame
             && additionalFrameIndex + 1 < additionalTrackedFrameList->GetNumberOfTrackedFrames())
      {
        if (additionalFrameIndex == additionalTrackedFrameList->GetNumberOfTrackedFrames() - 1)
        {
          // last frame, all remaining frames are assigned to it
          break;
        }
        additionalFrameIndex++;
        // use this frame index until timestamp is closest to this frame's timestamp
        maxTimestampValueForCurrentAdditionalFrame = (additionalTrackedFrameList->GetTrackedFrame(additionalFrameIndex)->GetTimestamp() +
            additionalTrackedFrameList->GetTrackedFrame(additionalFrameIndex + 1)->GetTimestamp()) / 2.0;
      }

      // Copy frame fields
      igsioTrackedFrame* additionalFrame = additionalTrackedFrameList->GetTrackedFrame(additionalFrameIndex);
      auto customFrameFields = additionalFrame->GetCustomFields();
      for (auto fieldIter = customFrameFields.begin(); fieldIter != customFrameFields.end(); ++fieldIter)
      {
        if (!fieldIter->first.compare("FrameNumber") ||
            !fieldIter->first.compare("Timestamp") ||
            !fieldIter->first.compare("UnfilteredTimestamp") ||
            !fieldIter->first.compare("ImageStatus"))
        {
          // Timing and image information is taken from the first sequence
          continue;
        }
        masterTrackedFrame->SetFrameField(fieldIter->first, fieldIter->second);
      }
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Append tracked frame list (one after the other)
PlusStatus AppendTrackedFrameLists(vtkIGSIOTrackedFrameList* trackedFrameList, std::vector<std::string> inputFileNames, bool incrementTimestamps)
{
  double lastTimestamp = 0;
  for (unsigned int i = 0; i < inputFileNames.size(); i++)
  {
    LOG_INFO("Read input sequence file: " << inputFileNames[i]);
    vtkSmartPointer<vtkIGSIOTrackedFrameList> timestampFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();
    if (vtkPlusSequenceIO::Read(inputFileNames[i], timestampFrameList) != PLUS_SUCCESS)
    {
      LOG_ERROR("Couldn't read sequence file: " << inputFileNames[0]);
      return PLUS_FAIL;
    }

    if (incrementTimestamps)
    {
      vtkIGSIOTrackedFrameList* tfList = timestampFrameList;
      for (unsigned int f = 0; f < tfList->GetNumberOfTrackedFrames(); ++f)
      {
        igsioTrackedFrame* tf = tfList->GetTrackedFrame(f);
        tf->SetTimestamp(lastTimestamp + tf->GetTimestamp());
      }

      lastTimestamp = tfList->GetTrackedFrame(tfList->GetNumberOfTrackedFrames() - 1)->GetTimestamp();
    }

    if (trackedFrameList->AddTrackedFrameList(timestampFrameList) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to append tracked frame list!");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Parse command-line arguments
  bool                            printHelp = false;
  int                             verboseLevel(vtkIGSIOLogger::LOG_LEVEL_UNDEFINED);
  vtksys::CommandLineArguments    args;

  std::string                     inputFileName; // Sequence file name with path to edit
  std::vector<std::string>        inputFileNames; // Sequence file name list with path to edit
  std::string                     outputFileName; // Sequence file name with path to save the result
  std::string                     strOperation;
  OperationType                   operation;
  bool                            useCompression = false;
  bool                            incrementTimestamps = false;

  int                             firstFrameIndex = -1; // First frame index used for trimming the sequence file.
  int                             lastFrameIndex = -1; // Last frame index used for trimming the sequence file.

  std::string                     fieldName; // Field name to edit
  std::string                     updatedFieldName;  // Updated field name after edit
  std::string                     updatedFieldValue;  // Updated field value after edit

  int                             frameScalarDecimalDigits = 5;  // Number of digits saved for frame field value into sequence file (Default: 5)

  double                          frameScalarStart = 0.0;  // Frame scalar field value starting index (Default: 0.0)
  double                          frameScalarIncrement = 1.0;  // Frame scalar field value increment (Default: 1.0)

  int                             decimationFactor = 2; // Keep every 2nd frame by default

  std::string                     strFrameTransformStart; // Frame transform field starting 4x4 transform matrix (Default: identity)
  vtkSmartPointer<vtkMatrix4x4>   frameTransformStart = vtkSmartPointer<vtkMatrix4x4>::New();  // Frame transform field starting 4x4 transform matrix (Default: identity)

  std::string                     strFrameTransformIncrement; // Frame transform increment 4x4 transform matrix
  vtkSmartPointer<vtkMatrix4x4>   frameTransformIncrement = vtkSmartPointer<vtkMatrix4x4>::New();  // Frame transform increment 4x4 transform matrix

  std::string                     strFrameTransformIndexFieldName;

  std::string                     strUpdatedReferenceTransformName;

  std::string                     transformNamesToAdd;  // Name of the transform to add to each frame
  std::string                     deviceSetConfigurationFileName; // Used device set configuration file path and name

  std::vector<int>                rectOriginPix; // Fill/crop rectangle top-left corner position in MF coordinate frame, in pixels
  std::vector<int>                rectSizePix; // Fill/crop rectangle size in MF coordinate frame, in pixels
  int                             fillGrayLevel = 0; // Rectangle fill color

  bool                            flipX(false);
  bool                            flipY(false);
  bool                            flipZ(false);

  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "Input sequence file name with path to edit");
  args.AddArgument("--source-seq-files", vtksys::CommandLineArguments::MULTI_ARGUMENT, &inputFileNames, "Input sequence file name list with path to edit");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Output sequence file name with path to save the result");

  args.AddArgument("--operation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strOperation, "Operation to modify sequence file. See available operations below.");

  // Trimming parameters
  args.AddArgument("--first-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &firstFrameIndex, "First frame index used for trimming the sequence file. Index of the first frame of the sequence is 0.");
  args.AddArgument("--last-frame-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &lastFrameIndex, "Last frame index used for trimming the sequence file.");

  // Decimation parameters
  args.AddArgument("--decimation-factor", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &decimationFactor, "Used for DECIMATE operation, where every N-th frame is kept. This parameter specifies N (Default: 2)");

  args.AddArgument("--field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fieldName, "Field name to edit");
  args.AddArgument("--updated-field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldName, "Updated field name after edit");
  args.AddArgument("--updated-field-value", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &updatedFieldValue, "Updated field value after edit");

  args.AddArgument("--frame-scalar-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarStart, "Frame scalar field value starting index (Default: 0.0)");
  args.AddArgument("--frame-scalar-increment", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarIncrement, "Frame scalar field value increment (Default: 1.0)");
  args.AddArgument("--frame-scalar-decimal-digits", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameScalarDecimalDigits, "Number of digits saved for frame scalar field value into sequence file (Default: 5)");

  args.AddArgument("--frame-transform-start", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strFrameTransformStart, "Frame transform field starting 4x4 transform matrix (Default: identity)");
  args.AddArgument("--frame-transform-increment", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strFrameTransformIncrement, "Frame transform increment 4x4 transform matrix (Default: identity)");
  args.AddArgument("--frame-transform-index-field-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strFrameTransformIndexFieldName, "If specified then increment is applied as many times as the value of this field");

  args.AddArgument("--update-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &strUpdatedReferenceTransformName, "Set the reference transform name to update old files by changing all ToolToReference transforms to ToolToTracker transform.");

  args.AddArgument("--use-compression", vtksys::CommandLineArguments::NO_ARGUMENT, &useCompression, "Compress sequence file images.");
  args.AddArgument("--increment-timestamps", vtksys::CommandLineArguments::NO_ARGUMENT, &incrementTimestamps, "Increment timestamps in the order of the input-file-names");

  args.AddArgument("--add-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &transformNamesToAdd, "Name of the transform to add to each frame (e.g., StylusTipToTracker); multiple transforms can be added separated by a comma (e.g., StylusTipToReference,ProbeToReference)");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationFileName, "Used device set configuration file path and name");

  // Clip and transform arguments
  args.AddArgument("--rect-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &rectOriginPix, "Fill or crop rectangle top-left corner position in MF coordinate frame, in pixels, separated by space (e.g., --rect-origin 12 34).");
  args.AddArgument("--rect-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &rectSizePix, "Fill or crop rectangle size in MF coordinate frame, in pixels, separated by space (e.g., --rect-size 56 78).");
  args.AddArgument("--flipX", vtksys::CommandLineArguments::NO_ARGUMENT, &flipX, "Flip image along X axis.");
  args.AddArgument("--flipY", vtksys::CommandLineArguments::NO_ARGUMENT, &flipY, "Flip image along Y axis.");
  args.AddArgument("--flipZ", vtksys::CommandLineArguments::NO_ARGUMENT, &flipZ, "Flip image along Z axis.");
  args.AddArgument("--fill-gray-level", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fillGrayLevel, "Rectangle fill gray level. 0 = black, 255 = white. (Default: 0)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////////////////////////////////////////////////////////
  if (argc == 1 || printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    std::cout << std::endl << "Operations: " << std::endl << std::endl;

    std::cout << "- UPDATE_FRAME_FIELD_NAME: update per-frame field names for each frame. If field does not exist then it is added." << std::endl;
    std::cout << "  Requires --field-name and --updated-field-name." << std::endl;
    std::cout << "- UPDATE_FRAME_FIELD_VALUE: update per-frame field values for each frame, if not exists add it." << std::endl;
    std::cout << "  Uses --field-name, --updated-field-name, --updated-field-value, --frame-scalar-*, --frame-transform-*" << std::endl;
    std::cout << "- DELETE_FRAME_FIELD: delete per-frame field (field values specified for each frame)." << std::endl;
    std::cout << "  Requires --field-name." << std::endl;

    std::cout << "- UPDATE_FIELD_NAME: update field name, if not exists add it." << std::endl;
    std::cout << "  Requires --field-name and --updated-field-name." << std::endl;
    std::cout << "- UPDATE_FIELD_VALUE: update field value, if not exists add it." << std::endl;
    std::cout << "  Requires --field-name and --updated-field-value." << std::endl;
    std::cout << "- DELETE_FIELD: delete field with name specified." << std::endl;
    std::cout << "  Requires --field-name." << std::endl;
    std::cout << "- ADD_TRANSFORM: add specified transform." << std::endl;
    std::cout << "  Requires --add-transform." << std::endl;

    std::cout << "- TRIM: Trim sequence file." << std::endl;
    std::cout << "  Requires --first-frame-index and --last-frame-index." << std::endl;
    std::cout << "- DECIMATE: Keep every N-th frame of the sequence file." << std::endl;
    std::cout << "  Requires --decimation-factor." << std::endl;
    std::cout << "- APPEND: Append multiple sequence files (one after the other)." << std::endl;
    std::cout << "  Set input files with the --source-seq-files parameter." << std::endl;
    std::cout << "- MIX: Merge fields stored in multiple sequence files." << std::endl;
    std::cout << "  Timepoints are defined by the first sequence. Image data is taken from the first sequence." << std::endl;
    std::cout << "  No interpolation is performed, fields are copied from the frame with the closest timestamp." << std::endl;
    std::cout << "  Set input files with the --source-seq-files parameter." << std::endl;

    std::cout << "- FILL_IMAGE_RECTANGLE: Fill a rectangle in the image (useful for removing patient data from sequences)." << std::endl;
    std::cout << "  Requires --rect-origin, --rect-size, and --fill-gray-level. E.g., --rect-origin 12 34 --rect-size 56 78)" << std::endl;
    std::cout << "- CROP: Crop a rectangle in the image (useful for cropping b-mode image from the data obtained via frame-grabber)." << std::endl;
    std::cout << "  Requires --rect-origin and --rect-size. (e.g., --rect-size 56 78). Optional: --flip*." << std::endl;

    std::cout << "- REMOVE_IMAGE_DATA: Remove image data from a meta file that has both image and tracker data, and keep only the tracker data." << std::endl;

    return EXIT_SUCCESS;
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  // Check command line arguments
  if (inputFileName.empty() && inputFileNames.empty())
  {
    LOG_ERROR("At least one input file name is mandantory!");
    return EXIT_FAILURE;
  }

  if (outputFileName.empty())
  {
    LOG_ERROR("Please set output file name!");
    return EXIT_FAILURE;
  }

  // Set operation
  if (strOperation.empty())
  {
    operation = NO_OPERATION;
    LOG_INFO("No modification operation has been specified (specify --operation parameter to change the input sequence).");
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "UPDATE_FRAME_FIELD_NAME"))
  {
    operation = UPDATE_FRAME_FIELD_NAME;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "UPDATE_FRAME_FIELD_VALUE"))
  {
    operation = UPDATE_FRAME_FIELD_VALUE;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "DELETE_FRAME_FIELD"))
  {
    operation = DELETE_FRAME_FIELD;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "UPDATE_FIELD_NAME"))
  {
    operation = UPDATE_FIELD_NAME;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "UPDATE_FIELD_VALUE"))
  {
    operation = UPDATE_FIELD_VALUE;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "DELETE_FIELD"))
  {
    operation = DELETE_FIELD;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "ADD_TRANSFORM"))
  {
    operation = ADD_TRANSFORM;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "TRIM"))
  {
    operation = TRIM;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "DECIMATE"))
  {
    operation = DECIMATE;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "APPEND"))
  {
    operation = APPEND;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "MERGE"))
  {
    LOG_WARNING("MERGE operation name is deprecated. Use APPEND instead.")
    operation = APPEND;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "MIX"))
  {
    operation = MIX;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "FILL_IMAGE_RECTANGLE"))
  {
    operation = FILL_IMAGE_RECTANGLE;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "CROP"))
  {
    if (rectOriginPix.size() != 2 && rectOriginPix.size() != 3 &&
        rectSizePix.size() != 2 && rectSizePix.size() != 3)
    {
      LOG_ERROR("--rect-origin and --rect-size must be of the form --rect-origin X Y <Z> and --rect-size I J <K>")
      return EXIT_FAILURE;
    }
    operation = CROP;
  }
  else if (igsioCommon::IsEqualInsensitive(strOperation, "REMOVE_IMAGE_DATA"))
  {
    operation = REMOVE_IMAGE_DATA;
  }
  else
  {
    LOG_ERROR("Invalid operation selected: " << strOperation);
    return EXIT_FAILURE;
  }

  // Convert strings transforms to vtkMatrix
  if (ConvertStringToMatrix(strFrameTransformStart, frameTransformStart) != PLUS_SUCCESS)
  {
    LOG_ERROR("String to matrix conversion failed for transform start matrix!");
    return EXIT_FAILURE;
  }

  if (ConvertStringToMatrix(strFrameTransformIncrement, frameTransformIncrement) != PLUS_SUCCESS)
  {
    LOG_ERROR("String to matrix conversion failed for transform increment matrix!");
    return EXIT_FAILURE;
  }

  ///////////////////////////////////////////////////////////////////
  // Read input files

  vtkSmartPointer<vtkIGSIOTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();

  if (!inputFileName.empty())
  {
    // Insert file name to the beginning of the list
    inputFileNames.insert(inputFileNames.begin(), inputFileName);
  }

  // Multiple input files are appended unless sequences are mixed
  PlusStatus status = PLUS_SUCCESS;
  if (operation == MIX)
  {
    status = MixTrackedFrameLists(trackedFrameList, inputFileNames);
  }
  else
  {
    status = AppendTrackedFrameLists(trackedFrameList, inputFileNames, incrementTimestamps);
  }
  if (status == PLUS_FAIL)
  {
    return EXIT_FAILURE;
  }

  ///////////////////////////////////////////////////////////////////
  // Make the operation

  switch (operation)
  {
  case NO_OPERATION:
  case APPEND:
  case MIX:
  {
    // No need to do anything just save into output file
  }
  break;
  case TRIM:
  {
    if (firstFrameIndex < 0)
    {
      firstFrameIndex = 0;
    }
    if (lastFrameIndex < 0)
    {
      lastFrameIndex = 0;
    }
    unsigned int firstFrameIndexUint = static_cast<unsigned int>(firstFrameIndex);
    unsigned int lastFrameIndexUint = static_cast<unsigned int>(lastFrameIndex);
    if (TrimSequenceFile(trackedFrameList, firstFrameIndexUint, lastFrameIndexUint) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to trim sequence file");
      return EXIT_FAILURE;
    }
  }
  break;
  case DECIMATE:
  {
    if (DecimateSequenceFile(trackedFrameList, decimationFactor) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to decimate sequence file");
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

    if (UpdateFrameFieldValue(fieldUpdate) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update frame field name '" << fieldName << "' to '" << updatedFieldName << "'");
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
    fieldUpdate.FrameTransformIndexFieldName = strFrameTransformIndexFieldName;

    if (UpdateFrameFieldValue(fieldUpdate) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update frame field value");
      return EXIT_FAILURE;
    }
  }
  break;
  case DELETE_FRAME_FIELD:
  {
    if (DeleteFrameField(trackedFrameList, fieldName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to delete frame field");
      return EXIT_FAILURE;
    }
  }
  break;
  case DELETE_FIELD:
  {
    // Delete field
    LOG_INFO("Delete field: " << fieldName);
    if (trackedFrameList->SetCustomString(fieldName.c_str(), NULL) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to delete field: " << fieldName);
      return EXIT_FAILURE;
    }
  }
  break;
  case UPDATE_FIELD_NAME:
  {
    // Update field name
    LOG_INFO("Update field name '" << fieldName << "' to  '" << updatedFieldName << "'");
    const char* fieldValue = trackedFrameList->GetCustomString(fieldName.c_str());
    if (fieldValue != NULL)
    {
      // Delete field
      if (trackedFrameList->SetCustomString(fieldName.c_str(), NULL) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to delete field: " << fieldName);
        return EXIT_FAILURE;
      }

      // Add new field
      if (trackedFrameList->SetCustomString(updatedFieldName.c_str(), fieldValue) != PLUS_SUCCESS)
      {
        LOG_ERROR("Failed to update field '" << updatedFieldName << "' with value '" << fieldValue << "'");
        return EXIT_FAILURE;
      }
    }
  }
  break;
  case UPDATE_FIELD_VALUE:
  {
    // Update field value
    LOG_INFO("Update field '" << fieldName << "' with value '" << updatedFieldValue << "'");
    if (trackedFrameList->SetCustomString(fieldName.c_str(), updatedFieldValue.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to update field '" << fieldName << "' with value '" << updatedFieldValue << "'");
      return EXIT_FAILURE;
    }
  }
  break;
  case ADD_TRANSFORM:
  {
    // Add transform
    LOG_INFO("Add transform '" << transformNamesToAdd << "' using device set configuration file '" << deviceSetConfigurationFileName << "'");
    std::vector<std::string> transformNamesList;
    igsioCommon::SplitStringIntoTokens(transformNamesToAdd, ',', transformNamesList);
    if (AddTransform(trackedFrameList, transformNamesList, deviceSetConfigurationFileName) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to add transform '" << transformNamesToAdd << "' using device set configuration file '" << deviceSetConfigurationFileName << "'");
      return EXIT_FAILURE;
    }
  }
  break;
  case FILL_IMAGE_RECTANGLE:
  {
    if (rectOriginPix.size() != 2 || rectSizePix.size() != 2)
    {
      LOG_ERROR("Incorrect size of vector for rectangle origin or size. Aborting.");
      return PLUS_FAIL;
    }
    if (rectOriginPix[0] < 0 || rectOriginPix[1] < 0 || rectSizePix[0] < 0 || rectSizePix[1] < 0)
    {
      LOG_ERROR("Negative value for rectangle origin or size entered. Aborting.");
      return PLUS_FAIL;
    }
    std::vector<unsigned int> rectOriginPixUint(rectOriginPix.begin(), rectOriginPix.end());
    std::vector<unsigned int> rectSizePixUint(rectSizePix.begin(), rectSizePix.end());
    // Fill a rectangular region in the image with a solid color
    if (FillRectangle(trackedFrameList, rectOriginPixUint, rectSizePixUint, fillGrayLevel) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to fill rectangle");
      return EXIT_FAILURE;
    }
  }
  break;
  case CROP:
  {
    // Crop a rectangular region from the image
    igsioVideoFrame::FlipInfoType flipInfo;
    flipInfo.hFlip = flipX;
    flipInfo.vFlip = flipY;
    flipInfo.eFlip = flipZ;
    if (CropRectangle(trackedFrameList, flipInfo, rectOriginPix, rectSizePix) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to fill rectangle");
      return EXIT_FAILURE;
    }
  }
  break;
  case REMOVE_IMAGE_DATA:
    // No processing is needed, image data is removed when writing the output
    break;
  default:
  {
    LOG_WARNING("Unknown operation is specified: " << strOperation);
    return EXIT_FAILURE;
  }
  }


  //////////////////////////////////////////////////////////////////
  // Convert files to the new file format

  if (!strUpdatedReferenceTransformName.empty())
  {
    igsioTransformName referenceTransformName;
    if (referenceTransformName.SetTransformName(strUpdatedReferenceTransformName.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Reference transform name is invalid: " << strUpdatedReferenceTransformName);
      return EXIT_FAILURE;
    }

    for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
    {
      igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i);

      vtkSmartPointer<vtkMatrix4x4> referenceToTrackerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if (trackedFrame->GetFrameTransform(referenceTransformName, referenceToTrackerMatrix) != PLUS_SUCCESS)
      {
        LOG_WARNING("Couldn't get reference transform with name: " << strUpdatedReferenceTransformName);
        continue;
      }

      std::vector<igsioTransformName> transformNameList;
      trackedFrame->GetFrameTransformNameList(transformNameList);

      vtkSmartPointer<vtkTransform> toolToTrackerTransform = vtkSmartPointer<vtkTransform>::New();
      vtkSmartPointer<vtkMatrix4x4> toolToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      for (unsigned int n = 0; n < transformNameList.size(); ++n)
      {
        // No need to change the reference transform
        if (transformNameList[n] == referenceTransformName)
        {
          continue;
        }

        ToolStatus status = TOOL_INVALID;
        if (trackedFrame->GetFrameTransform(transformNameList[n], toolToReferenceMatrix) != PLUS_SUCCESS)
        {
          std::string strTransformName;
          transformNameList[i].GetTransformName(strTransformName);
          LOG_ERROR("Failed to get frame transform: " << strTransformName);
          continue;
        }

        if (trackedFrame->GetFrameTransformStatus(transformNameList[n], status) != PLUS_SUCCESS)
        {
          std::string strTransformName;
          transformNameList[i].GetTransformName(strTransformName);
          LOG_ERROR("Failed to get frame transform status: " << strTransformName);
          continue;
        }

        // Compute ToolToTracker transform from ToolToReference
        toolToTrackerTransform->Identity();
        toolToTrackerTransform->Concatenate(referenceToTrackerMatrix);
        toolToTrackerTransform->Concatenate(toolToReferenceMatrix);

        // Update the name to ToolToTracker
        igsioTransformName toolToTracker(transformNameList[n].From().c_str(), "Tracker");
        // Set the new custom transform
        if (trackedFrame->SetFrameTransform(toolToTracker, toolToTrackerTransform->GetMatrix()) != PLUS_SUCCESS)
        {
          std::string strTransformName;
          transformNameList[i].GetTransformName(strTransformName);
          LOG_ERROR("Failed to set frame transform: " << strTransformName);
          continue;
        }

        // Use the same status as it was before
        if (trackedFrame->SetFrameTransformStatus(toolToTracker, status) != PLUS_SUCCESS)
        {
          std::string strTransformName;
          transformNameList[i].GetTransformName(strTransformName);
          LOG_ERROR("Failed to set frame transform status: " << strTransformName);
          continue;
        }

        // Delete old transform and status fields
        std::string oldTransformName, oldTransformStatus;
        transformNameList[n].GetTransformName(oldTransformName);
        // Append Transform to the end of the transform name
        vtksys::RegularExpression isTransform("Transform$");
        if (!isTransform.find(oldTransformName))
        {
          oldTransformName.append("Transform");
        }
        oldTransformStatus = oldTransformName;
        oldTransformStatus.append("Status");
        trackedFrame->DeleteFrameField(oldTransformName.c_str());
        trackedFrame->DeleteFrameField(oldTransformStatus.c_str());

      }
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Save output file to file

  LOG_INFO("Save output sequence file to: " << outputFileName);
  if (vtkPlusSequenceIO::Write(outputFileName, trackedFrameList, trackedFrameList->GetImageOrientation(), useCompression, operation != REMOVE_IMAGE_DATA) != PLUS_SUCCESS)
  {
    LOG_ERROR("Couldn't write sequence file: " << outputFileName);
    return EXIT_FAILURE;
  }

  LOG_INFO("Sequence file editing was successful!");
  return EXIT_SUCCESS;
}

//-------------------------------------------------------
PlusStatus TrimSequenceFile(vtkIGSIOTrackedFrameList* aTrackedFrameList, unsigned int aFirstFrameIndex, unsigned int aLastFrameIndex)
{
  LOG_INFO("Trim sequence file from frame #: " << aFirstFrameIndex << " to frame #" << aLastFrameIndex);
  if (aLastFrameIndex >= aTrackedFrameList->GetNumberOfTrackedFrames() || aFirstFrameIndex > aLastFrameIndex)
  {
    LOG_ERROR("Invalid input range: (" << aFirstFrameIndex << ", " << aLastFrameIndex << ")" << " Permitted range within (0, " << aTrackedFrameList->GetNumberOfTrackedFrames() - 1 << ")");
    return PLUS_FAIL;
  }

  if (aLastFrameIndex != aTrackedFrameList->GetNumberOfTrackedFrames() - 1)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(aLastFrameIndex + 1, aTrackedFrameList->GetNumberOfTrackedFrames() - 1);
  }

  if (aFirstFrameIndex != 0)
  {
    aTrackedFrameList->RemoveTrackedFrameRange(0, aFirstFrameIndex - 1);
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus DecimateSequenceFile(vtkIGSIOTrackedFrameList* aTrackedFrameList, unsigned int decimationFactor)
{
  LOG_INFO("Decimate sequence file: keep 1 frame out of every " << decimationFactor << " frames");
  if (decimationFactor < 2)
  {
    LOG_ERROR("Invalid decimation factor: " << decimationFactor << ". It must be an integer larger or equal than 2.");
    return PLUS_FAIL;
  }
  for (unsigned int i = 0; i < aTrackedFrameList->GetNumberOfTrackedFrames() - 1; i++)
  {
    unsigned int removeFirstFrameIndex = i + 1;
    unsigned int removeLastFrameIndex = i + decimationFactor - 1;
    if (removeLastFrameIndex >= aTrackedFrameList->GetNumberOfTrackedFrames())
    {
      removeLastFrameIndex = aTrackedFrameList->GetNumberOfTrackedFrames() - 1;
      if (removeLastFrameIndex < removeFirstFrameIndex)
      {
        removeLastFrameIndex = removeFirstFrameIndex;
      }
    }
    aTrackedFrameList->RemoveTrackedFrameRange(removeFirstFrameIndex, removeLastFrameIndex);
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus DeleteFrameField(vtkIGSIOTrackedFrameList* trackedFrameList, std::string fieldName)
{
  if (trackedFrameList == NULL)
  {
    LOG_ERROR("Tracked frame list is NULL!");
    return PLUS_FAIL;
  }

  if (fieldName.empty())
  {
    LOG_ERROR("Field name is empty!");
    return PLUS_FAIL;
  }

  LOG_INFO("Delete frame field: " << fieldName);
  int numberOfErrors(0);
  for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i);

    /////////////////////////////////
    // Delete field name
    const char* fieldValue = trackedFrame->GetFrameField(fieldName.c_str());
    if (fieldValue != NULL
        && trackedFrame->DeleteFrameField(fieldName.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to delete frame field '" << fieldName << "' for frame #" << i);
      numberOfErrors++;
    }
  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}


//-------------------------------------------------------
PlusStatus UpdateFrameFieldValue(FrameFieldUpdate& fieldUpdate)
{
  LOG_INFO("Update frame field");
  int numberOfErrors(0);

  // Set the start scalar value
  double scalarVariable = fieldUpdate.FrameScalarStart;

  // Set the start transform matrix
  vtkSmartPointer<vtkTransform> frameTransform = vtkSmartPointer<vtkTransform>::New();
  if (fieldUpdate.FrameTransformStart != NULL)
  {
    frameTransform->SetMatrix(fieldUpdate.FrameTransformStart);
  }

  for (unsigned int i = 0; i < fieldUpdate.TrackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = fieldUpdate.TrackedFrameList->GetTrackedFrame(i);

    /////////////////////////////////
    // Update field name
    if (!fieldUpdate.FieldName.empty() && !fieldUpdate.UpdatedFieldName.empty())
    {
      const char* fieldValue = trackedFrame->GetFrameField(fieldUpdate.FieldName.c_str());
      if (fieldValue != NULL)
      {
        std::string copyOfFieldValue(fieldValue);
        trackedFrame->DeleteFrameField(fieldUpdate.FieldName.c_str());
        trackedFrame->SetFrameField(fieldUpdate.UpdatedFieldName.c_str(), copyOfFieldValue.c_str());
      }
    }

    std::string fieldName = fieldUpdate.FieldName;
    if (!fieldUpdate.UpdatedFieldName.empty())
    {
      fieldName = fieldUpdate.UpdatedFieldName;
    }

    /////////////////////////////////
    // Update field value
    if (!fieldName.empty() && !fieldUpdate.UpdatedFieldValue.empty())
    {
      if (igsioCommon::IsEqualInsensitive(fieldUpdate.UpdatedFieldValue, FIELD_VALUE_FRAME_SCALAR))
      {
        // Update it as a scalar variable

        std::ostringstream fieldValue;
        fieldValue << std::fixed << std::setprecision(fieldUpdate.FrameScalarDecimalDigits) << scalarVariable;

        trackedFrame->SetFrameField(fieldName.c_str(), fieldValue.str().c_str());
        scalarVariable += fieldUpdate.FrameScalarIncrement;

      }
      else if (igsioCommon::IsEqualInsensitive(fieldUpdate.UpdatedFieldValue, FIELD_VALUE_FRAME_TRANSFORM))
      {
        // Update it as a transform variable

        double transformMatrix[16] = { 0 };
        if (fieldUpdate.FrameTransformIndexFieldName.empty())
        {
          vtkMatrix4x4::DeepCopy(transformMatrix, frameTransform->GetMatrix());
        }
        else
        {
          const char* frameIndexStr = trackedFrame->GetFrameField(fieldUpdate.FrameTransformIndexFieldName.c_str());
          int frameIndex = 0;
          if (igsioCommon::StringToInt<int>(frameIndexStr, frameIndex) != PLUS_SUCCESS)
          {
            LOG_ERROR("Cannot retrieve frame index from value " << frameIndexStr);
          }
          vtkSmartPointer<vtkMatrix4x4> cumulativeTransform = vtkSmartPointer<vtkMatrix4x4>::New();
          cumulativeTransform->DeepCopy(fieldUpdate.FrameTransformStart);
          for (int i = 0; i < frameIndex; i++)
          {
            vtkMatrix4x4::Multiply4x4(fieldUpdate.FrameTransformIncrement, cumulativeTransform, cumulativeTransform);
          }
          vtkMatrix4x4::DeepCopy(transformMatrix, cumulativeTransform);

        }

        std::ostringstream strTransform;
        strTransform << std::fixed << std::setprecision(fieldUpdate.FrameScalarDecimalDigits)
                     << transformMatrix[0] << " " << transformMatrix[1] << " " << transformMatrix[2] << " " << transformMatrix[3] << " "
                     << transformMatrix[4] << " " << transformMatrix[5] << " " << transformMatrix[6] << " " << transformMatrix[7] << " "
                     << transformMatrix[8] << " " << transformMatrix[9] << " " << transformMatrix[10] << " " << transformMatrix[11] << " "
                     << transformMatrix[12] << " " << transformMatrix[13] << " " << transformMatrix[14] << " " << transformMatrix[15] << " ";
        trackedFrame->SetFrameField(fieldName.c_str(), strTransform.str().c_str());

        if (fieldUpdate.FrameTransformIndexFieldName.empty())
        {
          frameTransform->Concatenate(fieldUpdate.FrameTransformIncrement);
        }

      }
      else // Update only as a string value
      {
        trackedFrame->SetFrameField(fieldName.c_str(), fieldUpdate.UpdatedFieldValue.c_str());
      }
    }

  }

  return (numberOfErrors == 0 ? PLUS_SUCCESS : PLUS_FAIL);
}

//-------------------------------------------------------
PlusStatus ConvertStringToMatrix(std::string& strMatrix, vtkMatrix4x4* matrix)
{
  if (matrix == NULL)
  {
    LOG_ERROR("Failed to convert string to matrix - output matrix is NULL!");
    return PLUS_FAIL;
  }

  if (!strMatrix.empty())
  {
    double transformMatrix[16] = { 0 };
    std::istringstream transform(strMatrix);
    double item;
    int i = 0;
    while (transform >> item && i < 16)
    {
      transformMatrix[i++] = item;
    }
    matrix->DeepCopy(transformMatrix);
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus AddTransform(vtkIGSIOTrackedFrameList* trackedFrameList, std::vector<std::string> transformNamesToAdd, std::string deviceSetConfigurationFileName)
{
  if (trackedFrameList == NULL)
  {
    LOG_ERROR("Tracked frame list is invalid");
    return PLUS_FAIL;
  }

  if (transformNamesToAdd.empty())
  {
    LOG_ERROR("No transform names are specified to be added");
    return PLUS_FAIL;
  }

  if (deviceSetConfigurationFileName.empty())
  {
    LOG_ERROR("Used device set configuration file name is empty");
    return PLUS_FAIL;
  }

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, deviceSetConfigurationFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << deviceSetConfigurationFileName.c_str());
    return PLUS_FAIL;
  }

  for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i);

    // Set up transform repository
    vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
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

    for (std::vector<std::string>::iterator transformNameToAddIt = transformNamesToAdd.begin(); transformNameToAddIt != transformNamesToAdd.end(); ++transformNameToAddIt)
    {
      // Create transform name
      igsioTransformName transformName;
      transformName.SetTransformName(transformNameToAddIt->c_str());

      // Get transform matrix
      ToolStatus status(TOOL_INVALID);
      vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if (transformRepository->GetTransform(transformName, transformMatrix, &status) != PLUS_SUCCESS)
      {
        LOG_WARNING("Failed to get transform " << (*transformNameToAddIt) << " from tracked frame " << i);
        transformMatrix->Identity();
        status = TOOL_INVALID;
      }
      trackedFrame->SetFrameTransform(transformName, transformMatrix);
      trackedFrame->SetFrameTransformStatus(transformName, status);
    }
  }

  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus FillRectangle(vtkIGSIOTrackedFrameList* trackedFrameList, const std::vector<unsigned int>& fillRectOrigin, const std::vector<unsigned int>& fillRectSize, int fillGrayLevel)
{
  if (trackedFrameList == NULL)
  {
    LOG_ERROR("Tracked frame list is NULL!");
    return PLUS_FAIL;
  }
  if (fillRectOrigin.size() != 2 || fillRectSize.size() != 2)
  {
    LOG_ERROR("Fill rectangle origin or size is not specified correctly");
    return PLUS_FAIL;
  }

  for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i);
    igsioVideoFrame* videoFrame = trackedFrame->GetImageData();
    FrameSizeType frameSize = { 0, 0, 0 };
    if (videoFrame == NULL || videoFrame->GetFrameSize(frameSize) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve pixel data from frame " << i << ". Fill rectangle failed.");
      continue;
    }
    if (fillRectOrigin[0] >= frameSize[0] ||
        fillRectOrigin[1] >= frameSize[1])
    {
      LOG_ERROR("Invalid fill rectangle origin is specified (" << fillRectOrigin[0] << ", " << fillRectOrigin[1] << "). The image size is ("
                << frameSize[0] << ", " << frameSize[1] << ").");
      continue;
    }
    if (fillRectSize[0] <= 0 || fillRectOrigin[0] + fillRectSize[0] > frameSize[0] ||
        fillRectSize[1] <= 0 || fillRectOrigin[1] + fillRectSize[1] > frameSize[1])
    {
      LOG_ERROR("Invalid fill rectangle size is specified (" << fillRectSize[0] << ", " << fillRectSize[1] << "). The specified fill rectangle origin is ("
                << fillRectOrigin[0] << ", " << fillRectOrigin[1] << ") and the image size is (" << frameSize[0] << ", " << frameSize[1] << ").");
      continue;
    }
    if (videoFrame->GetVTKScalarPixelType() != VTK_UNSIGNED_CHAR)
    {
      LOG_ERROR("Fill rectangle is supported only for B-mode images (unsigned char type)");
      continue;
    }
    unsigned char fillData = 0;
    if (fillGrayLevel < 0)
    {
      fillData = 0;
    }
    else if (fillGrayLevel > 255)
    {
      fillData = 255;
    }
    else
    {
      fillData = fillGrayLevel;
    }
    for (unsigned int y = 0; y < fillRectSize[1]; y++)
    {
      memset(static_cast<unsigned char*>(videoFrame->GetScalarPointer()) + (fillRectOrigin[1] + y)*frameSize[0] + fillRectOrigin[0], fillData, fillRectSize[0]);
    }
  }
  return PLUS_SUCCESS;
}

//-------------------------------------------------------
PlusStatus CropRectangle(vtkIGSIOTrackedFrameList* trackedFrameList, igsioVideoFrame::FlipInfoType& flipInfo, const std::vector<int>& cropRectOrigin, const std::vector<int>& cropRectSize)
{
  if (trackedFrameList == NULL)
  {
    LOG_ERROR("Tracked frame list is NULL!");
    return PLUS_FAIL;
  }
  std::array<int, 3> rectOrigin = { cropRectOrigin[0], cropRectOrigin[1], cropRectOrigin.size() == 3 ? cropRectOrigin[2] : 0 };
  std::array<int, 3> rectSize = { cropRectSize[0], cropRectSize[1], cropRectSize.size() == 3 ? cropRectSize[2] : 1 };

  vtkSmartPointer<vtkMatrix4x4> tfmMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  tfmMatrix->Identity();
  tfmMatrix->SetElement(0, 3, -rectOrigin[0]);
  tfmMatrix->SetElement(1, 3, -rectOrigin[1]);
  tfmMatrix->SetElement(2, 3, -rectOrigin[2]);
  igsioTransformName imageToCroppedImage("Image", "CroppedImage");

  for (unsigned int i = 0; i < trackedFrameList->GetNumberOfTrackedFrames(); ++i)
  {
    igsioTrackedFrame* trackedFrame = trackedFrameList->GetTrackedFrame(i);
    igsioVideoFrame* videoFrame = trackedFrame->GetImageData();

    FrameSizeType frameSize = { 0, 0, 0 };
    if (videoFrame == NULL || videoFrame->GetFrameSize(frameSize) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to retrieve pixel data from frame " << i << ". Crop rectangle failed.");
      continue;
    }

    vtkSmartPointer<vtkImageData> croppedImage = vtkSmartPointer<vtkImageData>::New();

    igsioVideoFrame::FlipClipImage(videoFrame->GetImage(), flipInfo, rectOrigin, rectSize, croppedImage);
    videoFrame->DeepCopyFrom(croppedImage);
    trackedFrame->SetFrameTransform(imageToCroppedImage, tfmMatrix);
    trackedFrame->SetFrameTransformStatus(imageToCroppedImage, TOOL_OK);
  }

  return PLUS_SUCCESS;
}
