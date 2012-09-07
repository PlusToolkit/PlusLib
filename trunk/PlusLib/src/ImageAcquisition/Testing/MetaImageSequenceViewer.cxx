/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkXMLUtilities.h"
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
#include "vtkActorCollection.h"
#include "vtkCollectionIterator.h"
#include "vtkImageActor.h"
#include "vtkImageImport.h"
#include "vtkProp.h"

#include "vtkMetaImageSequenceIO.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransformRepository.h"
#include "TrackedFrame.h"

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

  vtkMyCallback()
  {
    this->FrameNum = 0; 
    this->CurrentActor = NULL; 
    this->RenWin = NULL; 
    this->Iren = NULL;
    this->TextActor = NULL; 
    this->ImageActors = NULL; 
    this->ImageTransforms=NULL; 
  }

  virtual ~vtkMyCallback()
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
  bool printHelp(false);
  std::string inputMetaFilename;
  std::string inputConfigFileName; 
  std::string outputModelFilename; 
  std::string imageToReferenceTransformNameStr;
  bool renderingOff(false);

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--image-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imageToReferenceTransformNameStr, "Transform name used for displaying the slices");  
  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMetaFilename, "Tracked ultrasound recorded by Plus (e.g., by the TrackedUltrasoundCapturing application) in a sequence metafile (.mha)");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing coordinate system definitions");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run in test mode, without rendering.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	

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
    exit(EXIT_SUCCESS); 
  }

  if (inputMetaFilename.empty())
  {
    std::cerr << "--input-metafile is required" << std::endl;
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

  // Read input tracked ultrasound data.
  LOG_DEBUG("Reading input... ");
  vtkSmartPointer< vtkTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkTrackedFrameList >::New(); 
  // Orientation is XX so that the orientation of the trackedFrameList will match the orientation defined in the file
  trackedFrameList->ReadFromSequenceMetafile( inputMetaFilename.c_str() );
  LOG_DEBUG("Reading input done.");
  LOG_DEBUG("Number of frames: " << trackedFrameList->GetNumberOfTrackedFrames());

  // Read calibration matrices from the config file
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( !inputConfigFileName.empty() )
  {
    LOG_DEBUG("Reading config file...");
    vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
    LOG_DEBUG("Reading config file done.");
    if ( transformRepository->ReadConfiguration(configRead) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read transforms for transform repository!"); 
      return EXIT_FAILURE; 
    }
  }
  else
  {
    LOG_INFO("Configuration file is not specified. Only those transforms are available that are defined in the sequence metafile");    
  }

  LOG_INFO("Adding frames to actors...");

  std::vector<vtkTransform*> imageTransforms; 

  PlusTransformName imageToReferenceTransformName; 
  if (!imageToReferenceTransformNameStr.empty())
  {
    // transform name is defined, we'll show each image in its correct 3D position
    if ( imageToReferenceTransformName.SetTransformName(imageToReferenceTransformNameStr.c_str())!= PLUS_SUCCESS )
    {    
      LOG_ERROR("Invalid image to reference transform name: " << imageToReferenceTransformNameStr ); 
      return EXIT_FAILURE; 
    }
  }

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  for ( int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++ )
  {
    vtkPlusLogger::PrintProgressbar( (100.0 * frameIndex) / numberOfFrames ); 
    TrackedFrame* frame = trackedFrameList->GetTrackedFrame( frameIndex );

    // Update transform repository 
    if ( transformRepository->SetTransforms(*frame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set repository transforms from tracked frame!"); 
      continue; 
    }		

    vtkSmartPointer<vtkImageData> frameImageData = vtkSmartPointer<vtkImageData>::New(); 
    frameImageData->DeepCopy(frame->GetImageData()->GetVtkImage()); 

    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInput(frameImageData); 

    vtkSmartPointer< vtkTransform > imageToReferenceTransform = vtkSmartPointer< vtkTransform >::New();
    if (imageToReferenceTransformName.IsValid())
    {
      vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if ( transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceTransformMatrix) != PLUS_SUCCESS )
      {
        std::string strTransformName; 
        imageToReferenceTransformName.GetTransformName(strTransformName); 
        LOG_ERROR("Failed to get transform from repository: " << strTransformName ); 
        continue; 
      }      
      imageToReferenceTransform->SetMatrix( imageToReferenceTransformMatrix );
    }

    imageActor->SetUserTransform(imageToReferenceTransform); 
    imageActor->VisibilityOff(); 
    imageTransforms.push_back(imageToReferenceTransform); 
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
  }
  else
  {
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
  }

  std::cout << "MetaImageSequenceViewer completed successfully!" << std::endl;
  return EXIT_SUCCESS; 
}
