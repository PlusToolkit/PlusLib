#include "UsImageConverterCommon.h"

#include "PlusLogger.h"
#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkImageExport.h"
#include "vtkImageImport.h"

#include "itkFlipImageFilter.h"
#include "itkVTKImageExport.h"

//----------------------------------------------------------------------------
UsImageConverterCommon::UsImageConverterCommon()
{

}

//----------------------------------------------------------------------------
UsImageConverterCommon::~UsImageConverterCommon()
{

}

//----------------------------------------------------------------------------
UsImageConverterCommon::US_IMAGE_ORIENTATION UsImageConverterCommon::GetUsImageOrientationFromString( const char* usImgOrientation )
{
	US_IMAGE_ORIENTATION imageorientation = US_IMG_ORIENT_XX; 
	if ( usImgOrientation == NULL )
	{
		return imageorientation; 
	}
	else if ( STRCASECMP(usImgOrientation, "UF" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_UF; 
	}
	else if ( STRCASECMP(usImgOrientation, "UN" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_UN; 
	}
	else if ( STRCASECMP(usImgOrientation, "MF" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_MF; 
	}
	else if ( STRCASECMP(usImgOrientation, "MN" ) == 0 )
	{
		imageorientation = US_IMG_ORIENT_MN; 
	}

	return imageorientation; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, vtkImageData* outUsOrintedImage )
{
    if ( inUsImage == NULL )
    {
        LOG_ERROR("Failed to convert image data to MF orientation - input image is null!"); 
        return PLUS_FAIL; 
    }

    if ( outUsOrintedImage == NULL )
    {
        LOG_ERROR("Failed to convert image data to MF orientation - output image is null!"); 
        return PLUS_FAIL; 
    }

    if ( inUsImageOrientation == US_IMG_ORIENT_XX ) 
    {
        LOG_WARNING("Failed to convert image data to MF orientation - unknown input image orientation, return identical copy!"); 
        outUsOrintedImage->ShallowCopy( inUsImage ); 
        return PLUS_SUCCESS; 
    }

    if ( inUsImageOrientation == US_IMG_ORIENT_MF )
	{
		outUsOrintedImage->ShallowCopy( inUsImage ); 
        return PLUS_SUCCESS; 
	}

    vtkSmartPointer<vtkImageFlip> imageFlipX = vtkSmartPointer<vtkImageFlip>::New(); 
    imageFlipX->FlipAboutOriginOff();
    imageFlipX->SetFilteredAxis(0); 

    vtkSmartPointer<vtkImageFlip> imageFlipY = vtkSmartPointer<vtkImageFlip>::New(); 
    imageFlipY->FlipAboutOriginOff();
    imageFlipY->SetFilteredAxis(1); 
   
     switch( inUsImageOrientation )
    {
    case US_IMG_ORIENT_UF: 
        {
            imageFlipX->SetInput(inUsImage); 
            imageFlipX->Update(); 
            outUsOrintedImage->ShallowCopy(imageFlipX->GetOutput());
        }
        break; 
    case US_IMG_ORIENT_UN: 
        {
            imageFlipX->SetInput(inUsImage); 
            imageFlipX->Update(); 

            imageFlipY->SetInput(imageFlipX->GetOutput()); 
            imageFlipY->Update(); 

            outUsOrintedImage->ShallowCopy(imageFlipY->GetOutput());
        }
        break; 
    case US_IMG_ORIENT_MN: 
        {
            imageFlipY->SetInput(inUsImage); 
            imageFlipY->Update(); 
            outUsOrintedImage->ShallowCopy(imageFlipY->GetOutput());
        }
        break; 
    }

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( vtkImageData* inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage )
{
    ImageType::Pointer itkimage = ImageType::New(); 
    if ( UsImageConverterCommon::ConvertVtkImageToItkImage(inUsImage, itkimage) != PLUS_SUCCESS )
    {
        LOG_ERROR("Failed to get MF oriented image from vtk image data!"); 
        return PLUS_FAIL;
    }

    return UsImageConverterCommon::GetMFOrientedImage(itkimage, inUsImageOrientation, outUsOrintedImage); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::GetMFOrientedImage( const ImageType::Pointer inUsImage, US_IMAGE_ORIENTATION inUsImageOrientation, ImageType::Pointer& outUsOrintedImage )
{
    if ( inUsImage.IsNull() )
    {
        LOG_ERROR("Failed to convert image data MF orientation - input image is null!"); 
        return PLUS_FAIL; 
    }

    if ( outUsOrintedImage.IsNull() )
    {
        LOG_ERROR("Failed to convert image data MF orientation - output image is null!"); 
        return PLUS_FAIL; 
    }

    if ( inUsImageOrientation == US_IMG_ORIENT_XX ) 
    {
        LOG_WARNING("Failed to convert image data MF orientation - unknown input image orientation, return identical copy!"); 
        outUsOrintedImage = inUsImage; 
        return PLUS_SUCCESS; 
    }

	if ( inUsImageOrientation == US_IMG_ORIENT_MF )
	{
		outUsOrintedImage = inUsImage; 
        return PLUS_SUCCESS; 
	}

	typedef itk::FlipImageFilter <ImageType> FlipImageFilterType;
	FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
	flipFilter->SetInput(inUsImage);
	flipFilter->FlipAboutOriginOff(); 

    itk::FixedArray<bool, 2> flipAxes;
    switch( inUsImageOrientation )
    {
    case US_IMG_ORIENT_UF: 
        {
            flipAxes[0] = true;
            flipAxes[1] = false;
        }
        break; 
    case US_IMG_ORIENT_UN: 
        {
            flipAxes[0] = true;
            flipAxes[1] = true;
        }
        break; 
    case US_IMG_ORIENT_MN: 
        {
            flipAxes[0] = false;
            flipAxes[1] = true;
        }
        break; 
    }

	flipFilter->SetFlipAxes(flipAxes);
	flipFilter->Update();

	outUsOrintedImage = flipFilter->GetOutput(); 

    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::ConvertItkImageToVtkImage(const ImageType::Pointer& inFrame, vtkImageData* outFrame)
{
    if ( inFrame.IsNull() )
    {
        LOG_ERROR("Failed to convert itk image to vtk image - input image is NULL!"); 
        return PLUS_FAIL; 
    }

    if ( outFrame == NULL )
    {
        LOG_ERROR("Failed to convert itk image to vtk image - output image is NULL!"); 
        return PLUS_FAIL; 
    }

    vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
    importer->SetWholeExtent(0, inFrame->GetLargestPossibleRegion().GetSize()[0] - 1, 0, inFrame->GetLargestPossibleRegion().GetSize()[1] - 1, 0, 0);
    importer->SetDataExtentToWholeExtent(); 
    importer->SetDataScalarTypeToUnsignedChar(); 
    importer->SetImportVoidPointer(inFrame->GetBufferPointer()); 
    importer->SetNumberOfScalarComponents(1); 
    importer->Update(); 
 
    // VTK convention is for the image voxel index (0,0,0) to be the lower-left corner of the image, so we need to flip it
    vtkSmartPointer<vtkImageFlip> flipY = vtkSmartPointer<vtkImageFlip>::New(); 
    flipY->SetFilteredAxis(1); 
    flipY->SetInput( importer->GetOutput() ); 
    flipY->Update(); 

    outFrame->DeepCopy( flipY->GetOutput() ); 
    
    return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus UsImageConverterCommon::ConvertVtkImageToItkImage(vtkImageData* inFrame, ImageType::Pointer& outFrame)
{
    LOG_TRACE("UsImageConverterCommon::ConvertVtkImageToItkImage"); 

    if ( inFrame == NULL )
    {
        LOG_ERROR("Failed to convert vtk image to itk image - input image is NULL!"); 
        return PLUS_FAIL; 
    }

    if ( outFrame.IsNull() )
    {
        LOG_ERROR("Failed to convert vtk image to itk image - output image is NULL!"); 
        return PLUS_FAIL; 
    }
    
    // convert vtkImageData to itkImage 
    vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
    imageExport->ImageLowerLeftOn();
    imageExport->SetInput(inFrame); 
    imageExport->Update(); 
    
    int extent[6]= {0}; 
    inFrame->GetExtent(extent); 

    double width = extent[1] - extent[0] + 1; 
    double height = extent[3] - extent[2] + 1; 
    ImageType::SizeType size = { width, height };
    ImageType::IndexType start = {0,0};
    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    outFrame->SetRegions(region);
    try 
    {
        outFrame->Allocate();
    }
    catch(itk::ExceptionObject & err)
    {
        LOG_ERROR("Failed to allocate memory for the image conversion: " << err ); 
        return PLUS_FAIL; 
    }

    imageExport->Export( outFrame->GetBufferPointer() ); 

    return PLUS_SUCCESS; 
}

