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

#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"

///////////////////////////////////////////////////////////////////

class vtkMyCallback : public vtkCommand
{
public:
	static vtkMyCallback *New()	{return new vtkMyCallback;}

  void Initialize(vtkRenderWindow *renWin, vtkRenderWindowInteractor *iren, 
    vtkTextActor *textActor, vtkCollection *imageActors, 
    std::vector<vtkTransform*> *imageTransforms)
  {
    this->RenWin=renWin;
    this->Iren=iren; 
    this->TextActor=textActor;
    this->ImageActors=imageActors;
    this->ImageTransforms=imageTransforms;
  }

	virtual void Execute(vtkObject *caller, unsigned long callerevent, void*)
	{
		if (callerevent == vtkCommand::CharEvent)
		{
			char keycode=this->Iren->GetKeyCode();
			switch (keycode)
			{ 
			case '+':
				{
					if ( ++this->FrameNum >= this->ImageActors->GetNumberOfItems() )
					{
						this->FrameNum = 0; 
					}

					if  ( this->CurrentActor != NULL ) 
					{
						this->CurrentActor->VisibilityOff(); 
						static_cast<vtkImageActor*>(this->ImageActors->GetItemAsObject(0))->VisibilityOn(); 
					}

					this->CurrentActor = static_cast<vtkImageActor*>(this->ImageActors->GetItemAsObject(this->FrameNum)); 
					this->CurrentActor->VisibilityOn(); 
				}
				break;
			case '-':
				{
					if ( --this->FrameNum < 0 )
					{
						this->FrameNum = this->ImageActors->GetNumberOfItems() - 1; 
					}

					if  ( this->CurrentActor != NULL ) 
					{
						this->CurrentActor->VisibilityOff(); 
						static_cast<vtkImageActor*>(this->ImageActors->GetItemAsObject(0))->VisibilityOn(); 
					}

					this->CurrentActor = static_cast<vtkImageActor*>(this->ImageActors->GetItemAsObject(this->FrameNum)); 
					this->CurrentActor->VisibilityOn(); 
				}
				break;
			}
		}

		double * pos = (*this->ImageTransforms)[this->FrameNum]->GetPosition(); 
		std::ostringstream ss;
		ss.precision( 2 ); 
		ss << "Frame " << this->FrameNum << "\nImage position: " << std::fixed << pos[0] << "  " << pos[1] << "  " << pos[2] << std::ends;
		this->TextActor->SetInput(ss.str().c_str());

		this->RenWin->Render(); 
		//update the timer so it will trigger again
		this->Iren->CreateTimer(VTKI_TIMER_UPDATE);
	}

protected:

  vtkMyCallback::vtkMyCallback()
  {
    this->FrameNum = 0; 
    this->CurrentActor = NULL; 
    this->RenWin = NULL; 
    this->Iren = NULL;
    this->TextActor = NULL; 
    this->ImageActors = NULL; 
    this->ImageTransforms=NULL; 
  }

  virtual vtkMyCallback::~vtkMyCallback()
  {
  }

  int FrameNum; 
  vtkImageActor* CurrentActor; 
  vtkRenderWindow *RenWin; 
  vtkRenderWindowInteractor *Iren;
  vtkTextActor *TextActor; 
  vtkCollection *ImageActors; 
  std::vector<vtkTransform*> *ImageTransforms; 

};

int main(int argc, char **argv)
{

	std::string inputImageSequenceFileName;
	bool renderingOff(false);
	int inputOriginX(0); 
	int inputOriginY(0); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-img-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageSequenceFileName, "Filename of the input image sequence.");
	args.AddArgument("--input-origin-x", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputOriginX, "Image X origin in px (Default: 0)");
	args.AddArgument("--input-origin-y", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputOriginY, "Image Y origin in px (Default: 0)");
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

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

  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New(); 

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New(); 
	renWin->AddRenderer(renderer); 

	// Create imageactor collection
  vtkSmartPointer<vtkCollection> imageActors = vtkSmartPointer<vtkCollection>::New(); 	

	//Create the interactor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New(); 
	iren->SetRenderWindow(renWin);

	LOG_INFO("Reading image sequence...");
  vtkSmartPointer<vtkMetaImageSequenceIO> reader=vtkSmartPointer<vtkMetaImageSequenceIO>::New();
	reader->SetFileName(inputImageSequenceFileName.c_str());

	if (reader->Read()!=PLUS_SUCCESS)
  {		
    LOG_ERROR("Couldn't read sequence metafile: " <<  reader->GetFileName() ); 
  	return EXIT_FAILURE;
	}	

  vtkTrackedFrameList* trackedFrameList=reader->GetTrackedFrameList();
  if (trackedFrameList==NULL)
	{
		LOG_ERROR("Unable to get trackedFrameList!"); 
		return EXIT_FAILURE;
	}

  const unsigned long imageWidthInPixels = trackedFrameList->GetFrameSize()[0]; 
	const unsigned long imageHeightInPixels = trackedFrameList->GetFrameSize()[1]; 
  unsigned long numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();

  unsigned int frameSizeInBytes = imageWidthInPixels * imageHeightInPixels * trackedFrameList->GetNumberOfBitsPerPixel()/8;

	LOG_INFO("Adding frames to actors...");

  std::vector<vtkTransform*> imageTransforms; 
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
	for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
	{
		vtkPlusLogger::PrintProgressbar( (100.0 * imgNumber) / numberOfFrames ); 
    TrackedFrame* trackedFrame=trackedFrameList->GetTrackedFrame(imgNumber);

		vtkSmartPointer<vtkMatrix4x4> imageTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    reader->GetTrackedFrame(imgNumber)->GetCustomFrameTransform(defaultFrameTransformName.c_str(), imageTransMatrix);
		vtkSmartPointer<vtkTransform> imageTransform = vtkSmartPointer<vtkTransform>::New(); 
		imageTransform->SetMatrix(imageTransMatrix); 
		imageTransform->Update(); 

		vtkSmartPointer<vtkImageData> frame = vtkSmartPointer<vtkImageData>::New(); 
    trackedFrame->GetImageData()->CopyToVtkImage(frame);

		frame->SetOrigin(-inputOriginX, -inputOriginY, 0); 
    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
		imageActor->SetInput(frame); 
		imageActor->SetUserTransform(imageTransform); 
		imageActor->VisibilityOff(); 
		imageTransforms.push_back(imageTransform); 
		imageActors->AddItem(imageActor); 
	}

	vtkPlusLogger::PrintProgressbar( 100 ); 
	std::cout << std::endl;

	for (int i = 0; i < imageActors->GetNumberOfItems(); i++) 
	{
		vtkImageActor* imgActor = static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i)); 
		imgActor->SetVisibility(i==0); // hide all frames but the first one
		renderer->AddActor(imgActor);
	}

	if (renderingOff)
	{
		LOG_INFO("No need for rendering..."); 
		LOG_INFO("Exit successfully"); 
		exit(EXIT_SUCCESS); 
	}

	// Create a text actor for image position information
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
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
	vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
  call->Initialize(renWin, iren, textActor, imageActors, &imageTransforms);
	iren->AddObserver(vtkCommand::TimerEvent, call);
	iren->AddObserver(vtkCommand::CharEvent, call);
	iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

	//iren must be initialized so that it can handle events
	iren->Initialize();
	iren->Start();

	std::cout << "vtkMetaImageSequenceIOTest2 completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}