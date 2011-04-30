#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkDataCollector.h"

#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkImageViewer2.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkTextActor.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridGeometryFilter.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkPolyVertex.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetMapper.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"
#include "vtkDiskSource.h"
#include "vtkBMPReader.h"
#include "vtkImageFlip.h"
#include "vtkSmartPointer.h"
#include "vtkActorCollection.h"
#include "vtkCollectionIterator.h"
#include "vtkImageActor.h"
#include "vtkCamera.h"
#include "vtkBMPWriter.h"
#include "vtkWindowToImageFilter.h"

enum IMAGING_MODE
{
	TRANSVERSE, 
	LONGITUDINAL
}; 

enum TEMPLATE_HOLE_REPRESENTATION
{
	HOLE_DISK, 
	HOLE_SPHERE
}; 

enum TEMPLATE_LETTER_TYPE
{
	LETTER_CHAR, 
	LETTER_NUM
};

struct TemplateModel
{
	const char* ID; 
	double PositionX; 
	double PositionY; 
	double PositionZ; 
	TEMPLATE_HOLE_REPRESENTATION Representation; 
	TEMPLATE_LETTER_TYPE LetterType; 
	double Radius;

}; 

struct TemplateGridActors
{
	vtkSmartPointer<vtkCollection> TransverseGridActors;
	vtkSmartPointer<vtkCollection> LongitudinalGridActors;
	vtkSmartPointer<vtkCollection> TransverseLetterActors; 
	vtkSmartPointer<vtkCollection> LongitudinalLetterActors;
}; 

vtkDataCollector* dataCollector = NULL; 
vtkImageViewer2 *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;
TemplateGridActors templateGridActors; 
IMAGING_MODE ImigingMode = TRANSVERSE;

vtkTransform * TransformUSImageToProbe;
vtkTransform * TransformProbeToTracker;
vtkTransform * TransformTrackerToTemplateHolder;
vtkTransform * TransformTemplateHolderToTemplate;
vtkTransform * TransformUSImageToTemplate;

int iRotationAngle = 0; 
int FrameOriginX = 0; 
int FrameOriginY = 0; 
int ImageHeightInPx = 0; 
int ImageWidthInPx = 0; 


vtkTextActor *textActor = NULL; 

vtkTransform * ReadUSFrameToProbeTransformation(const char* filename);
void ReadCalibrationResultFile(const char* filename ); 
void ReadTemplateModelFile(vtkXMLDataElement *configTemplateModel); 
void ReadTemplateModelLetters(vtkXMLDataElement *config, std::vector<TemplateModel>& lettersTemplateModel); 
void ReadTemplateModelHoles(vtkXMLDataElement *config, std::vector<TemplateModel>& holesTemplateModel);
void CreateTemplateGridActors(vtkXMLDataElement *configTemplateModel, TemplateGridActors &templateGridActors);
void DisplayTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer); 
void RemoveTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer); 

