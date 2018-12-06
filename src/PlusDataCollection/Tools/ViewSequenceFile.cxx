/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igsioTrackedFrame.h"
#include "vtkActorCollection.h"
#include "vtkCallbackCommand.h"
#include "vtkCollectionIterator.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkImageViewer2.h"
#include "vtkMatrix4x4.h"
#include "vtkProp.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"
#include "vtkIGSIOSequenceIO.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkTransform.h"
#include "vtkIGSIOTransformRepository.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

///////////////////////////////////////////////////////////////////

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()  {return new vtkMyCallback;}

  void Initialize(vtkRenderWindow* renderWindow,
                  vtkRenderWindowInteractor* renderWindowInteractor,
                  vtkTextActor* textActor,
                  vtkCollection* imageActors,
                  std::vector<vtkTransform*>* imageTransforms)
  {
    this->RenderWindow = renderWindow;
    this->RenderWindowInteractor = renderWindowInteractor;
    this->TextActor = textActor;
    this->ImageActors = imageActors;
    this->ImageTransforms = imageTransforms;
  }

  virtual void Execute(vtkObject* caller, unsigned long callerEvent, void*)
  {
    if (callerEvent == vtkCommand::CharEvent)
    {
      char keycode = this->RenderWindowInteractor->GetKeyCode();
      switch (keycode)
      {
        case '+':
        {
          if (++this->FrameNum >= this->ImageActors->GetNumberOfItems())
          {
            this->FrameNum = 0;
          }

          if (this->CurrentActor != NULL)
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
          if (--this->FrameNum < 0)
          {
            this->FrameNum = this->ImageActors->GetNumberOfItems() - 1;
          }

          if (this->CurrentActor != NULL)
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

    double* position = (*this->ImageTransforms)[this->FrameNum]->GetPosition();
    std::ostringstream ss;
    ss.precision(2);
    ss << "Frame " << this->FrameNum << "\nImage position: " << std::fixed << position[0] << "  " << position[1] << "  " << position[2] << std::ends;
    this->TextActor->SetInput(ss.str().c_str());

    this->RenderWindow->Render();
    // Update the timer so it will trigger again
    this->RenderWindowInteractor->CreateTimer(VTKI_TIMER_UPDATE);
  }

protected:
  vtkMyCallback()
  {
    this->FrameNum = 0;
    this->CurrentActor = NULL;
    this->RenderWindow = NULL;
    this->RenderWindowInteractor = NULL;
    this->TextActor = NULL;
    this->ImageActors = NULL;
    this->ImageTransforms = NULL;
  }

  virtual ~vtkMyCallback()
  {
  }

  int FrameNum;
  vtkImageActor* CurrentActor;
  vtkRenderWindow* RenderWindow;
  vtkRenderWindowInteractor* RenderWindowInteractor;
  vtkTextActor* TextActor;
  vtkCollection* ImageActors;
  std::vector<vtkTransform*>* ImageTransforms;
};

int main(int argc, char** argv)
{
  bool printHelp(false);
  std::string inputSequenceFilename;
  std::string inputConfigFileName;
  std::string outputModelFilename;
  std::string imageToReferenceTransformNameStr;
  bool renderingOff(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--image-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &imageToReferenceTransformNameStr, "Transform name used for displaying the slices");
  args.AddArgument("--source-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceFilename, "Tracked ultrasound recorded by Plus (e.g., by the TrackedUltrasoundCapturing application) in a sequence file (.mha/.nrrd)");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing coordinate system definitions");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run in test mode, without rendering.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (printHelp)
  {
    std::cout << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputSequenceFilename.empty())
  {
    std::cerr << "--source-seq-file is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renWin->AddRenderer(renderer);

  // Create image actor collection
  vtkSmartPointer<vtkCollection> imageActors = vtkSmartPointer<vtkCollection>::New();

  //Create the inter actor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renWin);

  // Read input tracked ultrasound data.
  LOG_DEBUG("Reading input... ");
  vtkSmartPointer< vtkIGSIOTrackedFrameList > trackedFrameList = vtkSmartPointer< vtkIGSIOTrackedFrameList >::New();
  // Orientation is XX so that the orientation of the trackedFrameList will match the orientation defined in the file
  if (vtkIGSIOSequenceIO::Read(inputSequenceFilename, trackedFrameList) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to load input sequences file.");
    return EXIT_FAILURE;
  }
  LOG_DEBUG("Reading input done.");
  LOG_DEBUG("Number of frames: " << trackedFrameList->GetNumberOfTrackedFrames());

  // Read calibration matrices from the config file
  vtkSmartPointer<vtkIGSIOTransformRepository> transformRepository = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
  if (!inputConfigFileName.empty())
  {
    LOG_DEBUG("Reading config file...");
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
    if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
      return EXIT_FAILURE;
    }
    if (transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
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

  igsioTransformName imageToReferenceTransformName;
  if (!imageToReferenceTransformNameStr.empty())
  {
    // transform name is defined, we'll show each image in its correct 3D position
    if (imageToReferenceTransformName.SetTransformName(imageToReferenceTransformNameStr.c_str()) != PLUS_SUCCESS)
    {
      LOG_ERROR("Invalid image to reference transform name: " << imageToReferenceTransformNameStr);
      return EXIT_FAILURE;
    }
  }

  int numberOfFrames = trackedFrameList->GetNumberOfTrackedFrames();
  for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
  {
    vtkPlusLogger::PrintProgressbar((100.0 * frameIndex) / numberOfFrames);
    igsioTrackedFrame* frame = trackedFrameList->GetTrackedFrame(frameIndex);

    // Update transform repository
    if (transformRepository->SetTransforms(*frame) != PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set repository transforms from tracked frame!");
      continue;
    }

    vtkSmartPointer<vtkImageData> frameImageData = vtkSmartPointer<vtkImageData>::New();
    frameImageData->DeepCopy(frame->GetImageData()->GetImage());

    vtkSmartPointer<vtkImageActor> imageActor = vtkSmartPointer<vtkImageActor>::New();
    imageActor->SetInputData(frameImageData);

    vtkSmartPointer< vtkTransform > imageToReferenceTransform = vtkSmartPointer< vtkTransform >::New();
    if (imageToReferenceTransformName.IsValid())
    {
      vtkSmartPointer<vtkMatrix4x4> imageToReferenceTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
      if (transformRepository->GetTransform(imageToReferenceTransformName, imageToReferenceTransformMatrix) != PLUS_SUCCESS)
      {
        std::string strTransformName;
        imageToReferenceTransformName.GetTransformName(strTransformName);
        LOG_ERROR("Failed to get transform from repository: " << strTransformName);
        continue;
      }
      imageToReferenceTransform->SetMatrix(imageToReferenceTransformMatrix);
    }

    imageActor->SetUserTransform(imageToReferenceTransform);
    imageActor->VisibilityOff();
    imageTransforms.push_back(imageToReferenceTransform);
    imageActors->AddItem(imageActor);
  }

  vtkPlusLogger::PrintProgressbar(100);
  std::cout << std::endl;

  for (int i = 0; i < imageActors->GetNumberOfItems(); i++)
  {
    vtkImageActor* imgActor = static_cast<vtkImageActor*>(imageActors->GetItemAsObject(i));
    imgActor->SetVisibility(i == 0); // hide all frames but the first one
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
    textprop->SetColor(1, 0, 0);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(15);
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();

    textActor->VisibilityOn();
    textActor->SetDisplayPosition(10, 50);

    renderer->AddActor(textActor);

    renderer->SetBackground(0.1, 0.2, 0.4);
    renWin->SetSize(800, 600);
    renWin->Render();

    //establish timer event and create timer
    vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
    call->Initialize(renWin, renderWindowInteractor, textActor, imageActors, &imageTransforms);
    renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, call);
    renderWindowInteractor->AddObserver(vtkCommand::CharEvent, call);
    renderWindowInteractor->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

    //iren must be initialized so that it can handle events
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
  }

  std::cout << "MetaImageSequenceViewer completed successfully!" << std::endl;
  return EXIT_SUCCESS;
}
