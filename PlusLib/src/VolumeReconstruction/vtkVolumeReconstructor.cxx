#include "PlusConfigure.h"

#include <limits>

#include "vtkBMPWriter.h"
#include "vtkImageFlip.h" 
#include "vtkImageImport.h" 
#include "vtkImageData.h" 
#include "vtkImageViewer.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkTimerLog.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkTransform.h"
#include "vtkVideoBuffer2.h"
#include "vtkVolumeReconstructor.h"


vtkCxxRevisionMacro(vtkVolumeReconstructor, "$Revisions: 1.0 $");
vtkStandardNewMacro(vtkVolumeReconstructor);

//----------------------------------------------------------------------------
vtkVolumeReconstructor::vtkVolumeReconstructor()
{
	this->Tracker = NULL; 
	this->VideoSource = NULL; 
	this->Reconstructor = NULL; 

	this->ConfigFileName = NULL; 

	this->NumberOfFrames = -1; 

	this->TrackerToolID = 0;

	this->InitializedOff(); 

	vtkSmartPointer<vtkBufferedTracker> tracker = vtkSmartPointer<vtkBufferedTracker>::New();
	this->SetTracker(tracker); 

	vtkSmartPointer<vtkBufferedVideoSource> videoSource = vtkSmartPointer<vtkBufferedVideoSource>::New(); 
	this->SetVideoSource(videoSource); 

	vtkSmartPointer<vtkFreehandUltrasound2Dynamic> reconstructor = vtkSmartPointer<vtkFreehandUltrasound2Dynamic>::New(); 
	this->SetReconstructor(reconstructor); 

	this->GetReconstructor()->SetVideoSource( this->GetVideoSource() ); 
	this->GetReconstructor()->SetTrackerTool( this->GetTracker()->GetTool( this->TrackerToolID ) ); 
	this->GetReconstructor()->TrackerBuffer = this->GetTracker()->GetTool( this->TrackerToolID )->GetBuffer(); 
}

//----------------------------------------------------------------------------
vtkVolumeReconstructor::~vtkVolumeReconstructor()
{
	this->SetReconstructor(NULL); 

	this->SetTracker(NULL); 

	this->SetVideoSource(NULL); 
}

//----------------------------------------------------------------------------
void vtkVolumeReconstructor::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::Initialize()
{
	if ( this->GetNumberOfFrames() > 0 )
	{
		this->GetVideoSource()->SetFrameSize(this->GetFrameSize()); 

		this->GetVideoSource()->GetBuffer()->SetBufferSize( this->GetNumberOfFrames() ); 

		this->GetTracker()->GetTool( this->TrackerToolID )->GetBuffer()->SetBufferSize( this->GetNumberOfFrames() ); 
	}

	this->GetVideoSource()->Initialize(); 

	// Min and max values of double indicate intial values of max and min.

	this->SetVolumeExtentMax( - std::numeric_limits< double >::max(),
		- std::numeric_limits< double >::max(),
		- std::numeric_limits< double >::max() ); 
	this->SetVolumeExtentMin( std::numeric_limits< double >::max(),
		std::numeric_limits< double >::max(),
		std::numeric_limits< double >::max() ); 


	this->InitializedOn(); 
}

