#include "PlusConfigure.h"

#include "vtkCalibratorVisualizationComponent.h" 
#include "vtkProbeCalibrationController.h"
#include "vtkImageImport.h"


// VTK includes
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkDataObject.h"
#include "vtkXYPlotActor.h"
#include "vtkLookupTable.h"
#include "vtkAxisActor2D.h"
#include "vtkProperty2D.h"
#include "vtkWindowToImageFilter.h"
#include "vtkTIFFWriter.h"
#include "vtkImageFlip.h"
#include "vtkScalarBarActor.h"
#include "vtkImageViewer.h"
#include "vtkLookupTable.h"
#include "vtkTextProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkPolyDataMapper.h"
#include "vtkSphereSource.h"
#include "vtkDiskSource.h"
#include "vtkTextActor3D.h"
#include "vtkTextSource.h" 
#include "vtkAppendPolyData.h"
#include "vtkVertex.h"
#include "vtkDataObject.h"
#include "vtkPolyData.h"
#include "vtkTransformPolyDataFilter.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkCalibratorVisualizationComponent, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCalibratorVisualizationComponent); 


//----------------------------------------------------------------------------
vtkCalibratorVisualizationComponent::vtkCalibratorVisualizationComponent()
{
	this->InitializedOff(); 
	this->OutputPath = NULL; 

  this->CalibrationController = NULL; 
	this->RealtimeRenderer = NULL; 
	this->RealtimeImageActor = NULL; 
	this->CenterOfRotationActor = NULL; 
	this->PhantomWiresActor = NULL;
	this->TransformVtkImageToTemplate = NULL; 
	this->ImageCamera = NULL; 
	this->TemplateCamera = NULL; 
  this->TemplateLetterActors = NULL; 
  this->TemplateHolesActor = NULL; 
}


