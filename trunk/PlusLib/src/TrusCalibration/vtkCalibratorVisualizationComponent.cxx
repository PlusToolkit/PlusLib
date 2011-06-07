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
#include "vtkAppendPolyData.h"
#include "vtkVertex.h"
#include "vtkDataObject.h"
#include "vtkPolyData.h"

const int vtkCalibratorVisualizationComponent::mColorGreen[] = {0, 255, 0};
const int vtkCalibratorVisualizationComponent::mColorYellow[] = {255, 255, 0};
const int vtkCalibratorVisualizationComponent::mColorBlue[] = {0, 0, 255};
const int vtkCalibratorVisualizationComponent::mColorCyan[] ={0, 255, 255};
const int vtkCalibratorVisualizationComponent::mColorRed[] = {255, 0, 0};


//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkCalibratorVisualizationComponent, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCalibratorVisualizationComponent); 


//----------------------------------------------------------------------------
vtkCalibratorVisualizationComponent::vtkCalibratorVisualizationComponent()
{
	this->InitializedOff(); 
	this->OutputPath = NULL; 
	this->TemplateModelConfigFileName = NULL; 

	this->PlotRenderer = NULL; 
	this->RealtimeRenderer = NULL; 
	this->RealtimeImageActor = NULL; 
	this->CenterOfRotationActor = NULL; 
	this->PhantomWiresActor = NULL;
	this->TransformUSImageToWorld = NULL; 
	this->ImageCamera = NULL; 
	this->TemplateCamera = NULL; 
}