//----------------------------------------------------------------------------
void vtkVolumeReconstructor::StartReconstruction()
{

	if ( !this->GetInitialized() ) 
	{
		LOG_ERROR( "Unable to start reconstruction: First need to initialize!"); 
		exit(EXIT_FAILURE); 
	}

	this->Reconstructor->StartReconstruction( this->GetNumberOfFrames() ) ;
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::FillHoles()
{
	this->GetReconstructor()->FillHolesInOutput(); 
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::AddTrackedFrame( unsigned char* imageData, 
											 const int imageWidthInPixels, 
											 const int imageHeightInPixels, 
											 const double transformMatrix[16] )
{
	vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
	vtkMatrix->DeepCopy(transformMatrix);

	this->AddTrackedFrame(imageData, imageWidthInPixels, imageHeightInPixels, vtkMatrix); 

}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::AddTrackedFrame( unsigned char* imageData, 
											 const int imageWidthInPixels, 
											 const int imageHeightInPixels, 
											 vtkMatrix4x4* transformMatrix )
{
	vtkSmartPointer< vtkImageImport > importer = vtkSmartPointer<vtkImageImport>::New();
	importer->SetWholeExtent(0,imageWidthInPixels - 1,0,imageHeightInPixels - 1,0,0);
	importer->SetDataExtentToWholeExtent();
	importer->SetDataScalarTypeToUnsignedChar();
	importer->SetImportVoidPointer(imageData);
	importer->SetNumberOfScalarComponents(1); 
	importer->Update();

	vtkSmartPointer< vtkImageFlip > imageFlip = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlip->SetInput( importer->GetOutput() ); 
	imageFlip->SetFilteredAxis(1); 
	imageFlip->Update(); 

	this->AddTrackedFrame( imageFlip->GetOutput(), transformMatrix ); 
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::AddTrackedFrame( vtkImageData* frame, vtkMatrix4x4* mToolToReference )
{
	if ( !this->GetInitialized() ) 
	{
		LOG_WARNING( "Unable to add tracked frame to the volume: First need to initalize!" ); 
		return; 
	}


	vtkSmartPointer< vtkTransform > tToolToReference = vtkSmartPointer< vtkTransform >::New();
	tToolToReference->SetMatrix( mToolToReference );
	tToolToReference->Update();


	double timestamp = vtkAccurateTimer::GetSystemTime(); 
	this->GetVideoSource()->AddFrame( frame, timestamp ); 
	this->GetTracker()->AddTransform( tToolToReference->GetMatrix(), timestamp ); 

	this->FindOutputExtent( tToolToReference->GetMatrix(), frame->GetExtent() ); 
}



void vtkVolumeReconstructor::FindOutputExtent( vtkMatrix4x4* mToolToReference, int* frameExtent )
{
	vtkSmartPointer< vtkTransform > tImageToTool = this->GetImageToToolTransform();

	// Prepare the full ImageToReference transform.
	// Output volume is in the Reference coordinate system.

	vtkSmartPointer< vtkTransform > tToolToReference = vtkSmartPointer< vtkTransform >::New();
	tToolToReference->PostMultiply();
	tToolToReference->SetMatrix( mToolToReference );
	tToolToReference->Update();


	vtkSmartPointer< vtkTransform > tImageToReference = vtkSmartPointer< vtkTransform >::New();
	tImageToReference->PostMultiply();
	tImageToReference->Identity();
	tImageToReference->Concatenate( tImageToTool );
	tImageToReference->Concatenate( tToolToReference );
	tImageToReference->Update();


	// Prepare the four corner points of the input US image.

	std::vector< double* > cornersImage;
	double c0[ 4 ] = { frameExtent[ 0 ], frameExtent[ 2 ], 0,  1 };
	double c1[ 4 ] = { frameExtent[ 0 ], frameExtent[ 3 ], 0,  1 };
	double c2[ 4 ] = { frameExtent[ 1 ], frameExtent[ 2 ], 0,  1 };
	double c3[ 4 ] = { frameExtent[ 1 ], frameExtent[ 3 ], 0,  1 };
	cornersImage.push_back( c0 );
	cornersImage.push_back( c1 );
	cornersImage.push_back( c2 );
	cornersImage.push_back( c3 );


	// Transform the corners to Reference. Check for MIN MAX update.

	for ( unsigned int i = 0; i < cornersImage.size(); ++ i )
	{
		double cRef[ 4 ] = { 0, 0, 0, 1 };
		tImageToReference->MultiplyPoint( cornersImage[ i ], cRef );

		for ( int ii = 0; ii < 3; ii ++ )
		{
			if ( cRef[ ii ] > this->VolumeExtentMax[ ii ] ) this->VolumeExtentMax[ ii ] = cRef[ ii ];
			if ( cRef[ ii ] < this->VolumeExtentMin[ ii ] ) this->VolumeExtentMin[ ii ] = cRef[ ii ];
		}

	}

	// Set the output extent from the current min and max values.

	int outputExtent[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	double* outputSpacing = this->GetReconstructor()->GetOutputSpacing();
	outputExtent[ 1 ] = int( ( this->VolumeExtentMax[ 0 ] - this->VolumeExtentMin[ 0 ] ) / outputSpacing[ 0 ] );
	outputExtent[ 3 ] = int( ( this->VolumeExtentMax[ 1 ] - this->VolumeExtentMin[ 1 ] ) / outputSpacing[ 1 ] );
	outputExtent[ 5 ] = int( ( this->VolumeExtentMax[ 2 ] - this->VolumeExtentMin[ 2 ] ) / outputSpacing[ 2 ] );

	this->Reconstructor->SetOutputExtent( outputExtent );

	// Set the output origin from the current min and max values

	this->Reconstructor->SetOutputOrigin( this->VolumeExtentMin ); 
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::ReadConfiguration( const char* configFileName )
{
	this->SetConfigFileName( configFileName ); 
	this->ReadConfiguration(); 
}

//----------------------------------------------------------------------------
void vtkVolumeReconstructor::ReadConfiguration()
{
	if ( this->GetConfigFileName() == NULL ) 
	{
		LOG_ERROR( "You need to specify the configuration file name!" ); 
	}

	this->GetReconstructor()->ReadSummaryFile(this->GetConfigFileName()); 
}

//----------------------------------------------------------------------------
vtkSmartPointer< vtkTransform > vtkVolumeReconstructor::GetImageToToolTransform()
{ 
	vtkSmartPointer< vtkTransform > tImageToTool = vtkSmartPointer< vtkTransform >::New();
	tImageToTool->PostMultiply();
	tImageToTool->Identity();
	tImageToTool->SetMatrix( this->GetReconstructor()->GetTrackerTool()->GetCalibrationMatrix() );
	tImageToTool->Update();

	return tImageToTool;
}

//----------------------------------------------------------------------------
vtkSmartPointer< vtkTransform > vtkVolumeReconstructor::GetImageToReferenceTransform( int slice )
{
	vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
	vtkTrackerBuffer* buffer = this->GetTracker()->GetTool( this->TrackerToolID )->GetBuffer();
	buffer->GetMatrix( mToolToReference, buffer->GetNumberOfItems() - 1 - slice );

	vtkSmartPointer< vtkTransform > tToolToReference = vtkSmartPointer< vtkTransform >::New();
	tToolToReference->PostMultiply();
	tToolToReference->Identity();
	tToolToReference->SetMatrix( mToolToReference );
	tToolToReference->Update();

	vtkSmartPointer< vtkTransform > tImageToReference = vtkSmartPointer< vtkTransform >::New();
	tImageToReference->PostMultiply();
	tImageToReference->Identity();
	tImageToReference->Concatenate( this->GetImageToToolTransform() );
	tImageToReference->Concatenate( tToolToReference );
	tImageToReference->Update();

	return tImageToReference;
}