//----------------------------------------------------------------------------
vtkCalibratorVisualizationComponent::~vtkCalibratorVisualizationComponent()
{

  if ( this->RealtimeRenderer != NULL )
  {
    // Remove image actor to the realtime renderer 
    if ( this->GetRealtimeImageActor() != NULL )
    {
      this->RealtimeRenderer->RemoveActor(this->GetRealtimeImageActor()); 
    }

    if ( this->GetCenterOfRotationActor() != NULL )
    {
      this->RealtimeRenderer->RemoveActor(this->GetCenterOfRotationActor()); 
    }

    if ( this->GetPhantomWiresActor() )
    {
      this->RealtimeRenderer->RemoveActor(this->GetPhantomWiresActor()); 
    }
  }

	this->SetRealtimeRenderer(NULL); 
	this->SetRealtimeImageActor(NULL); 
	this->SetCenterOfRotationActor(NULL); 
	this->SetPhantomWiresActor(NULL);
	this->SetTransformVtkImageToTemplate(NULL); 
	this->SetTemplateCamera(NULL); 
	this->SetImageCamera(NULL); 

  if ( this->TemplateLetterActors != NULL )
  {
    this->TemplateLetterActors->Delete(); 
    this->TemplateLetterActors = NULL; 
  } 

  if ( this->TemplateHolesActor != NULL )
  {
    this->TemplateHolesActor->Delete(); 
    this->TemplateHolesActor = NULL; 
  }
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::Initialize()
{
	if ( this->Initialized )
	{
		return; 
	}

  if ( this->GetCalibrationController() == NULL )
  {
    LOG_ERROR("Failed to initialize vtkCalibratorVisualizationComponent without calibration controller!"); 
    return; 
  }

  // Template == World coordinate system
  // Image == itkImage coordinate system
  // transformUser =
  //   = transformVtkImageToWorld 
  //   = transformVtkImageToTemplate
  //   = transformImageToTemplate * transformVtkImageToImage

  int * frameSize = this->GetCalibrationController()->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize(); 

  // there is an in-place vertical flip between the coordinate systems of vtk and itk images
  vtkSmartPointer<vtkMatrix4x4> transformVtkImageToImage = vtkSmartPointer<vtkMatrix4x4>::New(); 
  transformVtkImageToImage->Identity();
  transformVtkImageToImage->SetElement(1,1, -1);
  transformVtkImageToImage->SetElement(1,3, frameSize[1]);

	// Set upper left corner as image origin
  vtkSmartPointer<vtkTransform> transformUser = vtkSmartPointer<vtkTransform>::New(); 
  transformUser->Concatenate(this->CalibrationController->GetTransformImageToTemplate()); 
  transformUser->Concatenate(transformVtkImageToImage); 
	transformUser->Update(); 
  this->SetTransformVtkImageToTemplate(transformUser); 

  vtkSmartPointer<vtkImageActor> realtimeImageActor = vtkSmartPointer<vtkImageActor>::New();
	realtimeImageActor->VisibilityOn(); 
	this->SetRealtimeImageActor(realtimeImageActor); 

	vtkSmartPointer<vtkActor> centerOfRotationActor = vtkSmartPointer<vtkActor>::New();
	centerOfRotationActor->VisibilityOff(); 
	this->SetCenterOfRotationActor(centerOfRotationActor); 

	vtkSmartPointer<vtkActor> phantomWiresActor = vtkSmartPointer<vtkActor>::New();
	phantomWiresActor->VisibilityOff(); 
	this->SetPhantomWiresActor(phantomWiresActor); 
	
	vtkSmartPointer<vtkCamera> templateCamera = vtkSmartPointer<vtkCamera>::New(); 
	templateCamera->SetPosition(30,30,-150); 
	templateCamera->SetFocalPoint(30,30,0); 
	templateCamera->SetViewUp(0, -1, 0);
	templateCamera->SetClippingRange(0.1, 1000);
	templateCamera->ParallelProjectionOn(); 
	templateCamera->SetParallelScale(40);	
	this->SetTemplateCamera(templateCamera); 

	this->SetupTemplateOverlay(); 
	
	this->InitializedOn(); 

}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetRealtimeRenderer( vtkRenderer* renderer )
{
	// Set up the realtime renderer
  this->RealtimeRenderer = renderer; 

  if ( this->RealtimeRenderer != NULL )
  {
    // Add image actor to the realtime renderer 
    this->RealtimeRenderer->AddActor(this->GetRealtimeImageActor()); 
    this->RealtimeRenderer->AddActor(this->GetCenterOfRotationActor()); 
    this->RealtimeRenderer->AddActor(this->GetPhantomWiresActor()); 

    // Set image camera
    vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
    double imageCenterX = this->GetCalibrationController()->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize()[0]/2.0; 
    double imageCenterY = this->GetCalibrationController()->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize()[1]/2.0; 
    imageCamera->SetPosition(imageCenterX, imageCenterY, -150); 
    imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0); 
    imageCamera->SetViewUp(0, -1, 0);
    imageCamera->SetClippingRange(0.1, 1000);
    imageCamera->ParallelProjectionOn(); 
    imageCamera->SetParallelScale(imageCenterY);	
    this->SetImageCamera(imageCamera); 

    this->RealtimeRenderer->GetActiveCamera()->DeepCopy(this->GetImageCamera());

    this->CreateTemplateGridActors(); 

  }
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupTemplateOverlay()
{
  if ( this->TemplateLetterActors == NULL )
  {
    this->TemplateLetterActors = vtkCollection::New(); 
  }

  if ( this->TemplateHolesActor == NULL )
  {
    this->TemplateHolesActor = vtkActor::New(); 
  }
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::OverlayCenterOfRotation()
{
	if ( !this->Initialized )
	{
		return; 
	}

	vtkSmartPointer<vtkPolyDataMapper> polyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(5); 
	polyMapper->SetInput(sphere->GetOutput());

	this->GetCenterOfRotationActor()->SetMapper(polyMapper);
	this->GetCenterOfRotationActor()->GetProperty()->SetColor(1,0,0);

	int originX = this->GetCalibrationController()->GetUSImageFrameOriginXInPixels(); 
	int originY = this->GetCalibrationController()->GetPatternRecognition()->GetFidSegmentation()->GetFrameSize()[1] - this->GetCalibrationController()->GetUSImageFrameOriginYInPixels(); 
	this->GetCenterOfRotationActor()->SetPosition(originX, originY, 0); 
	this->GetCenterOfRotationActor()->Modified(); 
	this->ShowCenterOfRotation(); 
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::OverlayPhantomWires()
{
	if ( !this->Initialized )
	{
		return; 
	}

	vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkPolyDataMapper> polyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	polyMapper->SetInputConnection(appendPolyData->GetOutputPort());

	this->GetPhantomWiresActor()->SetMapper(polyMapper);
	this->GetPhantomWiresActor()->GetProperty()->SetColor(1,0,0);

	vtkSmartPointer<vtkTransform> transformTemplateToUsImage = vtkSmartPointer<vtkTransform>::New(); 
	transformTemplateToUsImage->DeepCopy(this->CalibrationController->GetTransformImageToTemplate()); 
	transformTemplateToUsImage->Inverse(); 

	for ( int wireNum = 1; wireNum < 7; wireNum++ )
	{
		double wireposmm[3] = {0,0,0}; 
		if ( ! this->GetCalibrationController()->GetWirePosInTemplateCoordinate(wireNum, wireposmm) )
		{
			// plane and line are parallel
			this->GetPhantomWiresActor()->SetMapper(NULL); 
			return; 
		}

		vtkSmartPointer<vtkTransform> transformPoint = vtkSmartPointer<vtkTransform>::New(); 
		transformPoint->PreMultiply();
		transformPoint->Concatenate(transformTemplateToUsImage); 
		transformPoint->Translate(wireposmm[0], wireposmm[1], wireposmm[2]); 
		transformPoint->Update(); 

		double* wirepospx = transformPoint->GetPosition(); 
		vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
		sphere->SetRadius(3); 
		sphere->SetCenter(wirepospx[0], wirepospx[1], 0);
		appendPolyData->AddInput( sphere->GetOutput() );
	}

	///************************ Apply user transform to wires actor *********************************/
	this->GetPhantomWiresActor()->SetUserTransform( this->CalibrationController->GetTransformImageToTemplate() );
	this->GetPhantomWiresActor()->Modified();
}


//----------------------------------------------------------------------------
PlusStatus vtkCalibratorVisualizationComponent::AddFrameToRealtimeRenderer(vtkImageData* frame)
{
	if ( !this->Initialized )
	{
      LOG_WARNING("Failed to add frame to the realtime renderer - vtkCalibratorVisualizationComponent not initialized!"); 
		  return PLUS_FAIL; 
	}

    if ( this->GetRealtimeImageActor() == NULL )
    {
      LOG_WARNING("Failed to add frame to the realtime renderer - realtime image actor is NULL!"); 
		  return PLUS_FAIL;
    }

	this->GetRealtimeImageActor()->SetInput(frame); 
	this->GetRealtimeImageActor()->Modified(); 

	if ( this->GetRealtimeRenderer() == NULL || this->GetRealtimeRenderer()->GetRenderWindow() == NULL ) 
	{
        LOG_WARNING("Failed to render realtime image - render window is NULL!"); 
		return PLUS_FAIL; 
	}

    this->GetRealtimeRenderer()->GetRenderWindow()->Render(); 

    return PLUS_SUCCESS; 
}


// Create Template Grid overlaid on the US video
//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::CreateTemplateGridActors()
{
	//if ( !this->Initialized )
	//{
	//	return; 
	//}

	//const double XLONGOFFSET_HOLE = 2.5;

	//vtkSmartPointer<vtkAppendPolyData> templateGridPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
 // vtkSmartPointer<vtkPolyDataMapper> polyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
 // polyMapper->SetInputConnection(templateGridPolyData->GetOutputPort());
 // this->TemplateHolesActor->SetMapper(polyMapper); 
 // this->TemplateHolesActor->GetProperty()->SetColor(0,1,0); 
 // this->GetRealtimeRenderer()->AddActor(this->TemplateHolesActor); 

	//for (unsigned int i = 0; i < TemplateModelHoles.size(); i++)
	//{
	//		vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
	//		sphere->SetRadius(TemplateModelHoles[i].Radius); 
 //     //holeTransMapper->SetInput(sphere->GetOutput());
 //     sphere->SetCenter(TemplateModelHoles[i].PositionX, TemplateModelHoles[i].PositionY , TemplateModelHoles[i].PositionZ); 
 //     templateGridPolyData->AddInput( sphere->GetOutput() );
	//	}


	//// --------------------  template letters ------------------------ //

	//const double XTRANSOFFSET_LETTER = 0.9; 
	//const double YTRANSOFFSET_LETTER = 1.3; 
	//const double XLONGOFFSET_LETTER_CHAR = 0.9; 
	//const double YLONGOFFSET_LETTER_CHAR = 71.3; 
	//const double XLONGOFFSET_LETTER_NUM = 0.9; 
	//const double YLONGOFFSET_LETTER_NUM = 1.3; 

 // // Clean actors first
 // this->TemplateLetterActors->RemoveAllItems(); 

	//for (unsigned int i = 0; i < TemplateModelLetters.size(); i++)
	//{

	//	vtkSmartPointer<vtkTextActor3D> gridTransLettersActor = vtkSmartPointer<vtkTextActor3D>::New();
	//	gridTransLettersActor->GetTextProperty()->SetColor(0,1,0);
	//	gridTransLettersActor->GetTextProperty()->SetFontFamilyToArial();
 //   gridTransLettersActor->GetTextProperty()->SetFontSize(32);
	//	gridTransLettersActor->GetTextProperty()->SetJustificationToLeft();
	//	gridTransLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
	//	gridTransLettersActor->GetTextProperty()->BoldOn(); 

	//	double * imageScaleTrans = this->GetCalibrationController()->GetTransformImageToTemplate()->GetScale(); 
 //   //gridTransLettersActor->SetScale(imageScaleTrans[0], imageScaleTrans[1], imageScaleTrans[2]);
	//	gridTransLettersActor->RotateWXYZ(180, 1, 0, 0); 
	//	//gridTransLettersActor->SetScale(0.2,0.2,0.2); 
 //   gridTransLettersActor->SetScale(0.1,0.1,0.1); 
	//	gridTransLettersActor->SetInput(TemplateModelLetters[i].ID.c_str());
	//	gridTransLettersActor->SetPosition(TemplateModelLetters[i].PositionX - XTRANSOFFSET_LETTER, TemplateModelLetters[i].PositionY + YTRANSOFFSET_LETTER, TemplateModelLetters[i].PositionZ); 
	//	gridTransLettersActor->Modified(); 

 //   this->TemplateLetterActors->AddItem(gridTransLettersActor); 

 //   this->GetRealtimeRenderer()->AddActor(gridTransLettersActor); 

 //   //gridTransLettersActor->SetUserTransform(this->GetCalibrationController()->GetTransformTemplateHomeToTemplate()); 
	//	}

 // this->SetTemplateVisibility(false); 
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetTemplateVisibility( bool visibility )
{
for (int i = 0; i < this->TemplateLetterActors->GetNumberOfItems(); i++) 
	{
    vtkTextActor3D* actor = dynamic_cast<vtkTextActor3D*>(this->TemplateLetterActors->GetItemAsObject(i)); 
    actor->SetVisibility( visibility ); 
	}

  this->TemplateHolesActor->SetVisibility( visibility ); 
}


//----------------------------------------------------------------------------
PlusStatus vtkCalibratorVisualizationComponent::ReadTemplateModelConfiguration(vtkXMLDataElement *configTemplateModel)
{
	//if ( configTemplateModel == NULL ) 
	//{
	//	LOG_ERROR("Unable to read template model file (xml data element is not found)"); 
 //   return PLUS_FAIL;
	//}	

 // double templateHolderToTemplateTransformVector[16]={0}; 
 // if (configTemplateModel->GetVectorAttribute("TemplateHolderToTemplateTransform", 16, templateHolderToTemplateTransformVector)) 
 // {
 //   this->GetCalibrationController()->GetTransformTemplateHolderHomeToTemplateHome()->SetMatrix( templateHolderToTemplateTransformVector ); 
 // }
 // else
	//{
	//	LOG_ERROR("Unable to read template origin from template holder from template model file!"); 
 //   return PLUS_FAIL; 
	//}

	//// ************************* Template model letters *************************
	//vtkSmartPointer<vtkXMLDataElement> letterPositions = configTemplateModel->FindNestedElementWithName("LetterPositions");
	//if (letterPositions!=NULL)
	//{ 

	//	for (int i = 0; i < letterPositions->GetNumberOfNestedElements(); i++)
	//	{ 
	//		TemplateModel letterModel; 
	//		vtkXMLDataElement *elem=letterPositions->GetNestedElement(i);
	//		if (STRCASECMP("Letter", elem->GetName())!=0)
	//		{
	//			// not a Letter element
	//			continue;
	//		} 

	//		const char* id=elem->GetAttribute("ID");
	//		if (id!=0)
	//		{
	//			letterModel.ID = id; 
	//		} 

	//		double positionXYZ[3]; 
	//		if (elem->GetVectorAttribute("PositionXYZ",3,positionXYZ))
	//		{
	//			letterModel.PositionX = positionXYZ[0]; 
	//			letterModel.PositionY = positionXYZ[1]; 
	//			letterModel.PositionZ = positionXYZ[2];
	//		} 

	//		const char* type = elem->GetAttribute("Type"); 
	//		if (type != NULL && STRCASECMP("Char", type )==0)
	//		{	
	//			letterModel.LetterType = LETTER_CHAR; 
	//		}
	//		else 
	//		{
	//			letterModel.LetterType = LETTER_NUM; 
	//		}


	//		TemplateModelLetters.push_back(letterModel); 
	//	}
	//}
 // else
 // {
 //   LOG_ERROR("Unable to read LetterPositions xml data element from configuration file!"); 
 //   return PLUS_FAIL; 
 // }

	////************************* Template model holes *************************
	//
	//TEMPLATE_HOLE_REPRESENTATION holeRepresentation = HOLE_SPHERE; 
	//TEMPLATE_LETTER_TYPE letterType = LETTER_NUM; 
	//double holeRadius=0.2; 

	//vtkSmartPointer<vtkXMLDataElement> templateHole = configTemplateModel->FindNestedElementWithName("TemplateHole");
	//if (templateHole != NULL) 
	//{
	//	const char* representation = templateHole->GetAttribute("Representation"); 
	//	if (representation != NULL && STRCASECMP("Disk", representation)==0)
	//	{	
	//		holeRepresentation = HOLE_DISK; 
	//	}
	//	else 
	//	{
	//		holeRepresentation = HOLE_SPHERE; 
	//	}

	//	templateHole->GetScalarAttribute("Radius", holeRadius); 

	//}
 // else
 // {
 //   LOG_ERROR("Unable to read TemplateHole xml data element from configuration file!"); 
 //   return PLUS_FAIL; 
 // }

	//vtkSmartPointer<vtkXMLDataElement> holePositions = configTemplateModel->FindNestedElementWithName("HolePositions");
	//if (holePositions!=NULL)
	//{ 
	//	for (int i = 0; i < holePositions->GetNumberOfNestedElements(); i++)
	//	{ 
	//		TemplateModel holeModel; 
	//		vtkXMLDataElement *elem=holePositions->GetNestedElement(i);
	//		if (STRCASECMP("Hole", elem->GetName())!=0)
	//		{
	//			// not a Hole element
	//			continue;
	//		} 

	//		const char* id=elem->GetAttribute("ID");
	//		if (id!=0)
	//		{
	//			holeModel.ID = id; 
	//		} 

	//		double positionXYZ[3]; 
	//		if (elem->GetVectorAttribute("PositionXYZ",3,positionXYZ))
	//		{
	//			holeModel.PositionX = positionXYZ[0]; 
	//			holeModel.PositionY = positionXYZ[1]; 
	//			holeModel.PositionZ = positionXYZ[2];
	//		} 

	//		holeModel.Representation = holeRepresentation; 
	//		holeModel.Radius = holeRadius; 
	//		holeModel.LetterType = letterType; 
	//		TemplateModelHoles.push_back(holeModel); 
	//	}
	//}
 // else
 // {
 //   LOG_ERROR("Unable to read HolePositions xml data element from configuration file!"); 
 //   return PLUS_FAIL; 
 // }

  return PLUS_SUCCESS; 
}

