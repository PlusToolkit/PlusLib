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

///////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

	std::string inputImageSequenceFileName;
	std::string outputImageSequenceFileName;

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	int numberOfFailures(0); 
	
	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence.");
	args.AddArgument("--output-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageSequenceFileName, "Filename of the output image sequence.");
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
		std::cerr << "input-img-seq-file-name is required" << std::endl;
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
  vtkTrackedFrameList* trackedFrameList=reader->GetTrackedFrameList();
  if (trackedFrameList==NULL)
	{
		LOG_ERROR("Unable to get trackedFrameList!"); 
		return EXIT_FAILURE;
	}

	LOG_INFO("Test GetCustomString method ..."); 
	const char* defaultTransformName = trackedFrameList->GetCustomString("DefaultFrameTransformName"); 
	if ( defaultTransformName == NULL )
	{
		LOG_ERROR("Unable to get custom string!"); 
		numberOfFailures++; 
	}

	LOG_INFO("Test GetDefaultFrameTransformName method ..."); 
  std::string strDefaultTransformName; 
  if ( trackedFrameList->GetDefaultFrameTransformName().GetTransformName(strDefaultTransformName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Default transform name is invalid!"); 
    numberOfFailures++; 
  }

  if ( strcmp(strDefaultTransformName.c_str(), defaultTransformName) > 0 )
	{
		LOG_ERROR("Unable to get default frame transform name!"); 
		numberOfFailures++; 
	}

	LOG_INFO("Test GetCustomTransform method ..."); 
	double tImageToTool[16]; 
  if ( !trackedFrameList->GetCustomTransform("ImageToToolTransform", tImageToTool) )
	{
		LOG_ERROR("Unable to get custom transform!"); 
		numberOfFailures++; 	
	}
	
	// ****************************************************************************** 
  // Test writing

  vtkSmartPointer<vtkMetaImageSequenceIO> writer=vtkSmartPointer<vtkMetaImageSequenceIO>::New();			
	writer->SetFileName(outputImageSequenceFileName.c_str());
	writer->SetTrackedFrameList(trackedFrameList); 
	writer->UseCompressionOn();

	LOG_INFO("Test SetDefaultFrameTransform method ..."); 
  PlusTransformName defaultTransformToTracker("DefaultTransform", "Tracker"); 
	writer->SetDefaultFrameTransformName(defaultTransformToTracker); 

	LOG_INFO("Test SetFrameTransform method ..."); 
	// Add the transformation matrix to metafile
  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		transMatrix->SetElement(0,0,i); 
    writer->GetTrackedFrame(i)->SetCustomFrameTransform(
      writer->GetTrackedFrameList()->GetDefaultFrameTransformName(),
      transMatrix); 
	}

	LOG_INFO("Test SetCustomFrameTransform method ..."); 
  PlusTransformName customTransformName("CustomTransform", "Tracker"); 
	for ( int i = 0 ; i < numberOfFrames; i++ ) 
	{
		vtkSmartPointer<vtkMatrix4x4> transMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		transMatrix->SetElement(0,0,i); 
    writer->GetTrackedFrame(i)->SetCustomFrameTransform(customTransformName, transMatrix); 
	}
		
	LOG_INFO("Test SetCustomTransform method ..."); 
	vtkSmartPointer<vtkMatrix4x4> calibMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	calibMatrix->Identity(); 
  writer->GetTrackedFrameList()->SetCustomTransform("ImageToToolTransform", calibMatrix); 

  if (writer->Write()!=PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't write sequence metafile: " <<  writer->GetFileName() ); 
  	return EXIT_FAILURE;
	}	


  PlusTransformName defaultName = writer->GetDefaultFrameTransformName(); 

	LOG_INFO("Test GetFrameTransform method ..."); 
	for ( int i = 0; i < numberOfFrames; i++ )
	{
		vtkSmartPointer<vtkMatrix4x4> writerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkSmartPointer<vtkMatrix4x4> readerMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
		
    if ( !reader->GetTrackedFrameList()->GetTrackedFrame(i)->GetCustomFrameTransform(
      reader->GetTrackedFrameList()->GetDefaultFrameTransformName(), readerMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform to frame #" << i); 
			numberOfFailures++; 
		}

    if ( !writer->GetTrackedFrame(i)->GetCustomFrameTransform(defaultName, writerMatrix) )
		{
			LOG_ERROR("Unable to get default frame transform to frame #" << i); 
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
		
	}

	if ( numberOfFailures > 0 )
	{
		LOG_ERROR("Total number of failures: " << numberOfFailures ); 
		LOG_ERROR("vtkMetaImageSequenceIOTest1 failed!"); 
		return EXIT_FAILURE;
	}

	LOG_INFO("vtkMetaImageSequenceIOTest1 completed successfully!"); 
	return EXIT_SUCCESS; 
 }