class vtkMyCallback : public vtkCommand
{
public:
	static vtkMyCallback *New()
	{return new vtkMyCallback;}
	virtual void Execute(vtkObject *caller, unsigned long callerevent, void*)
	{
		if (callerevent == vtkCommand::CharEvent)
		{
			char keycode=iren->GetKeyCode();
			switch (keycode)
			{ 
			case 't':
				{
					if (ImigingMode != TRANSVERSE)
					{
						// Remove longitudinal grid
						RemoveTemplateGrid(templateGridActors.LongitudinalGridActors, templateGridActors.LongitudinalLetterActors, viewer->GetRenderer());
						// Display transverse grid
						DisplayTemplateGrid(templateGridActors.TransverseGridActors, templateGridActors.TransverseLetterActors, viewer->GetRenderer());
						iRotationAngle = 0; 
						ImigingMode = TRANSVERSE; 
					}
					break;

				} 
			case 'l':
				{
					if (ImigingMode != LONGITUDINAL)
					{
						// Remove transverse grid
						RemoveTemplateGrid(templateGridActors.TransverseGridActors, templateGridActors.TransverseLetterActors, viewer->GetRenderer());
						// Display longitudinal grid
						DisplayTemplateGrid(templateGridActors.LongitudinalGridActors, templateGridActors.LongitudinalLetterActors, viewer->GetRenderer());
						iRotationAngle = 0; 
						ImigingMode = LONGITUDINAL; 
					}
					break;
				} 
			}
		}

		std::ostringstream ss;
		ss.precision( 2 ); 
		vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 
		tFrame2Tracker = dataCollector->GetToolTransMatrix();

		ss	<< std::fixed 
			<< tFrame2Tracker->GetElement(0,0) << "   " << tFrame2Tracker->GetElement(0,1) << "   " << tFrame2Tracker->GetElement(0,2) << "   " << tFrame2Tracker->GetElement(0,3) << "\n"
			<< tFrame2Tracker->GetElement(1,0) << "   " << tFrame2Tracker->GetElement(1,1) << "   " << tFrame2Tracker->GetElement(1,2) << "   " << tFrame2Tracker->GetElement(1,3) << "\n"
			<< tFrame2Tracker->GetElement(2,0) << "   " << tFrame2Tracker->GetElement(2,1) << "   " << tFrame2Tracker->GetElement(2,2) << "   " << tFrame2Tracker->GetElement(2,3) << "\n"
			<< tFrame2Tracker->GetElement(3,0) << "   " << tFrame2Tracker->GetElement(3,1) << "   " << tFrame2Tracker->GetElement(3,2) << "   " << tFrame2Tracker->GetElement(3,3) << "\n"; 

		textActor->SetInput(ss.str().c_str());
		textActor->Modified(); 

		TransformProbeToTracker->SetMatrix(tFrame2Tracker); 
		TransformProbeToTracker->Update(); 

		//vtkWindowToImageFilter * wif = vtkWindowToImageFilter::New(); 
		//wif->SetInput(viewer->GetRenderWindow()); 
		////int *asize = viewer->GetRenderWindow()->GetActualSize(); 
		////std::ostringstream fn;
		////fn << "Probe_" << dProbePosition << "_Grid_" << dGridPosition << "_Rotation_" << dRotation << "_Size_" << asize[0]<< "by"<< asize[1]<<".bmp";
		//std::ostringstream fn;
		//fn << "Probe_" << dProbePosition << "_Grid_" << dGridPosition << "_Rotation_" << dRotation <<".bmp";
		//vtkBMPWriter* w = vtkBMPWriter::New(); 
		//w->SetFileName(fn.str().c_str()); 
		//w->SetInput(USFrame); 
		////w->SetInput(wif->GetOutput()); 
		//w->Update(); 
		//w->Delete(); 

		//int* dim = USFrame->GetDimensions();
		//viewer->GetImageActor()->SetOrigin(dim[0]/2, dim[1]/2, 0); 
		//viewer->GetImageActor()->SetOrientation(0,0,iRotationAngle); 
		//viewer->GetImageActor()->Modified(); 

		//if (ImigingMode == TRANSVERSE)
		//{
		//	viewer->GetImageActor()->SetOrientation(-1.0 * TransformProbeToTracker->GetOrientation()[2], 0,0);  
		//	//viewer->GetImageActor()->RotateZ( -1.0 * TransformProbeToTracker->GetOrientation()[2]); 
		//	viewer->GetImageActor()->Modified(); 
		//}


		viewer->Render();
		//update the timer so it will trigger again
		iren->CreateTimer(VTKI_TIMER_UPDATE);
	}
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	bool printHelp(false); 
	std::string inputConfigFileName; 
	std::string inputCalibrationResultFileName; 
	std::string inputTemplateModelFileName; 

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	VTK_LOG_TO_CONSOLE_ON; 

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Path to the data collection configuration file.");
	args.AddArgument("--input-calibration-result-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputCalibrationResultFileName, "Path to the calibration result file.");
	args.AddArgument("--input-template-model-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTemplateModelFileName, "Path to the template model file.");
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 3; 1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "\n\nHelp:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 
	}

	if ( inputConfigFileName.empty() || inputCalibrationResultFileName.empty() || inputTemplateModelFileName.empty() )
	{
		std::cerr << "input-config-file-name, input-calibration-result-file-name and input-template-model-file-name parameters are required" << std::endl;
		exit(EXIT_FAILURE);
	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);


	///////////////////////////////////////////////////

	LOG_DEBUG("Initialize data collector..."); 
	dataCollector = vtkDataCollector::New(); 
	dataCollector->ReadConfiguration(inputConfigFileName.c_str());
	dataCollector->Initialize(); 

	// Transformations
	TransformProbeToTracker = vtkTransform::New();
	TransformTrackerToTemplateHolder = vtkTransform::New();
	TransformTemplateHolderToTemplate = vtkTransform::New(); 
	TransformUSImageToProbe = vtkTransform::New();
	TransformUSImageToTemplate = vtkTransform::New(); 

	// Read USFrameToProbe Transformation from file 
	LOG_DEBUG("Read calibration result file..."); 
	ReadCalibrationResultFile(inputCalibrationResultFileName.c_str()); 

	// Read Template model file 
	LOG_DEBUG("Read template model file..."); 
	vtkXMLDataElement *configTemplateModel = vtkXMLUtilities::ReadElementFromFile(inputTemplateModelFileName.c_str()); 
	ReadTemplateModelFile(configTemplateModel); 

	// Read Probe2Tracker transformation from tracking device
	vtkSmartPointer<vtkMatrix4x4> tProbe2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 
	tProbe2Tracker = dataCollector->GetToolTransMatrix(); 
	TransformProbeToTracker->SetMatrix(tProbe2Tracker); 
	TransformProbeToTracker->Update(); 

	std::ostringstream osTransformProbeToTracker; 
	TransformProbeToTracker->Print(osTransformProbeToTracker); 
	LOG_DEBUG("TransformProbeToTracker:\n" << osTransformProbeToTracker.str()); 
	
	std::ostringstream osTransformTrackerToTemplateHolder; 
	TransformTrackerToTemplateHolder->Print(osTransformTrackerToTemplateHolder);  
	LOG_DEBUG("TransformTrackerToTemplateHolder:\n" << osTransformTrackerToTemplateHolder.str());

	TransformUSImageToTemplate->PostMultiply(); 
	TransformUSImageToTemplate->Concatenate(TransformUSImageToProbe); 
	TransformUSImageToTemplate->Concatenate(TransformProbeToTracker); 
	TransformUSImageToTemplate->Concatenate(TransformTrackerToTemplateHolder); 
	TransformUSImageToTemplate->Concatenate(TransformTemplateHolderToTemplate);
	TransformUSImageToTemplate->Update(); 

	std::ostringstream osTransformUSImageToTemplate; 
	TransformUSImageToTemplate->Print(osTransformUSImageToTemplate);  
	LOG_DEBUG( "TransformUSImageToTemplate:\n" << osTransformUSImageToTemplate.str());

	viewer = vtkImageViewer2::New();
	viewer->SetColorWindow(255);
	viewer->SetColorLevel(127.5);
	viewer->SetSize(800,600); 
	viewer->SetInput(dataCollector->GetOutput()); 
	viewer->GetImageActor()->VisibilityOn();

	viewer->GetImageActor()->SetPosition(-FrameOriginX,-FrameOriginY,0); 
	viewer->GetImageActor()->SetUserTransform(TransformUSImageToTemplate);
	
	// Create template grid actors
	templateGridActors.TransverseGridActors = vtkSmartPointer<vtkCollection>::New(); 
	templateGridActors.TransverseLetterActors = vtkSmartPointer<vtkCollection>::New(); 
	templateGridActors.LongitudinalGridActors = vtkSmartPointer<vtkCollection>::New(); 
	templateGridActors.LongitudinalLetterActors = vtkSmartPointer<vtkCollection>::New(); 

	CreateTemplateGridActors(configTemplateModel, templateGridActors); 

	// Display the default transverse mode
	DisplayTemplateGrid(templateGridActors.TransverseGridActors, templateGridActors.TransverseLetterActors, viewer->GetRenderer()); 

	//Create the interactor that handles the event loop
	iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(viewer->GetRenderWindow());
	viewer->SetupInteractor(iren);

	// Create a text actor for tracking information
	textActor=vtkTextActor::New();
	vtkSmartPointer<vtkTextProperty> textprop = textActor->GetTextProperty();
	textprop->SetColor(1,0,0);
	textprop->SetFontFamilyToArial();
	textprop->SetFontSize(15);
	textprop->SetJustificationToLeft();
	textprop->SetVerticalJustificationToTop();

	textActor->VisibilityOn(); 
	textActor->SetDisplayPosition(10,80); 

	viewer->GetRenderer()->AddActor(textActor); 
	viewer->GetRenderer()->SetBackground(1,1,1);

	viewer->Render();	//must be called after iren and viewer are linked
	//or there will be problems

	//establish timer event and create timer
	vtkSmartPointer<vtkMyCallback> call = vtkMyCallback::New();
	iren->AddObserver(vtkCommand::TimerEvent, call);
	iren->AddObserver(vtkCommand::CharEvent, call);
	iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

	viewer->GetRenderer()->GetActiveCamera()->ParallelProjectionOn(); 
	double* displayBounds = viewer->GetImageActor()->GetBounds(); 
	viewer->GetRenderer()->ResetCamera(); 
	viewer->GetRenderer()->GetActiveCamera()->SetParallelScale( (displayBounds[3] - displayBounds[2]) / 2.0 ); 

	//iren must be initialized so that it can handle events
	iren->Initialize();
	iren->Start();

	//delete all instances and release the hold the win32videosource
	//has on the pci card	
	viewer->Delete();
	iren->Delete();
	call->Delete(); 

	TransformProbeToTracker->Delete();
	TransformTrackerToTemplateHolder->Delete();
	TransformTemplateHolderToTemplate->Delete(); 
	TransformUSImageToProbe->Delete();
	TransformUSImageToTemplate->Delete();
	textActor->Delete(); 
	configTemplateModel->Delete(); 

	dataCollector->Delete(); 

	return EXIT_SUCCESS;
}

//----------------------------------------------------------------------
void ReadCalibrationResultFile(const char* filename )
{
	vtkXMLDataElement *xmlFile = vtkXMLUtilities::ReadElementFromFile(filename); 
	
	if ( xmlFile == NULL ) 
	{
		LOG_ERROR("Unable to read calibration file: " << filename ); 
		exit (EXIT_FAILURE); 
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationResults = xmlFile->FindNestedElementWithName("CalibrationResults"); 
	if ( calibrationResults == NULL ) 
	{
		LOG_ERROR("Unable to read calibration results from file: " << filename ); 
		exit (EXIT_FAILURE); 
	}

	vtkSmartPointer<vtkXMLDataElement> ultrasoundImageDimensions = calibrationResults->FindNestedElementWithName("UltrasoundImageDimensions"); 
	int imageWidth(0); 
	if ( ultrasoundImageDimensions->GetScalarAttribute("Width", imageWidth) ) 
	{
		ImageWidthInPx = imageWidth; 
	}

	int imageHeight(0); 
	if ( ultrasoundImageDimensions->GetScalarAttribute("Height", imageHeight) ) 
	{
		ImageHeightInPx = imageHeight; 
	}

	vtkSmartPointer<vtkXMLDataElement> ultrasoundImageOrigin = calibrationResults->FindNestedElementWithName("UltrasoundImageOrigin"); 
	int originX(0); 
	if ( ultrasoundImageOrigin->GetScalarAttribute("OriginX", originX) ) 
	{
		FrameOriginX = originX; 
	}

	int originY(0); 
	if ( ultrasoundImageOrigin->GetScalarAttribute("OriginY", originY) ) 
	{
		FrameOriginY = ImageHeightInPx - originY; 
	}
	
	vtkSmartPointer<vtkXMLDataElement> calibrationTransform = calibrationResults->FindNestedElementWithName("CalibrationTransform"); 

	double usImageFrameToProbe[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
	if ( calibrationTransform->GetVectorAttribute("USImageFrameToProbe", 16, usImageFrameToProbe) )
	{
		TransformUSImageToProbe->SetMatrix(usImageFrameToProbe); 

		std::ostringstream osTransformUSImageToProbe; 
		TransformUSImageToProbe->Print(osTransformUSImageToProbe);  
		LOG_DEBUG( "TransformUSImageToProbe:\n" << osTransformUSImageToProbe.str());
	}

	double trackerToTemplateHolder[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
	if ( calibrationTransform->GetVectorAttribute("TrackerToTemplateHolder", 16, trackerToTemplateHolder) )
	{
		TransformTrackerToTemplateHolder->SetMatrix(trackerToTemplateHolder); 

		std::ostringstream osTransformTrackerToTemplateHolder; 
		TransformTrackerToTemplateHolder->Print(osTransformTrackerToTemplateHolder);  
		LOG_DEBUG( "TransformTrackerToTemplateHolder:\n" << osTransformTrackerToTemplateHolder.str());
	}

	xmlFile->Delete(); 
}

void ReadTemplateModelFile(vtkXMLDataElement *configTemplateModel)
{

	if ( configTemplateModel == NULL ) 
	{
		LOG_ERROR("Unable to read template model file!"); 
		exit (EXIT_FAILURE); 
	}

	vtkSmartPointer<vtkXMLDataElement> originFromTemplateHolder = configTemplateModel->FindNestedElementWithName("OriginFromTemplateHolder"); 
	if ( originFromTemplateHolder == NULL ) 
	{
		LOG_ERROR("Unable to read template origin from template holder from template model file!"); 
		exit (EXIT_FAILURE); 
	}

	double originX(0); 
	if ( originFromTemplateHolder->GetScalarAttribute("OriginX",originX) )
	{
		TransformTemplateHolderToTemplate->Translate(originX, 0, 0); 

	}

	double originY(0); 
	if ( originFromTemplateHolder->GetScalarAttribute("OriginY",originY) )
	{
		TransformTemplateHolderToTemplate->Translate(0, originY, 0); 

	}

	std::ostringstream osTransformTemplateHolderToTemplate; 
	TransformTemplateHolderToTemplate->Print(osTransformTemplateHolderToTemplate);  
	LOG_DEBUG( "TransformTemplateHolderToTemplate:\n" << osTransformTemplateHolderToTemplate.str());


}

// Obsolete
//----------------------------------------------------------------------
vtkTransform* ReadUSFrameToProbeTransformation(const char* filename)
{	
	std::string line; 
	ifstream ifs( filename );
	if (!ifs.good()) 
	{
		return NULL; 
	}

	while( getline( ifs, line ) )
	{
		if ( line.find("# [ConvertedTo8Parameters-USImageFrameToUSProbeFrame]")!= std::string::npos)
		{
			getline( ifs, line );
			getline( ifs, line );
			break;
		}
	}

	ifs.close(); 

	std::stringstream ss(line);
	double value[8]; 
	int i = 0;
	//# FORMAT: Alpha,Beta,Gamma (in radians); Sx,Sy (in meters/pixel); Tx,Ty,Tz (in meters).
	while (std::getline(ss, line, '\t'))
	{
		value[i++] = atof(line.c_str());
	}

	TransformUSImageToProbe->Translate(value[5]*1000, value[6]*1000, value[7]*1000);

	const double PI = 4.0*atan(1.0); 
	TransformUSImageToProbe->RotateZ(value[0]*180.0/PI);
	TransformUSImageToProbe->RotateY(value[1]*180.0/PI);
	TransformUSImageToProbe->RotateX(value[2]*180.0/PI);

	TransformUSImageToProbe->Scale((value[3]*1000),(value[4]*1000), 0);

	TransformUSImageToProbe->Update(); 
	TransformUSImageToProbe->PrintSelf(std::cout, vtkIndent(0));

	return TransformUSImageToProbe; 
}


// Create Template Grid overlaid on the US video
//----------------------------------------------------------------------
void CreateTemplateGridActors(vtkXMLDataElement *configTemplateModel, TemplateGridActors &templateGridActors)
{
	if (configTemplateModel != NULL)
	{
		std::vector<TemplateModel> holesTemplateModel; 

		ReadTemplateModelHoles(configTemplateModel, holesTemplateModel); 
		const double XLONGOFFSET_HOLE = 2.5;

		for (unsigned int i = 0; i < holesTemplateModel.size(); i++)
		{
			// Transverse actors
			vtkSmartPointer<vtkActor> holeTransActor = vtkSmartPointer<vtkActor>::New();
			vtkSmartPointer<vtkPolyDataMapper> holeTransMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

			if (holesTemplateModel[i].Representation == HOLE_SPHERE ) 
			{
				vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
				sphere->SetRadius(holesTemplateModel[i].Radius); 
				holeTransMapper->SetInput(sphere->GetOutput());
			}
			else if (holesTemplateModel[i].Representation == HOLE_DISK)
			{
				vtkSmartPointer<vtkDiskSource> point = vtkSmartPointer<vtkDiskSource>::New();
				point->SetInnerRadius(holesTemplateModel[i].Radius-0.15); 
				point->SetOuterRadius(holesTemplateModel[i].Radius); 
				point->SetCircumferentialResolution(30); 
				holeTransMapper->SetInput(point->GetOutput());
			}


			holeTransActor->SetMapper(holeTransMapper);
			holeTransActor->GetProperty()->SetColor(0,1,0);

			holeTransActor->SetPosition(holesTemplateModel[i].PositionX, -holesTemplateModel[i].PositionY ,-holesTemplateModel[i].PositionZ ); 
			holeTransActor->Modified(); 

			//holeTransActor->SetUserTransform(TransformProbeToTracker); 

			templateGridActors.TransverseGridActors->AddItem(holeTransActor); 

			// Longitude actors
			vtkSmartPointer<vtkPolyDataMapper> holeLongMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			vtkSmartPointer<vtkActor> holeLongActor1 = vtkSmartPointer<vtkActor>::New();
			vtkSmartPointer<vtkActor> holeLongActor2 = vtkSmartPointer<vtkActor>::New();


			vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
			sphere->SetRadius(0.2); 
			holeLongMapper->SetInput(sphere->GetOutput());

			holeLongActor1->SetMapper(holeLongMapper); 
			holeLongActor1->GetProperty()->SetColor(0,1,0);


			holeLongActor1->SetPosition(holesTemplateModel[i].PositionX , -holesTemplateModel[i].PositionY , -holesTemplateModel[i].PositionZ); 
			holeLongActor1->Modified(); 
			//holeLongActor1->SetUserTransform(TransformUSImageToTemplate); 

			holeLongActor2->SetMapper(holeLongMapper); 
			holeLongActor2->GetProperty()->SetColor(0,1,0);
			holeLongActor2->SetPosition(holesTemplateModel[i].PositionX + XLONGOFFSET_HOLE, -holesTemplateModel[i].PositionY , -holesTemplateModel[i].PositionZ); 
			holeLongActor2->Modified(); 
			//holeLongActor2->SetUserTransform(TransformUSImageToTemplate); 

			templateGridActors.LongitudinalGridActors->AddItem(holeLongActor1);
			templateGridActors.LongitudinalGridActors->AddItem(holeLongActor2);
		}


		// --------------------  template letters ------------------------ //

		const double XTRANSOFFSET_LETTER = 0.9; 
		const double YTRANSOFFSET_LETTER = 1.3; 
		const double XLONGOFFSET_LETTER_CHAR = 0.9; 
		const double YLONGOFFSET_LETTER_CHAR = 71.3; 
		const double XLONGOFFSET_LETTER_NUM = 0.9; 
		const double YLONGOFFSET_LETTER_NUM = 1.3; 

		std::vector<TemplateModel> lettersTemplateModel; 

		ReadTemplateModelLetters(configTemplateModel, lettersTemplateModel); 

		for (unsigned int i = 0; i < lettersTemplateModel.size(); i++)
		{
			// Transverse actors
			vtkSmartPointer<vtkTextActor3D> gridTransLettersActor = vtkSmartPointer<vtkTextActor3D>::New();
			gridTransLettersActor->GetTextProperty()->SetColor(0,1,0);
			gridTransLettersActor->GetTextProperty()->SetFontFamilyToArial();
			gridTransLettersActor->GetTextProperty()->SetFontSize(16);
			gridTransLettersActor->GetTextProperty()->SetJustificationToLeft();
			gridTransLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
			gridTransLettersActor->GetTextProperty()->BoldOn(); 

			double * imageScaleTrans = TransformUSImageToTemplate->GetScale(); 
			gridTransLettersActor->SetScale(imageScaleTrans[0], imageScaleTrans[1], imageScaleTrans[2]);
			//gridTransLettersActor->SetScale(0.2,0.2,0.2); 
			gridTransLettersActor->SetInput(lettersTemplateModel[i].ID);
			gridTransLettersActor->SetPosition(lettersTemplateModel[i].PositionX - XTRANSOFFSET_LETTER,-lettersTemplateModel[i].PositionY - YTRANSOFFSET_LETTER,-lettersTemplateModel[i].PositionZ); 
			gridTransLettersActor->Modified(); 

			//gridTransLettersActor->SetUserTransform(TransformUSImageToTemplate); 


			templateGridActors.TransverseLetterActors->AddItem(gridTransLettersActor); 

			// Longitude actors

			vtkSmartPointer<vtkTextActor3D> gridLongLettersActor = vtkSmartPointer<vtkTextActor3D>::New();
			if (lettersTemplateModel[i].LetterType == LETTER_CHAR)
			{
				//gridLongLettersActor->SetPosition(lettersTemplateModel[i].PositionX - XLONGOFFSET_LETTER_CHAR,-lettersTemplateModel[i].PositionY - YLONGOFFSET_LETTER_CHAR,-lettersTemplateModel[i].PositionZ ); 
				//gridLongLettersActor->GetTextProperty()->SetColor(0,1,0);
				//gridLongLettersActor->GetTextProperty()->SetFontFamilyToArial();
				//gridLongLettersActor->GetTextProperty()->SetFontSize(16);
				//gridLongLettersActor->GetTextProperty()->SetJustificationToLeft();
				//gridLongLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
				//gridLongLettersActor->GetTextProperty()->BoldOn(); 

				//gridLongLettersActor->SetInput(lettersTemplateModel[i].ID);
			}
			else
			{
				gridLongLettersActor->GetTextProperty()->SetColor(0,1,0);
				gridLongLettersActor->GetTextProperty()->SetFontFamilyToArial();
				gridLongLettersActor->GetTextProperty()->SetFontSize(16);
				gridLongLettersActor->GetTextProperty()->SetJustificationToLeft();
				gridLongLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
				gridLongLettersActor->GetTextProperty()->BoldOn(); 
				gridLongLettersActor->SetInput(lettersTemplateModel[i].ID);
				gridLongLettersActor->SetPosition(lettersTemplateModel[i].PositionX - XLONGOFFSET_LETTER_NUM,-lettersTemplateModel[i].PositionY - YLONGOFFSET_LETTER_NUM,-lettersTemplateModel[i].PositionZ ); 
			}

			gridLongLettersActor->Modified(); 

			//gridLongLettersActor->SetUserTransform(TransformUSImageToTemplate); 
			double * imageScaleLong = TransformUSImageToTemplate->GetScale(); 
			gridLongLettersActor->SetScale(imageScaleLong[0], imageScaleLong[1], imageScaleLong[2]);

			templateGridActors.LongitudinalLetterActors->AddItem(gridLongLettersActor); 
		}

	}

}

//----------------------------------------------------------------------
void DisplayTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer)
{
	for (int i = 0; i < gridActors->GetNumberOfItems(); i++) 
	{
		renderer->AddActor(static_cast<vtkActor*>(gridActors->GetItemAsObject(i)));
	}

	for (int i = 0; i < letterActors->GetNumberOfItems(); i++) 
	{
		renderer->AddActor(static_cast<vtkTextActor3D*>(letterActors->GetItemAsObject(i)));
	}
}


//----------------------------------------------------------------------
void RemoveTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer)
{
	for (int i = 0; i < gridActors->GetNumberOfItems(); i++) 
	{
		renderer->RemoveActor(static_cast<vtkActor*>(gridActors->GetItemAsObject(i)));
	}

	for (int i = 0; i < letterActors->GetNumberOfItems(); i++) 
	{
		renderer->RemoveActor(static_cast<vtkTextActor3D*>(letterActors->GetItemAsObject(i)));
	}
}


//----------------------------------------------------------------------
void DisplayVideo(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer)
{
	iren->SetRenderWindow(viewer->GetRenderWindow());
	viewer->SetupInteractor(iren);
}


//----------------------------------------------------------------------
void DisplayTrackingData(vtkCollection* gridActors, vtkCollection* letterActors, vtkRenderer* renderer)
{
	for (int i = 0; i < gridActors->GetNumberOfItems(); i++) 
	{
		renderer->AddActor(static_cast<vtkActor*>(gridActors->GetItemAsObject(i)));
	}

	for (int i = 0; i < letterActors->GetNumberOfItems(); i++) 
	{
		renderer->AddActor(static_cast<vtkTextActor3D*>(letterActors->GetItemAsObject(i)));
	}
}


//----------------------------------------------------------------------
void ReadTemplateModelLetters(vtkXMLDataElement *config, std::vector<TemplateModel>& lettersTemplateModel)
{
	vtkSmartPointer<vtkXMLDataElement> letterPositions=config->FindNestedElementWithName("LetterPositions");
	if (letterPositions!=NULL)
	{ 

		for (int i=0; i<letterPositions->GetNumberOfNestedElements(); i++)
		{ 
			TemplateModel letterModel; 
			vtkXMLDataElement *elem=letterPositions->GetNestedElement(i);
			if (STRCASECMP("Letter", elem->GetName())!=0)
			{
				// not a Letter element
				continue;
			} 

			const char* id=elem->GetAttribute("ID");
			if (id!=0)
			{
				letterModel.ID = id; 
			} 

			double positionXYZ[3]; 
			if (elem->GetVectorAttribute("PositionXYZ",3,positionXYZ))
			{
				letterModel.PositionX = positionXYZ[0]; 
				letterModel.PositionY = positionXYZ[1]; 
				letterModel.PositionZ = positionXYZ[2];
			} 

			const char* type = elem->GetAttribute("Type"); 
			if (type != NULL && STRCASECMP("Char", type )==0)
			{	
				letterModel.LetterType = LETTER_CHAR; 
			}
			else 
			{
				letterModel.LetterType = LETTER_NUM; 
			}


			lettersTemplateModel.push_back(letterModel); 
		}
	}
}


//----------------------------------------------------------------------
void ReadTemplateModelHoles(vtkXMLDataElement *config, std::vector<TemplateModel>& holesTemplateModel)
{
	TEMPLATE_HOLE_REPRESENTATION holeRepresentation = HOLE_SPHERE; 
	TEMPLATE_LETTER_TYPE letterType = LETTER_NUM; 
	double holeRadius=0.2; 


	vtkSmartPointer<vtkXMLDataElement> templateHole=config->FindNestedElementWithName("TemplateHole");
	if (templateHole != NULL) 
	{
		const char* representation = templateHole->GetAttribute("Representation"); 
		if (representation != NULL && STRCASECMP("Disk", representation)==0)
		{	
			holeRepresentation = HOLE_DISK; 
		}
		else 
		{
			holeRepresentation = HOLE_SPHERE; 
		}

		templateHole->GetScalarAttribute("Radius", holeRadius); 

	}

	vtkSmartPointer<vtkXMLDataElement> holePositions=config->FindNestedElementWithName("HolePositions");
	if (holePositions!=NULL)
	{ 

		for (int i=0; i<holePositions->GetNumberOfNestedElements(); i++)
		{ 
			TemplateModel holeModel; 
			vtkXMLDataElement *elem=holePositions->GetNestedElement(i);
			if (STRCASECMP("Hole", elem->GetName())!=0)
			{
				// not a Hole element
				continue;
			} 

			const char* id=elem->GetAttribute("ID");
			if (id!=0)
			{
				holeModel.ID = id; 
			} 

			double positionXYZ[3]; 
			if (elem->GetVectorAttribute("PositionXYZ",3,positionXYZ))
			{
				holeModel.PositionX = positionXYZ[0]; 
				holeModel.PositionY = positionXYZ[1]; 
				holeModel.PositionZ = positionXYZ[2];
			} 

			holeModel.Representation = holeRepresentation; 
			holeModel.Radius = holeRadius; 
			holeModel.LetterType = letterType; 
			holesTemplateModel.push_back(holeModel); 
		}
	}
}

