#include "PlusConfigure.h"

#include <limits>

#include "vtkBMPWriter.h"
#include "vtkImageImport.h" 
#include "vtkImageData.h" 
#include "vtkImageViewer.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkTimerLog.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkTransform.h"
#include "vtkVideoBuffer.h"
#include "vtkVolumeReconstructor.h"
#include "vtkXMLUtilities.h"


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

  this->SetNumberOfBitsPerPixel(0); 

  this->SetFrameSize(0, 0); 

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
    this->GetVideoSource()->GetBuffer()->SetNumberOfBitsPerPixel( this->GetNumberOfBitsPerPixel() ); 

    if ( this->GetVideoSource()->GetBuffer()->SetBufferSize( this->GetNumberOfFrames() ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to set video buffer size!"); 
      return; 
    }

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
PlusStatus vtkVolumeReconstructor::StartReconstruction()
{

  if ( !this->GetInitialized() ) 
  {
    LOG_ERROR( "Unable to start reconstruction: First need to initialize!"); 
    return PLUS_FAIL; 
  }

  return this->Reconstructor->StartReconstruction( this->GetNumberOfFrames() ) ;
}


//----------------------------------------------------------------------------
void vtkVolumeReconstructor::FillHoles()
{
  this->GetReconstructor()->FillHolesInOutput(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVolumeReconstructor::AddTrackedFrame( ImageType::Pointer frame, US_IMAGE_ORIENTATION usImageOrientation, vtkMatrix4x4* mToolToReference, double timestamp )
{
  if ( !this->GetInitialized() ) 
  {
    LOG_WARNING( "Unable to add tracked frame to the volume: First need to initalize!" ); 
    return PLUS_FAIL; 
  }

  PlusStatus  videoStatus = this->GetVideoSource()->AddFrame( frame, usImageOrientation, timestamp ); 
  PlusStatus trackerStatus = this->GetTracker()->AddTransform( mToolToReference, timestamp ); 

  int extent[6] = {0, frame->GetLargestPossibleRegion().GetSize()[0] - 1, 0, frame->GetLargestPossibleRegion().GetSize()[1] - 1, 0, 0 }; 

  if ( videoStatus == PLUS_SUCCESS && trackerStatus == PLUS_SUCCESS )
  {
    this->FindOutputExtent( mToolToReference, extent ); 
  }
  else
  {
    LOG_ERROR("Failed to add tracked frame to buffer!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
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
PlusStatus vtkVolumeReconstructor::ReadConfiguration( const char* configFileName )
{
  this->SetConfigFileName( configFileName ); 
  return this->ReadConfiguration(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVolumeReconstructor::ReadConfiguration()
{
  if ( this->GetConfigFileName() == NULL ) 
  {
    LOG_ERROR( "You need to specify the configuration file name!" ); 
    return PLUS_FAIL;
  }

  // read in the freehand information
  vtkSmartPointer<vtkXMLDataElement> rootElement = vtkXMLUtilities::ReadElementFromFile(this->GetConfigFileName());
  if (rootElement == NULL)
  {
    LOG_ERROR("Read volume reconstruction configuration - invalid file " << this->GetConfigFileName());
    return PLUS_FAIL;
  }

  return this->GetReconstructor()->ReadSummary(rootElement); 
}

//----------------------------------------------------------------------------
PlusStatus vtkVolumeReconstructor::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  return this->GetReconstructor()->ReadSummary(aConfig); 
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

const vtkMatrix4x4* vtkVolumeReconstructor::GetImageToToolMatrix()
{
  vtkMatrix4x4* matrix = this->GetReconstructor()->GetTrackerTool()->GetCalibrationMatrix();
  const vtkMatrix4x4* constMatrix = const_cast< const vtkMatrix4x4* >( matrix );
  
  return constMatrix;
}

