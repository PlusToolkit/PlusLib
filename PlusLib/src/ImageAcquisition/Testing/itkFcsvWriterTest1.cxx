#include "PlusConfigure.h"

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

//The following include to make Visual Studio 6 happy
#include "itkImageHelper.h"

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "itkFcsvReader.h"
#include "itkFcsvWriter.h"
#include "itkLandmarkSpatialObject.h"

int main(int argc, char *argv[])
{
	vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG);  

	itk::FcsvReader::Pointer reader = itk::FcsvReader::New();

	reader->SetFileName(argv[1]);
	reader->Update();

	LOG_DEBUG("Number Of Points: "<< reader->GetFcsvDataObject()->points.size());
	if (reader->GetFcsvDataObject()->points.size()!=18)
	{
		LOG_ERROR("Number Of Points: mismatch");
		return EXIT_FAILURE;
	}

	{
		int fidIndex=0;
		std::vector<itk::FcsvPoint>::iterator it = reader->GetFcsvDataObject()->points.begin(); 
		while( it != reader->GetFcsvDataObject()->points.end() )
		{
			LOG_DEBUG("Point position: " << (*it).position);
			if (fidIndex==2)
			{
				(*it).position[0]=100;
				(*it).position[1]=200;
				(*it).position[2]=300;
				(*it).selected=0;
				(*it).visibility=1;
			}
			++it;
			fidIndex++;
		}
	}


	// Write the updated fcsv data

	itk::FcsvWriter::Pointer writer = itk::FcsvWriter::New();
	writer->SetFileName(argv[2]);
	*writer->GetFcsvDataObject() = *reader->GetFcsvDataObject();
	writer->Update();

	// Read the updated fcsv data

	itk::FcsvReader::Pointer reader2 = itk::FcsvReader::New();
	reader2->SetFileName(argv[2]);
	reader2->Update();

	LOG_DEBUG("Number Of Points: "<< reader2->GetFcsvDataObject()->points.size());
	if (reader2->GetFcsvDataObject()->points.size()!=18)
	{
		LOG_ERROR("Number Of Points: mismatch");
		return EXIT_FAILURE;
	}

	{
		int fidIndex=0;
		std::vector<itk::FcsvPoint>::iterator it = reader2->GetFcsvDataObject()->points.begin(); 
		while( it != reader2->GetFcsvDataObject()->points.end() )
		{
			LOG_DEBUG("Point position: " << (*it).position);
			if (fidIndex==2)
			{
				if ((*it).position[0]!=100)
				{
					LOG_ERROR("Position[0] mismatch");
					return EXIT_FAILURE;
				}
				if ((*it).position[1]!=200)
				{
					LOG_ERROR("Position[1] mismatch");
					return EXIT_FAILURE;
				}
				if ((*it).position[2]!=300)
				{
					LOG_ERROR("Position[2] mismatch");
					return EXIT_FAILURE;
				}
				if ((*it).selected!=0)
				{
					LOG_ERROR("selected mismatch");
					return EXIT_FAILURE;
				}
				if ((*it).visibility!=1)
				{
					LOG_ERROR("visibility mismatch");
					return EXIT_FAILURE;
				}
			}
			++it;
			fidIndex++;
		}
	}


	return EXIT_SUCCESS;
}
