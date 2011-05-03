#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"
#include "vtkImageViewer2.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkTextActor.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkImageFlip.h"
#include "vtkActorCollection.h"
#include "vtkCollectionIterator.h"
#include "vtkImageActor.h"
#include "vtkImageImport.h"
#include "vtkImageFlip.h"
#include "vtkProp.h"




#include "itkMetaImageSequenceIO.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

///////////////////////////////////////////////////////////////////
// Image type definition

typedef unsigned char          PixelType; // define type for pixel representation
const unsigned int             imageDimension = 2; 
const unsigned int             imageSequenceDimension = 3; 

typedef itk::Image< PixelType, imageDimension > ImageType;
typedef itk::Image< PixelType, 3 > ImageType3D;
typedef itk::Image< PixelType, imageSequenceDimension > ImageSequenceType;

typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
typedef itk::ImageFileWriter< ImageType3D > ImageWriterType;

///////////////////////////////////////////////////////////////////

vtkRenderWindow *renWin = NULL; 
vtkRenderWindowInteractor *iren = NULL;
vtkRenderer* renderer = NULL; 
vtkTextActor *textActor = NULL; 
vtkCollection *imageActors = NULL; 
std::vector<vtkTransform*> imageTransforms; 
vtkImageActor* currentActor = NULL; 
int frameNum = 0; 

void PrintProgressBar( int percent ); 

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
			case '+':
				{
					if ( ++frameNum >= imageActors->GetNumberOfItems() )
					{
						frameNum = 0; 
					}

					if  ( currentActor != NULL ) 
					{
						currentActor->VisibilityOff(); 
						static_cast<vtkImageActor*>(imageActors->GetItemAsObject(0))->VisibilityOn(); 
					}

					currentActor = static_cast<vtkImageActor*>(imageActors->GetItemAsObject(frameNum)); 
					currentActor->VisibilityOn(); 
				}
				break;
			case '-':
				{
					if ( --frameNum < 0 )
					{
						frameNum = imageActors->GetNumberOfItems() - 1; 
					}

					if  ( currentActor != NULL ) 
					{
						currentActor->VisibilityOff(); 
						static_cast<vtkImageActor*>(imageActors->GetItemAsObject(0))->VisibilityOn(); 
					}

					currentActor = static_cast<vtkImageActor*>(imageActors->GetItemAsObject(frameNum)); 
					currentActor->VisibilityOn(); 
				}
				break;
			}
		}

		double * pos = imageTransforms[frameNum]->GetPosition(); 
		std::ostringstream ss;
		ss.precision( 2 ); 
		ss << "Frame " << frameNum << "\nImage position: " << std::fixed << pos[0] << "  " << pos[1] << "  " << pos[2] << std::ends;
		textActor->SetInput(ss.str().c_str());

		renWin->Render(); 
		//update the timer so it will trigger again
		iren->CreateTimer(VTKI_TIMER_UPDATE);
	}
};

