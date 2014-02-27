/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"

#include "vtkMetaImageSequenceIO.h"
#include "itkImage.h"

#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"


///////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

  std::string inputImageSequenceFileName;
  std::string outputImageSequenceFileName;

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  int numberOfFailures(0); 

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--img-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence.");
  args.AddArgument("--output-img-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageSequenceFileName, "Filename of the output image sequence.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputImageSequenceFileName.empty())
  {
    std::cerr << "--img-seq-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (outputImageSequenceFileName.empty())
  {
    std::cerr << "output-img-seq-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkMetaImageSequenceIO> reader=vtkSmartPointer<vtkMetaImageSequenceIO>::New();        
  reader->SetFileName(inputImageSequenceFileName.c_str());
  if (reader->Read()!=PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't read sequence metafile: " <<  inputImageSequenceFileName ); 
    return EXIT_FAILURE;
  }    
  vtkTrackedFrameList* trackedFrameList = reader->GetTrackedFrameList();

  if (trackedFrameList==NULL)
  {
    LOG_ERROR("Unable to get trackedFrameList!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Test GetCustomString method ..."); 
  const char* imgOrientation = trackedFrameList->GetCustomString("UltrasoundImageOrientation"); 
  if ( imgOrientation == NULL )
  {
    LOG_ERROR("Unable to get custom string!"); 
    numberOfFailures++; 
  }

  LOG_INFO("Test GetCustomTransform method ..."); 
  double tImageToTool[16]; 
  if ( !trackedFrameList->GetCustomTransform("ImageToToolTransform", tImageToTool) )
  {
    LOG_ERROR("Unable to get custom transform!"); 
    numberOfFailures++;   
  }

  // Create an absolute path to the output image sequence, in the output directory
  outputImageSequenceFileName=vtkPlusConfig::GetInstance()->GetOutputPath(outputImageSequenceFileName);

  // ****************************************************************************** 
  // Test writing

  vtkSmartPointer<vtkMetaImageSequenceIO> writer=vtkSmartPointer<vtkMetaImageSequenceIO>::New();      
  writer->UseCompressionOn();
  writer->SetFileName(outputImageSequenceFileName.c_str());
  writer->SetTrackedFrameList(trackedFrameList); 

  LOG_INFO("Test SetCustomFrameTransform method ..."); 
  // Add the transformation matrix to metafile
  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  for ( int i = 0 ; i < numberOfFrames; i++ ) 
  {
    vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    transMatrix->SetElement(0,0,i); 
    writer->GetTrackedFrame(i)->SetCustomFrameTransform(
      PlusTransformName("Tool", "Tracker"),
      transMatrix); 
  }

  vtkSmartPointer<vtkMatrix4x4> highPrecMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  highPrecMatrix->SetElement(0,0,0.12345678901234567890); 
  highPrecMatrix->SetElement(0,1,1.12345678901234567890); 
  highPrecMatrix->SetElement(0,2,2.12345678901234567890); 
  highPrecMatrix->SetElement(1,0,10.12345678901234567890); 
  highPrecMatrix->SetElement(1,1,11.12345678901234567890); 
  highPrecMatrix->SetElement(1,2,12.12345678901234567890); 
  highPrecMatrix->SetElement(2,0,20.12345678901234567890); 
  highPrecMatrix->SetElement(2,1,21.12345678901234567890); 
  highPrecMatrix->SetElement(2,2,22.12345678901234567890); 
  highPrecMatrix->SetElement(0,3,12345.12345678901234567890); 
  highPrecMatrix->SetElement(0,3,23456.12345678901234567890); 
  highPrecMatrix->SetElement(0,3,34567.12345678901234567890);     

  const double highPrecTimeOffset=0.123456789012345678901234567890;

  PlusTransformName highPrecTransformName("HighPrecTool", "Tracker"); 
  for ( int i = 0 ; i < numberOfFrames; i++ ) 
  {
    vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    transMatrix->DeepCopy(highPrecMatrix);
    writer->GetTrackedFrame(i)->SetCustomFrameTransform(highPrecTransformName, transMatrix);     
    writer->GetTrackedFrame(i)->SetTimestamp(double(i)+highPrecTimeOffset);
  }

  LOG_INFO("Test SetCustomTransform method ..."); 
  vtkSmartPointer<vtkMatrix4x4> calibMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  calibMatrix->Identity(); 
  writer->GetTrackedFrameList()->SetCustomTransform("ImageToToolTransform", calibMatrix); 

  if (writer->Write()!=PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't write sequence metafile: " <<  outputImageSequenceFileName ); 
    return EXIT_FAILURE;
  }  

  PlusTransformName tnToolToTracker("Tool", "Tracker"); 
  LOG_INFO("Test GetFrameTransform method ..."); 
  for ( int i = 0; i < numberOfFrames; i++ )
  {
    vtkSmartPointer<vtkMatrix4x4> writerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkMatrix4x4> readerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    if ( !reader->GetTrackedFrameList()->GetTrackedFrame(i)->GetCustomFrameTransform(tnToolToTracker, readerMatrix) )
    {
      LOG_ERROR("Unable to get ToolToTracker frame transform to frame #" << i); 
      numberOfFailures++; 
    }

    if ( !writer->GetTrackedFrame(i)->GetCustomFrameTransform(tnToolToTracker, writerMatrix) )
    {
      LOG_ERROR("Unable to get ToolToTracker frame transform to frame #" << i); 
      numberOfFailures++; 
    }

    for (int row = 0; row < 4; row++)
    {
      for (int col = 0; col < 4; col++)
      {
        if ( readerMatrix->GetElement(row, col) != writerMatrix->GetElement(row, col) ) 
        {
          LOG_ERROR("The input and output matrices are not the same at element: (" << row << ", " << col << "). ");
          numberOfFailures++; 
        }
      }
    }

    if ( !reader->GetTrackedFrameList()->GetTrackedFrame(i)->GetCustomFrameTransform(highPrecTransformName, readerMatrix) )
    {
      LOG_ERROR("Unable to get high precision frame transform for frame #" << i); 
      numberOfFailures++; 
    }
    const double FLOAT_COMPARISON_TOLERANCE=1e-12;
    for (int row = 0; row < 4; row++)
    {
      for (int col = 0; col < 4; col++)
      {

        if ( fabs(readerMatrix->GetElement(row, col)-highPrecMatrix->GetElement(row, col))>FLOAT_COMPARISON_TOLERANCE ) 
        {
          LOG_ERROR("The input and output matrices are not the same at element: (" << row << ", " << col << "). ");
          numberOfFailures++; 
        }
      }
    }
    double readerTimestamp=reader->GetTrackedFrameList()->GetTrackedFrame(i)->GetTimestamp();
    double expectedTimestamp=double(i)+highPrecTimeOffset;
    if (fabs(expectedTimestamp-readerTimestamp)>FLOAT_COMPARISON_TOLERANCE)
    {
      LOG_ERROR("The timestamp is not precise enough at frame "<<i);
      numberOfFailures++; 
    }

  }

  // ****************************************************************************** 
  // Test image status 

  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  matrix->SetElement(0,3, -50); 
  matrix->SetElement(0,3, 150); 

  vtkSmartPointer<vtkTrackedFrameList> dummyTrackedFrame = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  TrackedFrame validFrame; 
  int frameSize[2]={200,200}; 
  validFrame.GetImageData()->AllocateFrame(frameSize, VTK_UNSIGNED_CHAR, 1); 
  validFrame.GetImageData()->FillBlank(); 
  validFrame.SetCustomFrameTransform(PlusTransformName("Image", "Probe"), matrix); 
  validFrame.SetCustomFrameField("FrameNumber", "0"); 
  validFrame.SetTimestamp(1.0); 

  TrackedFrame invalidFrame; 
  invalidFrame.SetCustomFrameTransform(PlusTransformName("Image", "Probe"), matrix); 
  invalidFrame.SetCustomFrameField("FrameNumber", "1"); 
  invalidFrame.SetTimestamp(2.0); 

  TrackedFrame validFrame_copy(validFrame); 
  validFrame_copy.SetTimestamp(3.0); 
  validFrame_copy.SetCustomFrameField("FrameNumber", "3"); 

  dummyTrackedFrame->AddTrackedFrame(&validFrame); 
  dummyTrackedFrame->AddTrackedFrame(&invalidFrame); 
  dummyTrackedFrame->AddTrackedFrame(&validFrame_copy); 

  vtkSmartPointer<vtkMetaImageSequenceIO> writerImageStatus=vtkSmartPointer<vtkMetaImageSequenceIO>::New();      
  writerImageStatus->SetFileName(outputImageSequenceFileName.c_str());
  writerImageStatus->SetTrackedFrameList(dummyTrackedFrame); 
  writerImageStatus->UseCompressionOn();

  if (writerImageStatus->Write()!=PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't write sequence metafile: " <<  outputImageSequenceFileName ); 
    return EXIT_FAILURE;
  }  

  vtkSmartPointer<vtkMetaImageSequenceIO> readerImageStatus=vtkSmartPointer<vtkMetaImageSequenceIO>::New();        
  readerImageStatus->SetFileName(outputImageSequenceFileName.c_str());
  if (readerImageStatus->Read()!=PLUS_SUCCESS)
  {    
    LOG_ERROR("Couldn't read sequence metafile: " <<  outputImageSequenceFileName ); 
    return EXIT_FAILURE;
  }    
  vtkTrackedFrameList* trackedFrameListImageStatus=readerImageStatus->GetTrackedFrameList();
  if (trackedFrameListImageStatus==NULL)
  {
    LOG_ERROR("Unable to get trackedFrameList!"); 
    return EXIT_FAILURE;
  }

  if ( trackedFrameListImageStatus->GetNumberOfTrackedFrames() != 3  
    && !trackedFrameListImageStatus->GetTrackedFrame(1)->GetImageData()->IsImageValid() )
  {
    LOG_ERROR("Image status read/write failed!"); 
    return EXIT_FAILURE;
  }

  // Test metafile writting with different sized images 

  TrackedFrame differentSizeFrame; 
  int frameSizeSmaller[2]={150,150}; 
  differentSizeFrame.GetImageData()->AllocateFrame(frameSizeSmaller, VTK_UNSIGNED_CHAR, 1); 
  differentSizeFrame.GetImageData()->FillBlank(); 
  differentSizeFrame.SetCustomFrameTransform(PlusTransformName("Image", "Probe"), matrix); 
  differentSizeFrame.SetCustomFrameField("FrameNumber", "6"); 
  differentSizeFrame.SetTimestamp(6.0); 
  dummyTrackedFrame->AddTrackedFrame(&differentSizeFrame); 

  vtkSmartPointer<vtkMetaImageSequenceIO> writerDiffSize=vtkSmartPointer<vtkMetaImageSequenceIO>::New();      
  writerDiffSize->SetFileName(outputImageSequenceFileName.c_str());
  writerDiffSize->SetTrackedFrameList(dummyTrackedFrame); 
  writerDiffSize->UseCompressionOff();

  // We should get an error when trying to write a sequence with different frame sizes into file
  int oldVerboseLevel=vtkPlusLogger::Instance()->GetLogLevel();
  vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_ERROR-1); // temporarily disable error logging (as we are expecting an error)
  if (writerDiffSize->Write()==PLUS_SUCCESS)
  {    
    vtkPlusLogger::Instance()->SetLogLevel(oldVerboseLevel);
    LOG_ERROR("Expect a 'Frame size mismatch' error in vtkMetaImageSequenceIO but the operation has been reported to be successful." ); 
    return EXIT_FAILURE;
  }
  vtkPlusLogger::Instance()->SetLogLevel(oldVerboseLevel);  

  if ( numberOfFailures > 0 )
  {
    LOG_ERROR("Total number of failures: " << numberOfFailures ); 
    LOG_ERROR("vtkMetaImageSequenceIOTest1 failed!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("vtkMetaImageSequenceIOTest1 completed successfully!"); 
  return EXIT_SUCCESS; 
}