//----------------------------------------------------------------------------
vtkCalibratorVisualizationComponent::~vtkCalibratorVisualizationComponent()
{

	this->SetPlotRenderer(NULL); 
	this->SetRealtimeRenderer(NULL); 
	this->SetRealtimeImageActor(NULL); 
	this->SetCenterOfRotationActor(NULL); 
	this->SetPhantomWiresActor(NULL);
	this->SetTransformUSImageToWorld(NULL); 
	this->SetTemplateCamera(NULL); 
	this->SetImageCamera(NULL); 
	
	if( mptrNumberOfData != NULL )
	{
		mptrNumberOfData->Delete();
		mptrNumberOfData = NULL;
	}

	if( mptrPRE3DinXAxis != NULL )
	{
		mptrPRE3DinXAxis->Delete();
		mptrPRE3DinXAxis = NULL;
	}

	if( mptrPRE3DinYAxis != NULL )
	{
		mptrPRE3DinYAxis->Delete();
		mptrPRE3DinYAxis = NULL;
	}

	if( mptrPRE3DinZAxis != NULL )
	{
		mptrPRE3DinZAxis->Delete();
		mptrPRE3DinZAxis = NULL;
	}

	if( mptrFieldDataPRE3DinXAxis != NULL )
	{
		mptrFieldDataPRE3DinXAxis->Delete();
		mptrFieldDataPRE3DinXAxis = NULL;
	}

	if( mptrFieldDataPRE3DinYAxis != NULL )
	{
		mptrFieldDataPRE3DinYAxis->Delete();
		mptrFieldDataPRE3DinYAxis = NULL;
	}

	if( mptrFieldDataPRE3DinZAxis != NULL )
	{
		mptrFieldDataPRE3DinZAxis->Delete();
		mptrFieldDataPRE3DinZAxis = NULL;
	}

	if( mptrDataObjectPRE3DinXAxis != NULL )
	{
		mptrDataObjectPRE3DinXAxis->Delete();
		mptrDataObjectPRE3DinXAxis = NULL;
	}

	if( mptrDataObjectPRE3DinYAxis != NULL )
	{
		mptrDataObjectPRE3DinYAxis->Delete();
		mptrDataObjectPRE3DinYAxis = NULL;
	}

	if( mptrDataObjectPRE3DinZAxis != NULL )
	{
		mptrDataObjectPRE3DinZAxis->Delete();
		mptrDataObjectPRE3DinZAxis = NULL;
	}

	if( mptrPlotLookupTable != NULL )
	{
		mptrPlotLookupTable->Delete();
		mptrPlotLookupTable = NULL;
	}

	if( mptrPlotActor != NULL )
	{
		mptrPlotActor->Delete();
		mptrPlotActor = NULL;
	}

	if( mptrPRE3DColorLookupTable != NULL )
	{
		mptrPRE3DColorLookupTable->Delete();
		mptrPRE3DColorLookupTable = NULL;
	}

	if( mptrPRE3DScalarBarActorX != NULL )
	{
		mptrPRE3DScalarBarActorX->Delete();
		mptrPRE3DScalarBarActorX = NULL;
	}

	if( mptrPRE3DScalarBarActorY != NULL )
	{
		mptrPRE3DScalarBarActorY->Delete();
		mptrPRE3DScalarBarActorY = NULL;
	}

	if( mptrPRE3DScalarBarActorZ != NULL )
	{
		mptrPRE3DScalarBarActorZ->Delete();
		mptrPRE3DScalarBarActorZ = NULL;
	}

	if( mptrPRE3DonUSImageViewerX != NULL )
	{
		mptrPRE3DonUSImageViewerX->Delete();
		mptrPRE3DonUSImageViewerX = NULL;
	}

	if( mptrPRE3DonUSImageViewerY != NULL )
	{
		mptrPRE3DonUSImageViewerY->Delete();
		mptrPRE3DonUSImageViewerY = NULL;
	}

	if( mptrPRE3DonUSImageViewerZ != NULL )
	{
		mptrPRE3DonUSImageViewerZ->Delete();
		mptrPRE3DonUSImageViewerZ = NULL;
	}
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::Initialize(vtkProbeCalibrationController* calibrationController)
{
	if ( this->Initialized )
	{
		return; 
	}

	this->CalibrationController = calibrationController; 
	
	this->SetupDataObjects(); 
	this->SetupPlotActors(); 

	vtkSmartPointer<vtkTransform> transformUSImageToWorld = vtkSmartPointer<vtkTransform>::New(); 
	transformUSImageToWorld->Concatenate(this->CalibrationController->GetTransformImageToTemplate()); 
	transformUSImageToWorld->Scale(1,-1,-1); 
	transformUSImageToWorld->Update(); 
	this->SetTransformUSImageToWorld(transformUSImageToWorld); 

	// Set up the main plot renderer
	vtkSmartPointer<vtkRenderer> plotRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	plotRenderer->SetBackground(0,0,0);
	plotRenderer->AddActor2D(mptrPlotActor);
	this->SetPlotRenderer(plotRenderer); 

	vtkSmartPointer<vtkImageActor> realtimeImageActor = vtkSmartPointer<vtkImageActor>::New();
	realtimeImageActor->VisibilityOn(); 
	this->SetRealtimeImageActor(realtimeImageActor); 

	vtkSmartPointer<vtkActor> centerOfRotationActor = vtkSmartPointer<vtkActor>::New();
	centerOfRotationActor->VisibilityOff(); 
	this->SetCenterOfRotationActor(centerOfRotationActor); 

	vtkSmartPointer<vtkActor> phantomWiresActor = vtkSmartPointer<vtkActor>::New();
	phantomWiresActor->VisibilityOff(); 
	this->SetPhantomWiresActor(phantomWiresActor); 
	
	// Set up the realtime renderer
	vtkSmartPointer<vtkRenderer> realtimeRenderer = vtkSmartPointer<vtkRenderer>::New(); 
	realtimeRenderer->SetBackground(0,0,0); 
	this->SetRealtimeRenderer(realtimeRenderer); 
	
	// Add image actor to the realtime renderer 
	this->GetRealtimeRenderer()->AddActor(this->GetRealtimeImageActor()); 
	this->GetRealtimeRenderer()->AddActor(this->GetCenterOfRotationActor()); 
	this->GetRealtimeRenderer()->AddActor(this->GetPhantomWiresActor()); 

	vtkSmartPointer<vtkCamera> templateCamera = vtkSmartPointer<vtkCamera>::New(); 
	templateCamera->SetPosition(30,30,-150); 
	templateCamera->SetFocalPoint(30,30,0); 
	templateCamera->SetViewUp(0, -1, 0);
	templateCamera->SetClippingRange(0.1, 1000);
	templateCamera->ParallelProjectionOn(); 
	templateCamera->SetParallelScale(40);	
	this->SetTemplateCamera(templateCamera); 

	vtkSmartPointer<vtkCamera> imageCamera = vtkSmartPointer<vtkCamera>::New(); 
	double imageCenterX = this->GetCalibrationController()->GetImageWidthInPixels()/2.0; 
	double imageCenterY = this->GetCalibrationController()->GetImageHeightInPixels()/2.0; 
	imageCamera->SetPosition(imageCenterX, imageCenterY, 150); 
	imageCamera->SetFocalPoint(imageCenterX, imageCenterY, 0); 
	imageCamera->SetViewUp(0, 1, 0);
	imageCamera->SetClippingRange(0.1, 1000);
	imageCamera->ParallelProjectionOn(); 
	imageCamera->SetParallelScale(imageCenterY);	
	this->SetImageCamera(imageCamera); 

	this->GetRealtimeRenderer()->GetActiveCamera()->DeepCopy(this->GetImageCamera());
	
	this->SetupColorLUT(); 

	this->SetupColorBars(); 

	this->SetupImageViewers(); 

	this->SetupTemplateOverlay(); 
	
	this->InitializedOn(); 

}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupTemplateOverlay()
{
	vtkSmartPointer<vtkXMLDataElement> configTemplateModel = vtkXMLUtilities::ReadElementFromFile(this->GetTemplateModelConfigFileName()); 
	// Read Template model file 
	this->ReadTemplateModelConfiguration(configTemplateModel); 

	this->TemplateGridActors.TransverseGridActors = vtkSmartPointer<vtkCollection>::New(); 
	this->TemplateGridActors.TransverseLetterActors = vtkSmartPointer<vtkCollection>::New(); 
	this->TemplateGridActors.LongitudinalGridActors = vtkSmartPointer<vtkCollection>::New(); 
	this->TemplateGridActors.LongitudinalLetterActors = vtkSmartPointer<vtkCollection>::New(); 

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
	int originY = this->GetCalibrationController()->GetImageHeightInPixels() - this->GetCalibrationController()->GetUSImageFrameOriginYInPixels(); 
	this->GetCenterOfRotationActor()->SetPosition(originX, originY, 0); 
	this->GetCenterOfRotationActor()->Modified(); 
	this->ShowCenterOfRotation(); 
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::OverlayTemplate()
{
	if ( !this->Initialized )
	{
		return; 
	}

	int originX = this->GetCalibrationController()->GetUSImageFrameOriginXInPixels(); 
	int originY = this->GetCalibrationController()->GetImageHeightInPixels() - this->GetCalibrationController()->GetUSImageFrameOriginYInPixels(); 

	// Set upper left corner as image origin
	this->GetRealtimeImageActor()->SetPosition( 0, -this->GetCalibrationController()->GetImageHeightInPixels(), 0);
	this->GetRealtimeImageActor()->SetUserTransform( this->GetTransformUSImageToWorld() ); 

	// Create template grid actors
	this->CreateTemplateGridActors(); 

	// Display the default transverse mode
	this->DisplayTemplateGridInTransverseMode(); 
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::OverlayPhantom()
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
	transformTemplateToUsImage->DeepCopy(this->GetTransformUSImageToWorld()); 
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
	this->GetPhantomWiresActor()->SetUserTransform( this->GetTransformUSImageToWorld() );
	this->GetPhantomWiresActor()->Modified();
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::RemoveTemplate()
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->GetRealtimeImageActor()->SetPosition( 0, 0, 0); 
	vtkSmartPointer<vtkTransform> identity = vtkSmartPointer<vtkTransform>::New(); 
	this->GetRealtimeImageActor()->SetUserTransform( identity ); 
	this->RemoveTemplateGridInTransverseMode(); 
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupDataObjects()
{
		// Data arrays
		mptrNumberOfData = vtkIntArray::New();
		mptrNumberOfData->SetName("NumberOfData");

		mptrPRE3DinXAxis = vtkDoubleArray::New();
		mptrPRE3DinXAxis->SetName("PRE3DinXAxis");

		mptrPRE3DinYAxis = vtkDoubleArray::New();
		mptrPRE3DinYAxis->SetName("PRE3DinYAxis");

		mptrPRE3DinZAxis = vtkDoubleArray::New();
		mptrPRE3DinZAxis->SetName("PRE3DinZAxis");

		// Field Data
		mptrFieldDataPRE3DinXAxis = vtkFieldData::New();	
		mptrFieldDataPRE3DinXAxis->AllocateArrays(2);
		mptrFieldDataPRE3DinXAxis->AddArray( mptrNumberOfData );
		mptrFieldDataPRE3DinXAxis->AddArray( mptrPRE3DinXAxis );

		mptrFieldDataPRE3DinYAxis = vtkFieldData::New();
		mptrFieldDataPRE3DinYAxis->AllocateArrays(2);
		mptrFieldDataPRE3DinYAxis->AddArray( mptrNumberOfData );
		mptrFieldDataPRE3DinYAxis->AddArray( mptrPRE3DinYAxis );

		mptrFieldDataPRE3DinZAxis = vtkFieldData::New();
		mptrFieldDataPRE3DinZAxis->AllocateArrays(2);
		mptrFieldDataPRE3DinZAxis->AddArray( mptrNumberOfData );
		mptrFieldDataPRE3DinZAxis->AddArray( mptrPRE3DinZAxis );

		// Data Objects
		mptrDataObjectPRE3DinXAxis = vtkDataObject::New();
		mptrDataObjectPRE3DinXAxis->SetFieldData( mptrFieldDataPRE3DinXAxis );

		mptrDataObjectPRE3DinYAxis = vtkDataObject::New();	
		mptrDataObjectPRE3DinYAxis->SetFieldData( mptrFieldDataPRE3DinYAxis );

		mptrDataObjectPRE3DinZAxis = vtkDataObject::New();
		mptrDataObjectPRE3DinZAxis->SetFieldData( mptrFieldDataPRE3DinZAxis );

}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupPlotActors()
{
		// Add data objects
		// Each data object corresponds to one curve to draw
		mptrPlotActor = vtkXYPlotActor::New();
		mptrPlotActor->AddDataObjectInput( mptrDataObjectPRE3DinXAxis );
		mptrPlotActor->AddDataObjectInput( mptrDataObjectPRE3DinYAxis );
		mptrPlotActor->AddDataObjectInput( mptrDataObjectPRE3DinZAxis );

		// Set which parts of the data object are to be used for which axis
		mptrPlotActor->SetDataObjectXComponent(0, 0);
		mptrPlotActor->SetDataObjectYComponent(0, 1);
		mptrPlotActor->SetDataObjectYComponent(1, 1);
		mptrPlotActor->SetDataObjectYComponent(2, 1);
		mptrPlotActor->SetPlotLabel(0, "x-axis");
		mptrPlotActor->SetPlotLabel(1, "y-axis");
		mptrPlotActor->SetPlotLabel(2, "z-axis");

		// Set up the lookup table for the appropriate range of colours
		mptrPlotLookupTable = vtkLookupTable::New();
		mptrPlotLookupTable->Build();
		double RGBColors[3][3];
		for(int i = 0; i < 3; i++ )
		{
			mptrPlotLookupTable->GetColor((double)i/(3-i), RGBColors[i]);
		}
		// Change the colour of the separate lines
		//mptrPlotActor->SetPlotColor(0, RGBColors[0][0], RGBColors[0][1], RGBColors[0][2]);
		//mptrPlotActor->SetPlotColor(1, RGBColors[1][0], RGBColors[1][1], RGBColors[1][2]);
		//mptrPlotActor->SetPlotColor(2, RGBColors[2][0], RGBColors[2][1], RGBColors[2][2]);
		mptrPlotActor->SetPlotColor(0, (double)mColorYellow[0]/255, (double)mColorYellow[1]/255, (double)mColorYellow[2]/255 );
		mptrPlotActor->SetPlotColor(1, (double)mColorCyan[0]/255, (double)mColorCyan[1]/255, (double)mColorCyan[2]/255 );
		mptrPlotActor->SetPlotColor(2, (double)mColorRed[0]/255,(double)mColorRed[1]/255, (double)mColorRed[2]/255 );

		mptrPlotActor->SetPosition(0, 0); 
		mptrPlotActor->SetWidth(1);
		mptrPlotActor->SetHeight(1);
		// Set the title
		mptrPlotActor->SetTitle("3D Point Reconstruction Error");
		mptrPlotActor->SetXTitle("Number of Input Data");
		mptrPlotActor->SetYTitle("PRE3D (mm)");
		mptrPlotActor->SetXValuesToValue();
		mptrPlotActor->GetXAxisActor2D()->GetProperty()->SetColor(0,0,0);
		mptrPlotActor->GetYAxisActor2D()->GetProperty()->SetColor(0,0,0);
		mptrPlotActor->SetNumberOfXLabels(10);
		mptrPlotActor->SetNumberOfYLabels(10);
		mptrPlotActor->LegendOn();

		mptrPlotActor->VisibilityOff(); 
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupColorLUT()
{

	// Set up a global color lookup table for PRE3D distributions
		// ---------------------------------------------------------

		mptrPRE3DColorLookupTable = vtkLookupTable::New();
		// PRE3D range: 0.5mm to 3mm
		mptrPRE3DColorLookupTable->SetTableRange(0, 3.0);
		//mptrPRE3DColorLookupTable->SetNumberOfTableValues(6);
		// Full Hue from Yellow (60) all the way up to Red (360).
		mptrPRE3DColorLookupTable->SetHueRange (0.15, 1); 
		// Set Saturation and Value to full scale (1) such that
		// the top circle of the Hue could be used (in HSV space)
		mptrPRE3DColorLookupTable->SetSaturationRange (1, 1);
		mptrPRE3DColorLookupTable->SetValueRange (1, 1);
		mptrPRE3DColorLookupTable->Build();

}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupColorBars()
{
	// Set up the color bars
		// ----------------------

		// The scalar bar for color distribution in PRE3D-X
		mptrPRE3DScalarBarActorX  = vtkScalarBarActor::New();
		mptrPRE3DScalarBarActorX->SetTitle("PRE3D-x\n(mm)");
		mptrPRE3DScalarBarActorX->SetLookupTable( mptrPRE3DColorLookupTable );
		mptrPRE3DScalarBarActorX->SetOrientationToVertical();
		mptrPRE3DScalarBarActorX->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
		mptrPRE3DScalarBarActorX->GetPositionCoordinate()->SetValue(0.1, 0.05);
		mptrPRE3DScalarBarActorX->SetWidth( 0.1 );
		mptrPRE3DScalarBarActorX->SetHeight( 0.9 );
		mptrPRE3DScalarBarActorX->SetPosition( 0.9, 0.1 );
		mptrPRE3DScalarBarActorX->SetLabelFormat( "%-#6.2f" );
		// This would be used to divide the PRE3D range from 0mm to 3mm
		mptrPRE3DScalarBarActorX->SetNumberOfLabels(7);
		mptrPRE3DScalarBarActorX->GetLabelTextProperty()->SetColor( 1, 1, 1 );
		mptrPRE3DScalarBarActorX->GetTitleTextProperty()->SetColor( 1, 1, 1 );

		// The scalar bar for color distribution in PRE3D-Y
		mptrPRE3DScalarBarActorY  = vtkScalarBarActor::New();
		mptrPRE3DScalarBarActorY->SetTitle("PRE3D-y\n(mm)");
		mptrPRE3DScalarBarActorY->SetLookupTable( mptrPRE3DColorLookupTable );
		mptrPRE3DScalarBarActorY->SetOrientationToVertical();
		mptrPRE3DScalarBarActorY->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
		mptrPRE3DScalarBarActorY->GetPositionCoordinate()->SetValue(0.1, 0.05);
		mptrPRE3DScalarBarActorY->SetWidth( 0.1 );
		mptrPRE3DScalarBarActorY->SetHeight( 0.9 );
		mptrPRE3DScalarBarActorY->SetPosition( 0.9, 0.1 );
		mptrPRE3DScalarBarActorY->SetLabelFormat( "%-#6.2f" );
		// This would be used to divide the PRE3D range from 0mm to 3mm
		mptrPRE3DScalarBarActorY->SetNumberOfLabels(7);
		mptrPRE3DScalarBarActorY->GetLabelTextProperty()->SetColor( 1, 1, 1 );
		mptrPRE3DScalarBarActorY->GetTitleTextProperty()->SetColor( 1, 1, 1 );

		// The scalar bar for color distribution in PRE3D-Z
		mptrPRE3DScalarBarActorZ  = vtkScalarBarActor::New();
		mptrPRE3DScalarBarActorZ->SetTitle("PRE3D-z\n(mm)");
		mptrPRE3DScalarBarActorZ->SetLookupTable( mptrPRE3DColorLookupTable );
		mptrPRE3DScalarBarActorZ->SetOrientationToVertical();
		mptrPRE3DScalarBarActorZ->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
		mptrPRE3DScalarBarActorZ->GetPositionCoordinate()->SetValue(0.1, 0.05);
		mptrPRE3DScalarBarActorZ->SetWidth( 0.1 );
		mptrPRE3DScalarBarActorZ->SetHeight( 0.9 );
		mptrPRE3DScalarBarActorZ->SetPosition( 0.9, 0.1 );
		mptrPRE3DScalarBarActorZ->SetLabelFormat( "%-#6.2f" );
		// This would be used to divide the PRE3D range from 0mm to 3mm
		mptrPRE3DScalarBarActorZ->SetNumberOfLabels(7);
		mptrPRE3DScalarBarActorZ->GetLabelTextProperty()->SetColor( 1, 1, 1 );
		mptrPRE3DScalarBarActorZ->GetTitleTextProperty()->SetColor( 1, 1, 1 );

}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SetupImageViewers()
{
		// Set up the image viewers
		// -------------------------
	
		// The image viewer to map PRE3D-X on the US image
		mptrPRE3DonUSImageViewerX = vtkImageViewer::New();
		// Render Window size: [mImageWidthInPixels*1.125, mImageHeightInPixels]
		// This is to ensure there is enough space for the color bar display
		mptrPRE3DonUSImageViewerX->GetRenderWindow()->SetSize( 
			int(this->GetCalibrationController()->GetImageWidthInPixels()*1.125), this->GetCalibrationController()->GetImageHeightInPixels() );
		// The background is set to be all black for the renderer
		mptrPRE3DonUSImageViewerX->GetRenderer()->SetBackground(0, 0, 0);
		mptrPRE3DonUSImageViewerX->GetRenderer()->AddActor( mptrPRE3DScalarBarActorX );
		mptrPRE3DonUSImageViewerX->SetColorLevel(138.5);
		mptrPRE3DonUSImageViewerX->SetColorWindow(233);

		// The image viewer to map PRE3D-Y on the US image
		mptrPRE3DonUSImageViewerY = vtkImageViewer::New();
		// Render Window size: [mImageWidthInPixels*1.125, mImageHeightInPixels]
		// This is to ensure there is enough space for the color bar display
		mptrPRE3DonUSImageViewerY->GetRenderWindow()->SetSize( 
			int(this->GetCalibrationController()->GetImageWidthInPixels()*1.125), this->GetCalibrationController()->GetImageHeightInPixels() );
		// The background is set to be all black for the renderer
		mptrPRE3DonUSImageViewerY->GetRenderer()->SetBackground(0, 0, 0);
		mptrPRE3DonUSImageViewerY->GetRenderer()->AddActor( mptrPRE3DScalarBarActorY );
		mptrPRE3DonUSImageViewerY->SetColorLevel(138.5);
		mptrPRE3DonUSImageViewerY->SetColorWindow(233);

		// The image viewer to map PRE3D-Z on the US image
		mptrPRE3DonUSImageViewerZ = vtkImageViewer::New();
		// Render Window size: [mImageWidthInPixels*1.125, mImageHeightInPixels]
		// This is to ensure there is enough space for the color bar display
		mptrPRE3DonUSImageViewerZ->GetRenderWindow()->SetSize( 
			int(this->GetCalibrationController()->GetImageWidthInPixels()*1.125), this->GetCalibrationController()->GetImageHeightInPixels() );
		// The background is set to be all black for the renderer
		mptrPRE3DonUSImageViewerZ->GetRenderer()->SetBackground(0, 0, 0);
		mptrPRE3DonUSImageViewerZ->GetRenderer()->AddActor( mptrPRE3DScalarBarActorZ );
		mptrPRE3DonUSImageViewerZ->SetColorLevel(138.5);
		mptrPRE3DonUSImageViewerZ->SetColorWindow(233);

}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::AddFrameToRealtimeRenderer(vtkImageData* frame)
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->GetRealtimeImageActor()->SetInput(frame); 
	this->GetRealtimeImageActor()->Modified(); 
	if ( this->GetRealtimeRenderer()->GetRenderWindow() != NULL ) 
	{
		this->GetRealtimeRenderer()->GetRenderWindow()->Render(); 
	}
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::PlotPRE3D()
{
	if ( !this->Initialized )
	{
		return; 
	}

	if( mptrNumberOfData->GetNumberOfTuples() <= 0 )
	{
		return void(1);		
	}

	PlotPRE3Ddistribution ();
	SavePRE3DplotToImage ();
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SavePRE3DplotToImage()
{
	try
	{
		if ( !this->Initialized  || this->PlotRenderer->GetRenderWindow() == NULL ) 
		{
			return; 
		}

		// Construct the calibration result file name with path and timestamp
		const std::string PRE3DDistributionToImageFileName = std::string(this->GetOutputPath()) + std::string("/") + this->GetCalibrationController()->GetCalibrator()->getCalibrationTimeStampInString() + ".PRE3Dplot.tiff";

		// convert the render window to an image
		this->PlotRenderer->GetRenderWindow()->OffScreenRenderingOn(); 
		this->PlotRenderer->GetRenderWindow()->Render(); 
		vtkSmartPointer<vtkWindowToImageFilter> renderWindowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();

		renderWindowToImageFilter->SetInput( this->PlotRenderer->GetRenderWindow() ); 

		// save the image to file
		vtkSmartPointer<vtkTIFFWriter> imageWriter = vtkSmartPointer<vtkTIFFWriter>::New();
		imageWriter->SetInput( renderWindowToImageFilter->GetOutput() );
		imageWriter->SetFileName( PRE3DDistributionToImageFileName.c_str() );
		imageWriter->Update();
		this->PlotRenderer->GetRenderWindow()->OffScreenRenderingOff(); 
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> Unable to save the PRE3D plot to image!!!"
			<< "  This operation was discarded ...\n";
	}
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::UpdatePlotComponents()
{
	if ( !this->Initialized )
	{
		return; 
	}

	mptrPlotActor->VisibilityOn(); 
	// Number of input data for calibration
	int mCurrentPRE3DdistributionID = mptrNumberOfData->InsertNextValue( this->GetCalibrationController()->GetCalibrator()->getNumberOfDataForCalibration() );
	mptrNumberOfData->Modified();

	// The averaged 3D Point Reconstruction Error (PRE3D) from the 
	// validation positions in the US probe frame.
	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]
	// [ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]
	// [ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]
	// [ vector 9	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - rms : root mean square;
	// - std : standard deviation.
	// - validation data confidence level: this is a percentage of 
	//   the independent validation data used to produce the final
	//   validation results.  It serves as an effective way to get 
	//   rid of corrupted data (or outliers) in the validation 
	//   dataset.  Default value: 0.95 (or 95%), meaning the top 
	//   ranked 95% of the ascendingly-ordered PRE3D values from the 
	//   validation data would be accepted as the valid PRE3D values.
	vtkstd::vector<double> absPRE3DAnalysis4ValidationPositionsInUSProbeFrame = this->GetCalibrationController()->GetCalibrator()->getPRE3DAnalysis4ValidationDataSet(); 

	const double AbsPRE3D_X_mm = 
		1000 * absPRE3DAnalysis4ValidationPositionsInUSProbeFrame.at(0);
	const double AbsPRE3D_Y_mm = 
		1000 * absPRE3DAnalysis4ValidationPositionsInUSProbeFrame.at(3);
	const double AbsPRE3D_Z_mm = 
		1000 * absPRE3DAnalysis4ValidationPositionsInUSProbeFrame.at(6);

	mptrPRE3DinXAxis->InsertNextValue( AbsPRE3D_X_mm );
	mptrPRE3DinXAxis->Modified();

	mptrPRE3DinYAxis->InsertNextValue( AbsPRE3D_Y_mm );
	mptrPRE3DinYAxis->Modified();

	mptrPRE3DinZAxis->InsertNextValue( AbsPRE3D_Z_mm );
	mptrPRE3DinZAxis->Modified();

	// Update the plot labels
	char LabelX[256] = {'\0'};
	sprintf( LabelX, "x=%.2f", AbsPRE3D_X_mm + 0.005 );
	char LabelY[256] = {'\0'};
	sprintf( LabelY, "y=%.2f", AbsPRE3D_Y_mm + 0.005 );
	char LabelZ[256] = {'\0'};
	sprintf( LabelZ, "z=%.2f", AbsPRE3D_Z_mm + 0.005 );

	mptrPlotActor->SetPlotLabel( 0, LabelX );
	mptrPlotActor->SetPlotLabel( 1, LabelY );
	mptrPlotActor->SetPlotLabel( 2, LabelZ );

	// change the plot renderer scaling
	if ( mptrPRE3DinXAxis->GetMaxId() > 20 )
	{
		double * dataX = mptrPRE3DinXAxis->GetPointer( mptrPRE3DinXAxis->GetMaxId() - 20 ); 
		double * dataY = mptrPRE3DinYAxis->GetPointer( mptrPRE3DinYAxis->GetMaxId() - 20 ); 
		double * dataZ = mptrPRE3DinZAxis->GetPointer( mptrPRE3DinZAxis->GetMaxId() - 20 ); 

		double dataMax(0); 
		for ( int i = 0; i < 20; i++ )
		{
			if ( *dataX++ > dataMax)
			{
				dataMax = *dataX; 
			}

			if ( *dataY++ > dataMax)
			{
				dataMax = *dataY; 
			}

			if ( *dataZ++ > dataMax)
			{
				dataMax = *dataZ; 
			}
		}

		mptrPlotActor->SetYRange(0, 2.0 * dataMax); 
	}


	mptrPlotActor->Modified();
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::PlotPRE3Ddistribution()
{
	if ( !this->Initialized )
	{
		return; 
	}

	if ( this->PlotRenderer->GetRenderWindow() != NULL ) 
	{
		this->PlotRenderer->GetRenderWindow()->Render();
	}
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::MapPRE3DdistributionToUSImage( unsigned char* imageData )
{
	try
	{
		if ( !this->Initialized )
	{
		return; 
	}

		// create an importer to read the data back in
		vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
		importer->SetWholeExtent(0,this->GetCalibrationController()->GetImageWidthInPixels() - 1,0,this->GetCalibrationController()->GetImageHeightInPixels() - 1,0,0);
		importer->SetDataExtentToWholeExtent();
		importer->SetDataScalarTypeToUnsignedChar();
		importer->SetImportVoidPointer(imageData);
		importer->SetNumberOfScalarComponents(1); 
		importer->Update();
	
		vtkSmartPointer<vtkImageFlip> imageFlip = vtkSmartPointer<vtkImageFlip>::New(); 
		imageFlip->SetInput( importer->GetOutput() ); 
		imageFlip->SetFilteredAxis(1); 
		imageFlip->Update(); 

		std::vector<Phantom::vnl_vector_double> ValidationPositionsInUSImageFrame
			= this->GetCalibrationController()->GetCalibrator()->getValidationDataSetInUSImageFrame ();

		const unsigned int NumberOfValidationPositions = 
			ValidationPositionsInUSImageFrame.size();

		if( NumberOfValidationPositions !=
			this->GetCalibrationController()->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN().cols() )
		{
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< ">>>>>>>> Size of the validation set ("
				<< NumberOfValidationPositions << ") does not match "
				<< "the number of columns of the raw PRE3D matrix (" 
				<< this->GetCalibrationController()->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN().cols()
				<< ")!!!  Thrown up ...\n";

			throw;
		}

		vtkSmartPointer<vtkImageData> xPRE3DonUSImage = vtkSmartPointer<vtkImageData>::New(); 
		xPRE3DonUSImage->CopyStructure(imageFlip->GetOutput()); 
		xPRE3DonUSImage->SetNumberOfScalarComponents(3); 
		xPRE3DonUSImage->SetScalarTypeToUnsignedChar(); 
		xPRE3DonUSImage->Update(); 

		for (int y = 0; y < this->GetCalibrationController()->GetImageHeightInPixels(); y++)
		{
			for (int x = 0; x < this->GetCalibrationController()->GetImageWidthInPixels(); x++)
			{
				unsigned char* xPRE3DPointer = static_cast<unsigned char*> ( xPRE3DonUSImage->GetScalarPointer(x, y, 0) ); 
				unsigned char* importedImgPointer = static_cast<unsigned char*> ( imageFlip->GetOutput()->GetScalarPointer(x, y, 0) ); 

				for ( int components = 0 ; components < xPRE3DonUSImage->GetNumberOfScalarComponents(); components++ )
				{
					*xPRE3DPointer++ = *importedImgPointer; 
				}
			}
		}
		
		vtkSmartPointer<vtkImageData> yPRE3DonUSImage = vtkSmartPointer<vtkImageData>::New();
		yPRE3DonUSImage->DeepCopy( xPRE3DonUSImage ); 
		yPRE3DonUSImage->Update(); 

		vtkSmartPointer<vtkImageData> zPRE3DonUSImage = vtkSmartPointer<vtkImageData>::New(); 
		zPRE3DonUSImage->DeepCopy( xPRE3DonUSImage );
		zPRE3DonUSImage->Update(); 

		// We are only interested in top-ranked validation data w.r.t. to 
		// their PRE3D euclidean distance in an asending order.  The default
		// cutting ratio is 95% of the top-ranked data.  This would 
		// effectively get rid of outliers (e.g., bad validation data) in
		// the calibration evaluation.
		vtkstd::vector<double> absPRE3DAnalysis4ValidationPositionsInUSProbeFrame = this->GetCalibrationController()->GetCalibrator()->getPRE3DAnalysis4ValidationDataSet(); 

		const double ValidationDataConfidenceLevel = 
			absPRE3DAnalysis4ValidationPositionsInUSProbeFrame.at(9);
		const int NumberOfTopRankedCalibrationData = 
			floor( (double)NumberOfValidationPositions * ValidationDataConfidenceLevel + 0.5 );
		const double PRE3DeuclideanDistanceThreshold = 
			this->GetCalibrationController()->GetCalibrator()->getSortedPRE3DsRAWMatrixforValidationDataSet4xN().get(3, NumberOfTopRankedCalibrationData-1);

		for( unsigned int i = 0; i < NumberOfValidationPositions; i++ )
		{
			if( this->GetCalibrationController()->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN().get(3, i) > 
				PRE3DeuclideanDistanceThreshold )
			{
				continue;
			}

			vnl_vector<double> ThisValidationPointInTRUSImageFrame = 
				ValidationPositionsInUSImageFrame.at(i);

			vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
			this->GetCalibrationController()->ConvertVtkMatrixToVnlMatrixInMeter(this->GetCalibrationController()->GetTransformImageHomeToUserImageHome()->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 

			// Transform matrix from TRUS Image frame to Original Image frame
			vnl_matrix_inverse<double> inverseMatrix( transformOrigImageFrame2TRUSImageFrameMatrix4x4 );
			vnl_matrix<double> TransformTRUSImageFrameToOrigImageFrameMatrix4x4 = 
				inverseMatrix.inverse();
			// Make sure the last row in homogeneous transform is [0 0 0 1]
			vnl_vector<double> lastRow(4,0);
			lastRow.put(3, 1);
			TransformTRUSImageFrameToOrigImageFrameMatrix4x4.set_row(3, lastRow);

			// Note: the pixel location will have to be transferred back
			//       to the original image frame (from the US image frame).
			vnl_vector<double> ThisValidationPointInOrigImageFrame =
				TransformTRUSImageFrameToOrigImageFrameMatrix4x4 *
				ThisValidationPointInTRUSImageFrame;


			//       to the original image frame (not the US image frame).
			// Also, bear in mind, in VTK BMPReader has the lower-left 
			// corner as the default image origin (which is different than ours in Y axis )
			const int PixelCoordsXInVTKImageFrame = 
				floor( ThisValidationPointInOrigImageFrame.get(0) + 0.5 );
			const int PixelCoordsYInVTKImageFrame = 
				this->GetCalibrationController()->GetImageHeightInPixels() - floor( ThisValidationPointInOrigImageFrame.get(1) + 0.5 ) ;


			// ========================================================
			// IMPORTANT:
			// We use the defined lookup table to display the PRE3D
			// distributions on the US image at the positions of the
			// validation data set, for X, Y and Z axis, respectively:
			// NOTE: the range value is an approximated value.
			// - Yellow:		     PRE3D <= 0.5 mm
			// - Green:		0.5 mm < PRE3D <= 1.0 mm
			// - Cyan:		1.0 mm < PRE3D <= 1.5 mm
			// - Blue:		1.5 mm < PRE3D <= 2.0 mm
			// - Magenta:	2.0 mm < PRE3D <= 2.5 mm
			// - Red:		2.5 mm < PRE3D
			// ========================================================

			const vnl_vector<double> AbsPRE3DofThisPoint = 
				this->GetCalibrationController()->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN().get_column(i).apply( fabs );

			const double AbsPRE3D_X_mm = AbsPRE3DofThisPoint.get(0) * 1000;
			const double AbsPRE3D_Y_mm = AbsPRE3DofThisPoint.get(1) * 1000;
			const double AbsPRE3D_Z_mm = AbsPRE3DofThisPoint.get(2) * 1000;

			//int xColorInRGBarray[3] = {0};
			//getColorForPRE3Ddistribution( AbsPRE3D_X_mm, xColorInRGBarray );
			//int yColorInRGBarray[3] = {0};
			//getColorForPRE3Ddistribution( AbsPRE3D_Y_mm, yColorInRGBarray );
			//int zColorInRGBarray[3] = {0};
			//getColorForPRE3Ddistribution( AbsPRE3D_Z_mm, zColorInRGBarray );

			// Use the lookup table to map colors given the PRE3D value
			double xColorInRGBarray[3] = {0};
			mptrPRE3DColorLookupTable->GetColor( AbsPRE3D_X_mm, xColorInRGBarray );
			double yColorInRGBarray[3] = {0};
			mptrPRE3DColorLookupTable->GetColor( AbsPRE3D_Y_mm, yColorInRGBarray );
			double zColorInRGBarray[3] = {0}; 
			mptrPRE3DColorLookupTable->GetColor( AbsPRE3D_Z_mm, zColorInRGBarray );

			xPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 0, xColorInRGBarray[0]*255 );
			xPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 1, xColorInRGBarray[1]*255 );
			xPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 2, xColorInRGBarray[2]*255 );

			yPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 0, yColorInRGBarray[0]*255 );
			yPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 1, yColorInRGBarray[1]*255 );
			yPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 2, yColorInRGBarray[2]*255 );	

			zPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 0, zColorInRGBarray[0]*255 );
			zPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 1, zColorInRGBarray[1]*255 );
			zPRE3DonUSImage->SetScalarComponentFromDouble( 
				PixelCoordsXInVTKImageFrame, PixelCoordsYInVTKImageFrame, 0, 2, zColorInRGBarray[2]*255 );
		}

		// Display and save the images to file
		// ====================================

		// Construct the image file name with path and timestamp
		vtkstd::string calibrationTimestampInString = this->GetCalibrationController()->GetCalibrator()->getCalibrationTimeStampInString();
		const std::string xPRE3DonUSImageFileName = 
			std::string(this->GetOutputPath()) + std::string("/") + calibrationTimestampInString + ".PRE3DColorMapX.tiff";
		// Construct the image file name with path and timestamp
		const std::string yPRE3DonUSImageFileName = 
			std::string(this->GetOutputPath()) + std::string("/") + calibrationTimestampInString + ".PRE3DColorMapY.tiff";
		// Construct the image file name with path and timestamp
		const std::string zPRE3DonUSImageFileName = 
			std::string(this->GetOutputPath()) + std::string("/") + calibrationTimestampInString + ".PRE3DColorMapZ.tiff";

		vtkSmartPointer<vtkTIFFWriter> imgWriter = vtkSmartPointer<vtkTIFFWriter>::New(); 
		vtkSmartPointer<vtkWindowToImageFilter> renderWindowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();

		// Display and save the PRE3D-x on US image with time-stamped file name
		xPRE3DonUSImage->Update();
		mptrPRE3DonUSImageViewerX->SetInput( xPRE3DonUSImage );
		mptrPRE3DonUSImageViewerX->OffScreenRenderingOn();
		mptrPRE3DonUSImageViewerX->GetRenderWindow()->Render();
		// convert the render window to an image
		renderWindowToImageFilter->SetInput( mptrPRE3DonUSImageViewerX->GetRenderWindow() );
		// save the image to file
		imgWriter->SetInput( renderWindowToImageFilter->GetOutput() );
		imgWriter->SetFileName( xPRE3DonUSImageFileName.c_str() );
		imgWriter->Write();

		// Display and save the PRE3D-y on US image with time-stamped file name
		yPRE3DonUSImage->Update();
		mptrPRE3DonUSImageViewerY->SetInput( yPRE3DonUSImage );
		mptrPRE3DonUSImageViewerY->OffScreenRenderingOn();
		mptrPRE3DonUSImageViewerY->GetRenderWindow()->Render();
		// convert the render window to an image
		renderWindowToImageFilter->SetInput( mptrPRE3DonUSImageViewerY->GetRenderWindow() );
		// save the image to file
		imgWriter->SetInput( renderWindowToImageFilter->GetOutput() );
		imgWriter->SetFileName( yPRE3DonUSImageFileName.c_str() );
		imgWriter->Write();

		// Display and save the PRE3D-z on US image with time-stamped file name
		zPRE3DonUSImage->Update();
		mptrPRE3DonUSImageViewerZ->SetInput( zPRE3DonUSImage );
		mptrPRE3DonUSImageViewerZ->OffScreenRenderingOn();
		mptrPRE3DonUSImageViewerZ->GetRenderWindow()->Render();
		// convert the render window to an image
		renderWindowToImageFilter->SetInput( mptrPRE3DonUSImageViewerZ->GetRenderWindow() );
		// save the image to file
		imgWriter->SetInput( renderWindowToImageFilter->GetOutput() );
		imgWriter->SetFileName( zPRE3DonUSImageFileName.c_str() );
		imgWriter->Write();

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> Unable to map the PRE3D distribution to the US image!!!"
			<< "  This operation was discarded ...\n";
	}
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::GetColorForPRE3Ddistribution( const double PRE3DabsValue, int* ColorInRGBarray )
{
	if ( !this->Initialized )
	{
		return; 
	}

	// ========================================================
	// IMPORTANT:
	// We define the following color scheme to display the PRE3D
	// distributions on the US image at the positions of the
	// validation data set, for X, Y and Z axis, respectively:
	// - Green:		         PRE3D <= 0.5 mm
	// - Yellow:	0.5 mm < PRE3D <= 1.0 mm
	// - Blue:		1.0 mm < PRE3D <= 2.0 mm
	// - Magenta:	2.0 mm < PRE3D <= 3.0 mm
	// - Red:		3.0 mm < PRE3D
	// ========================================================		
	if( PRE3DabsValue <= 0.5 )
	{
		ColorInRGBarray[0] = mColorGreen[0];
		ColorInRGBarray[1] = mColorGreen[1];
		ColorInRGBarray[2] = mColorGreen[2];
	}
	else if( 0.5 < PRE3DabsValue && PRE3DabsValue <= 1.0 )
	{
		ColorInRGBarray[0] = mColorYellow[0];
		ColorInRGBarray[1] = mColorYellow[1];
		ColorInRGBarray[2] = mColorYellow[2];
	}
	else if( 1.0 < PRE3DabsValue && PRE3DabsValue <= 2.0 )
	{
		ColorInRGBarray[0] = mColorBlue[0];
		ColorInRGBarray[1] = mColorBlue[1];
		ColorInRGBarray[2] = mColorBlue[2];
	}
	else if( 2.0 < PRE3DabsValue && PRE3DabsValue <= 3.0 )
	{
		ColorInRGBarray[0] = mColorCyan[0];
		ColorInRGBarray[1] = mColorCyan[1];
		ColorInRGBarray[2] = mColorCyan[2];
	}
	else // PRE3D > 3.0 mm
	{
		ColorInRGBarray[0] = mColorRed[0];
		ColorInRGBarray[1] = mColorRed[1];
		ColorInRGBarray[2] = mColorRed[2];
	}		
}

// Create Template Grid overlaid on the US video
//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::CreateTemplateGridActors()
{
	if ( !this->Initialized )
	{
		return; 
	}

	const double XLONGOFFSET_HOLE = 2.5;

	vtkSmartPointer<vtkAppendPolyData> templateGridPolyData = vtkSmartPointer<vtkAppendPolyData>::New();

	for (unsigned int i = 0; i < TemplateModelHoles.size(); i++)
	{
		// Transverse actors
		vtkSmartPointer<vtkActor> holeTransActor = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkPolyDataMapper> holeTransMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

		if (TemplateModelHoles[i].Representation == HOLE_SPHERE ) 
		{
			vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
			sphere->SetRadius(TemplateModelHoles[i].Radius); 
			holeTransMapper->SetInput(sphere->GetOutput());
		}
		else if (TemplateModelHoles[i].Representation == HOLE_DISK)
		{
			vtkSmartPointer<vtkDiskSource> point = vtkSmartPointer<vtkDiskSource>::New();
			point->SetInnerRadius(TemplateModelHoles[i].Radius-0.15); 
			point->SetOuterRadius(TemplateModelHoles[i].Radius); 
			point->SetCircumferentialResolution(30); 
			holeTransMapper->SetInput(point->GetOutput());
		}

		//templateGridPolyData->AddInput(
		holeTransActor->SetMapper(holeTransMapper);
		holeTransActor->GetProperty()->SetColor(0,1,0);

		holeTransActor->SetPosition(TemplateModelHoles[i].PositionX, TemplateModelHoles[i].PositionY , TemplateModelHoles[i].PositionZ ); 
		holeTransActor->Modified(); 

		holeTransActor->SetUserTransform( this->GetCalibrationController()->GetTransformTemplateHomeToTemplate() ); 

		TemplateGridActors.TransverseGridActors->AddItem(holeTransActor); 

		// Longitude actors
		vtkSmartPointer<vtkPolyDataMapper> holeLongMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		vtkSmartPointer<vtkActor> holeLongActor1 = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkActor> holeLongActor2 = vtkSmartPointer<vtkActor>::New();


		vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
		sphere->SetRadius(0.2); 
		holeLongMapper->SetInput(sphere->GetOutput());

		holeLongActor1->SetMapper(holeLongMapper); 
		holeLongActor1->GetProperty()->SetColor(0,1,0);


		holeLongActor1->SetPosition(TemplateModelHoles[i].PositionX , TemplateModelHoles[i].PositionY , TemplateModelHoles[i].PositionZ); 
		holeLongActor1->Modified(); 
		holeLongActor1->SetUserTransform(this->GetCalibrationController()->GetTransformTemplateHomeToTemplate()); 

		holeLongActor2->SetMapper(holeLongMapper); 
		holeLongActor2->GetProperty()->SetColor(0,1,0);
		holeLongActor2->SetPosition(TemplateModelHoles[i].PositionX + XLONGOFFSET_HOLE, TemplateModelHoles[i].PositionY , TemplateModelHoles[i].PositionZ); 
		holeLongActor2->Modified(); 
		holeLongActor2->SetUserTransform(this->GetCalibrationController()->GetTransformTemplateHomeToTemplate()); 

		TemplateGridActors.LongitudinalGridActors->AddItem(holeLongActor1);
		TemplateGridActors.LongitudinalGridActors->AddItem(holeLongActor2);
	}


	// --------------------  template letters ------------------------ //

	const double XTRANSOFFSET_LETTER = 0.9; 
	const double YTRANSOFFSET_LETTER = 1.3; 
	const double XLONGOFFSET_LETTER_CHAR = 0.9; 
	const double YLONGOFFSET_LETTER_CHAR = 71.3; 
	const double XLONGOFFSET_LETTER_NUM = 0.9; 
	const double YLONGOFFSET_LETTER_NUM = 1.3; 

	for (unsigned int i = 0; i < TemplateModelLetters.size(); i++)
	{
		// Transverse actors
		vtkSmartPointer<vtkTextActor3D> gridTransLettersActor = vtkSmartPointer<vtkTextActor3D>::New();
		gridTransLettersActor->GetTextProperty()->SetColor(0,1,0);
		gridTransLettersActor->GetTextProperty()->SetFontFamilyToArial();
		gridTransLettersActor->GetTextProperty()->SetFontSize(16);
		gridTransLettersActor->GetTextProperty()->SetJustificationToLeft();
		gridTransLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
		gridTransLettersActor->GetTextProperty()->BoldOn(); 

		double * imageScaleTrans = this->GetCalibrationController()->GetTransformImageToTemplate()->GetScale(); 
		gridTransLettersActor->SetScale(imageScaleTrans[0], imageScaleTrans[1], imageScaleTrans[2]);
		gridTransLettersActor->RotateWXYZ(180, 1, 0, 0); 
		//gridTransLettersActor->SetScale(0.2,0.2,0.2); 
		gridTransLettersActor->SetInput(TemplateModelLetters[i].ID.c_str());
		gridTransLettersActor->SetPosition(TemplateModelLetters[i].PositionX - XTRANSOFFSET_LETTER, TemplateModelLetters[i].PositionY + YTRANSOFFSET_LETTER, TemplateModelLetters[i].PositionZ); 
		gridTransLettersActor->Modified(); 

		gridTransLettersActor->SetUserTransform(this->GetCalibrationController()->GetTransformTemplateHomeToTemplate()); 


		TemplateGridActors.TransverseLetterActors->AddItem(gridTransLettersActor); 

		// Longitude actors

		vtkSmartPointer<vtkTextActor3D> gridLongLettersActor = vtkSmartPointer<vtkTextActor3D>::New();
		if (TemplateModelLetters[i].LetterType == LETTER_CHAR)
		{
			//gridLongLettersActor->SetPosition(TemplateModelLetters[i].PositionX - XLONGOFFSET_LETTER_CHAR,-TemplateModelLetters[i].PositionY - YLONGOFFSET_LETTER_CHAR,-TemplateModelLetters[i].PositionZ ); 
			//gridLongLettersActor->GetTextProperty()->SetColor(0,1,0);
			//gridLongLettersActor->GetTextProperty()->SetFontFamilyToArial();
			//gridLongLettersActor->GetTextProperty()->SetFontSize(16);
			//gridLongLettersActor->GetTextProperty()->SetJustificationToLeft();
			//gridLongLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
			//gridLongLettersActor->GetTextProperty()->BoldOn(); 

			//gridLongLettersActor->SetInput(TemplateModelLetters[i].ID);
		}
		else
		{
			gridLongLettersActor->GetTextProperty()->SetColor(0,1,0);
			gridLongLettersActor->GetTextProperty()->SetFontFamilyToArial();
			gridLongLettersActor->GetTextProperty()->SetFontSize(16);
			gridLongLettersActor->GetTextProperty()->SetJustificationToLeft();
			gridLongLettersActor->GetTextProperty()->SetVerticalJustificationToTop();
			gridLongLettersActor->GetTextProperty()->BoldOn(); 
			gridLongLettersActor->SetInput(TemplateModelLetters[i].ID.c_str());
			gridLongLettersActor->SetPosition(TemplateModelLetters[i].PositionX - XLONGOFFSET_LETTER_NUM,TemplateModelLetters[i].PositionY + YLONGOFFSET_LETTER_NUM, TemplateModelLetters[i].PositionZ ); 
		}

		gridLongLettersActor->Modified(); 

		gridLongLettersActor->SetUserTransform(this->GetCalibrationController()->GetTransformTemplateHomeToTemplate()); 
		double * imageScaleLong = this->GetCalibrationController()->GetTransformImageToTemplate()->GetScale(); 
		gridLongLettersActor->SetScale(imageScaleLong[0], imageScaleLong[1], imageScaleLong[2]);

		TemplateGridActors.LongitudinalLetterActors->AddItem(gridLongLettersActor); 
	}
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::DisplayTemplateGridInTransverseMode()
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->DisplayTemplateGrid(this->TemplateGridActors.TransverseGridActors, this->TemplateGridActors.TransverseLetterActors); 
}


//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::DisplayTemplateGridInLongitudinalMode()
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->DisplayTemplateGrid(this->TemplateGridActors.LongitudinalGridActors , this->TemplateGridActors.LongitudinalLetterActors); 
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::DisplayTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors)
{
	if ( !this->Initialized )
	{
		return; 
	}

	for (int i = 0; i < gridActors->GetNumberOfItems(); i++) 
	{
		this->GetRealtimeRenderer()->AddActor(static_cast<vtkActor*>(gridActors->GetItemAsObject(i)));
	}

	for (int i = 0; i < letterActors->GetNumberOfItems(); i++) 
	{
		this->GetRealtimeRenderer()->AddActor(static_cast<vtkTextActor3D*>(letterActors->GetItemAsObject(i)));
	}

	this->GetRealtimeRenderer()->GetActiveCamera()->DeepCopy(this->GetTemplateCamera());
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::RemoveTemplateGridInTransverseMode()
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->RemoveTemplateGrid(this->TemplateGridActors.TransverseGridActors, this->TemplateGridActors.TransverseLetterActors);
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::RemoveTemplateGridInLongitudinalMode()
{
	if ( !this->Initialized )
	{
		return; 
	}

	this->RemoveTemplateGrid(this->TemplateGridActors.LongitudinalGridActors , this->TemplateGridActors.LongitudinalLetterActors);
}

//----------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::RemoveTemplateGrid(vtkCollection* gridActors, vtkCollection* letterActors)
{
	if ( !this->Initialized )
	{
		return; 
	}

	for (int i = 0; i < gridActors->GetNumberOfItems(); i++) 
	{
		this->GetRealtimeRenderer()->RemoveActor(static_cast<vtkActor*>(gridActors->GetItemAsObject(i)));
	}

	for (int i = 0; i < letterActors->GetNumberOfItems(); i++) 
	{
		this->GetRealtimeRenderer()->RemoveActor(static_cast<vtkTextActor3D*>(letterActors->GetItemAsObject(i)));
	}

	this->GetRealtimeRenderer()->GetActiveCamera()->DeepCopy(this->GetImageCamera());
}

//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::ReadTemplateModelConfiguration(vtkXMLDataElement *configTemplateModel)
{
	if ( configTemplateModel == NULL ) 
	{
		LOG_ERROR("Unable to read template model file: " << this->GetTemplateModelConfigFileName()); 
	}

	vtkXMLDataElement* originFromTemplateHolder = configTemplateModel->FindNestedElementWithName("OriginFromTemplateHolder"); 
	if ( originFromTemplateHolder == NULL ) 
	{
		LOG_ERROR("Unable to read template origin from template holder from template model file!"); 
	}

	vtkSmartPointer<vtkTransform> transformTemplateHolderHomeToTemplateHome = vtkSmartPointer<vtkTransform>::New(); 

	double originX(0); 
	if ( originFromTemplateHolder->GetScalarAttribute("OriginX",originX) )
	{
		transformTemplateHolderHomeToTemplateHome->Translate(originX, 0, 0); 

	}

	double originY(0); 
	if ( originFromTemplateHolder->GetScalarAttribute("OriginY",originY) )
	{
		transformTemplateHolderHomeToTemplateHome->Translate(0, originY, 0); 

	}

	this->GetCalibrationController()->SetTransformTemplateHolderHomeToTemplateHome( transformTemplateHolderHomeToTemplateHome ); 

	// ************************* Template model letters *************************
	vtkXMLDataElement* letterPositions = configTemplateModel->FindNestedElementWithName("LetterPositions");
	if (letterPositions!=NULL)
	{ 

		for (int i = 0; i < letterPositions->GetNumberOfNestedElements(); i++)
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


			TemplateModelLetters.push_back(letterModel); 
		}
	}

	//************************* Template model holes *************************
	
	TEMPLATE_HOLE_REPRESENTATION holeRepresentation = HOLE_SPHERE; 
	TEMPLATE_LETTER_TYPE letterType = LETTER_NUM; 
	double holeRadius=0.2; 

	vtkXMLDataElement* templateHole = configTemplateModel->FindNestedElementWithName("TemplateHole");
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

	vtkXMLDataElement* holePositions = configTemplateModel->FindNestedElementWithName("HolePositions");
	if (holePositions!=NULL)
	{ 
		for (int i = 0; i < holePositions->GetNumberOfNestedElements(); i++)
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
			TemplateModelHoles.push_back(holeModel); 
		}
	}
}


//----------------------------------------------------------------------------
void vtkCalibratorVisualizationComponent::SaveFrameToFile(vtkImageData* frame, char* fileName)
{
	vtkSmartPointer<vtkTIFFWriter> writer = vtkSmartPointer<vtkTIFFWriter>::New(); 
	writer->SetFileName(fileName); 
	writer->SetInput(frame); 
	writer->Update(); 
}