int main(int argc, char **argv)
{

	std::string inputImageSequenceFileName;
	bool renderingOff(false);
	int inputOriginX(0); 
	int inputOriginY(0); 

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence.");
	args.AddArgument("--input-origin-x", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputOriginX, "Image X origin in px (Default: 0)");
	args.AddArgument("--input-origin-y", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputOriginY, "Image Y origin in px (Default: 0)");
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (inputImageSequenceFileName.empty())
	{
		std::cerr << "input-img-seq-file-name is required" << std::endl;
		exit(EXIT_FAILURE);
	}

	///////////////

	renWin = vtkRenderWindow::New(); 
	renderer = vtkRenderer::New(); 
	renWin->AddRenderer(renderer); 

	// Create imageactor collection
	imageActors = vtkCollection::New(); 

	//Create the interactor that handles the event loop
	iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	LOG_INFO("Reading image sequence...");
	itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
	ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

	// Set the image IO 
	reader->SetImageIO(readerMetaImageSequenceIO); 
	reader->SetFileName(inputImageSequenceFileName.c_str());

	try
	{
		reader->Update(); 
	}
	catch (itk::ExceptionObject & err) 
	{		
		LOG_ERROR( "Sequence image reader couldn't update: " <<  err); 
		exit(EXIT_FAILURE);
	}	

	ImageSequenceType::Pointer imageSeq = reader->GetOutput();

	const unsigned long imageWidthInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[0]; 
	const unsigned long imageHeightInPixels = imageSeq->GetLargestPossibleRegion().GetSize()[1]; 
	unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	
	//numberOfFrames = 100; 
	unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * sizeof(PixelType);

	LOG_INFO("Adding frames to actors...");

	PixelType* imageSeqData = imageSeq->GetBufferPointer(); 
	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{
		PrintProgressBar( (100.0 * imgNumber) / numberOfFrames ); 
		unsigned char* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

		vtkSmartPointer<vtkMatrix4x4> imageTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		readerMetaImageSequenceIO->GetFrameTransform(imgNumber, imageTransMatrix);
		vtkSmartPointer<vtkTransform> imageTransform = vtkSmartPointer<vtkTransform>::New(); 
		imageTransform->SetMatrix(imageTransMatrix); 
		imageTransform->Update(); 

		// create an importer to read the data back in
		vtkImageImport* importer = vtkImageImport::New();
		importer->SetWholeExtent(0,imageWidthInPixels - 1,0,imageHeightInPixels - 1,0,0);
		importer->SetDataExtentToWholeExtent();
		importer->SetDataScalarTypeToUnsignedChar();
		importer->SetImportVoidPointer(currentFrameImageData);
		importer->SetNumberOfScalarComponents(1); 
		importer->Update();

		vtkImageFlip* imageFlipX = vtkImageFlip::New(); 
		imageFlipX->SetInput( importer->GetOutput() ); 
		imageFlipX->SetFilteredAxis(0); 
		imageFlipX->Update();

		vtkImageFlip* imageFlip = vtkImageFlip::New(); 
		imageFlip->SetInput( imageFlipX->GetOutput() ); 
		imageFlip->SetFilteredAxis(1); 
		imageFlip->Update(); 

		vtkImageData* frame = vtkImageData::New(); 
		frame->DeepCopy(imageFlip->GetOutput()); 

		imageFlip->Delete(); 
		imageFlipX->Delete(); 
		importer->Delete(); 

		frame->SetOrigin(-inputOriginX, -inputOriginY, 0); 
		vtkImageActor* imageActor = vtkImageActor::New();
		imageActor->SetInput(frame); 
		imageActor->SetUserTransform(imageTransform); 
		imageActor->VisibilityOff(); 
		imageTransforms.push_back(imageTransform); 
		imageActors->AddItem(imageActor); 
	}

	PrintProgressBar( 100 ); 
	std::cout << std::endl;

	for (int i = 0; i < imageActors->GetNumberOfItems(); i++) 
	{
		vtkImageActor* imgActor = static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i)); 
		(i == 0 ? imgActor->VisibilityOn() : imgActor->VisibilityOff() ); 
		renderer->AddActor(imgActor);
	}

	if (renderingOff)
	{
		LOG_INFO("No need for rendering..."); 

		if ( renWin != NULL ) 
		{
			renWin->Delete();
		}

		if ( renderer != NULL ) 
		{
			renderer->Delete();
		}

		if ( iren != NULL ) 
		{
			iren->Delete();
		}

		if ( imageActors != NULL ) 
		{
			for ( int i = 0; i < imageActors->GetNumberOfItems(); i++)
			{
				static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i))->GetInput()->Delete();  
				static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i))->Delete(); 
			}
			imageActors->Delete(); 
		}

		if ( textActor != NULL ) 
		{
			textActor->Delete(); 
		}

		LOG_INFO("Exit successfully"); 
		exit(EXIT_SUCCESS); 
	}


	// Create a text actor for image position information
	textActor=vtkTextActor::New();
	vtkSmartPointer<vtkTextProperty> textprop = textActor->GetTextProperty();
	textprop->SetColor(1,0,0);
	textprop->SetFontFamilyToArial();
	textprop->SetFontSize(15);
	textprop->SetJustificationToLeft();
	textprop->SetVerticalJustificationToTop();

	textActor->VisibilityOn(); 
	textActor->SetDisplayPosition(10,50); 

	renderer->AddActor(textActor); 

	renderer->SetBackground(0.1,0.2,0.4);
	renWin->SetSize(800,600);
	renWin->Render();

	//establish timer event and create timer
	vtkMyCallback* call = vtkMyCallback::New();
	iren->AddObserver(vtkCommand::TimerEvent, call);
	iren->AddObserver(vtkCommand::CharEvent, call);
	iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

	//iren must be initialized so that it can handle events
	iren->Initialize();
	iren->Start();

	if ( renWin != NULL ) 
	{
		renWin->Delete();
	}

	if ( renderer != NULL ) 
	{
		renderer->Delete();
	}

	if ( iren != NULL ) 
	{
		iren->Delete();
	}

	if ( call != NULL )
	{
		call->Delete(); 
	}

	if ( imageActors != NULL ) 
	{
		for ( int i = 0; i < imageActors->GetNumberOfItems(); i++)
		{
			static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i))->GetInput()->Delete();  
			static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i))->Delete(); 
		}
		imageActors->Delete(); 
	}

	if ( textActor != NULL ) 
	{
		textActor->Delete(); 
	}


	std::cout << "itkMetaImageSequenceIOTest2 completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}

void PrintProgressBar( int percent )
{
	std::string bar;

	for(int i = 0; i < 50; i++)
	{
		if( i < (percent/2))
		{
			bar.replace(i,1,"=");
		}
		else if( i == (percent/2))
		{
			bar.replace(i,1,">");
		}
		else
		{
			bar.replace(i,1," ");
		}
	}

	std::cout<< "\r" "[" << bar << "] ";
	std::cout.width( 3 );
	std::cout<< percent << "%     " << std::flush;
}