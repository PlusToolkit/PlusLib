/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/
template<typename PixelType> PlusStatus PlusVideoFrame::DeepCopyVtkImageToItkImage( vtkImageData* inFrame, typename itk::Image< PixelType, 2 >::Pointer outFrame )
{
  LOG_TRACE("PlusVideoFrame::ConvertVtkImageToItkImage"); 

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

  if( PlusVideoFrame::GetNumberOfBytesPerPixel(inFrame->GetScalarType()) != sizeof(PixelType) )
  {
    LOG_ERROR("Mismatch between input and output pixel types. In: " << PlusVideoFrame::GetStringFromVTKPixelType(inFrame->GetScalarType()));
    return PLUS_FAIL;
  }

  // convert vtkImageData to itkImage 
  vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New(); 
  imageExport->SetInput(inFrame); 
  imageExport->Update(); 

  int extent[6]={0,0,0,0,0,0}; 
  inFrame->GetExtent(extent); 

  double width = extent[1] - extent[0] + 1; 
  double height = extent[3] - extent[2] + 1; 
  itk::Image< PixelType, 2 >::SizeType size;
  size[0] = width;
  size[1] = height;
  itk::Image< PixelType, 2 >::IndexType start;
  start[0]=0;
  start[1]=0;
  itk::Image< PixelType, 2 >::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  outFrame->SetRegions(region);
  try 
  {
    outFrame->Allocate();
  }
  catch(itk::ExceptionObject & err)
  {
    LOG_ERROR("Failed to allocate memory for the image conversion: " << err.GetDescription() ); 
    return PLUS_FAIL; 
  }

  imageExport->Export( outFrame->GetBufferPointer() ); 

  return PLUS_SUCCESS;
}