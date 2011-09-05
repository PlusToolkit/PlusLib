#include "vtkTrackedFrameList.h" 
#include "PlusMath.h"

#include <math.h>
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkXMLUtilities.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include "itkFixedArray.h"
#include "itkCastImageFilter.h"


//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame()
{
  this->Status = TR_OK; 
  this->Timestamp = 0; 
  this->FrameSize[0] = 0; 
  this->FrameSize[1] = 0; 
}

//----------------------------------------------------------------------------
TrackedFrame::~TrackedFrame()
{
}

//----------------------------------------------------------------------------
TrackedFrame::TrackedFrame(const TrackedFrame& frame)
{
  this->Status = TR_OK; 
  this->Timestamp = 0; 
  this->FrameSize[0] = 0; 
  this->FrameSize[1] = 0; 

  *this = frame; 
}

//----------------------------------------------------------------------------
TrackedFrame& TrackedFrame::operator=(TrackedFrame const&trackedFrame)
{
  // Handle self-assignment
  if (this == &trackedFrame)
  {
    return *this;
  }

  this->DefaultFrameTransformName = trackedFrame.DefaultFrameTransformName; 
  this->CustomFrameFieldList = trackedFrame.CustomFrameFieldList; 
  this->CustomFieldList = trackedFrame.CustomFieldList; 
  this->ImageData = trackedFrame.ImageData; 
  this->Timestamp = trackedFrame.Timestamp;
  this->Status = trackedFrame.Status; 
  this->FrameSize[0] = trackedFrame.FrameSize[0]; 
  this->FrameSize[1] = trackedFrame.FrameSize[1]; 

  return *this;
}

//----------------------------------------------------------------------------
int* TrackedFrame::GetFrameSize()
{
  this->ImageData.GetFrameSize(this->FrameSize);
  return this->FrameSize; 
}


//----------------------------------------------------------------------------
int TrackedFrame::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerPixel(0); 
  numberOfBitsPerPixel = this->ImageData.GetNumberOfBytesPerPixel()*8;
  return numberOfBitsPerPixel; 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameField( std::string name, std::string value )
{
  CustomFrameFieldPair pair(name, value); 
  this->CustomFrameFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetCustomFrameField( std::string name )
{
  std::vector<CustomFrameFieldPair>::iterator customFrameValue; 
  for ( customFrameValue = this->CustomFrameFieldList.begin(); customFrameValue != this->CustomFrameFieldList.end(); customFrameValue++ )
  {
    if ( STRCASECMP(customFrameValue->first.c_str(), name.c_str()) == 0 ) 
    { 
      return customFrameValue->second.c_str(); 
    }
  }

  return NULL; 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomField( std::string name, std::string value )
{
  CustomFieldPair pair(name, value); 
  this->CustomFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
const char* TrackedFrame::GetCustomField( std::string name )
{
  std::vector<CustomFieldPair>::iterator customValue; 
  for ( customValue = this->CustomFieldList.begin(); customValue != this->CustomFieldList.end(); customValue++ )
  {
    if ( STRCASECMP(customValue->first.c_str(), name.c_str()) == 0 )
    { 
      return customValue->second.c_str(); 
    }
  }

  return NULL; 
}



/**
 * @param transform 16 elements of the transform matrix.
 */
bool TrackedFrame::GetDefaultFrameTransform(double transform[16]) 
{
  // Find default frame transform 
  std::vector<CustomFrameFieldPair>::iterator defaultFrameTransform; 
  for ( defaultFrameTransform = this->CustomFrameFieldList.begin(); defaultFrameTransform != this->CustomFrameFieldList.end(); defaultFrameTransform++ )
  {
    if ( STRCASECMP(defaultFrameTransform->first.c_str(), this->DefaultFrameTransformName.c_str()) == 0) 
    {
      std::istringstream transformFieldValue(defaultFrameTransform->second); 
      double item; 
      int i = 0; 
      while ( transformFieldValue >> item )
      {
        if ( i < 16 )
          transform[i++] = item; 
      }
      return true;
    }
  }

  LOG_ERROR("Unable to find default transform in tracked frame!"); 
  return false; 
}

//----------------------------------------------------------------------------
bool TrackedFrame::GetCustomFrameTransform(const char* frameTransformName, double transform[16]) 
{
  if (frameTransformName == NULL )
  {
    LOG_ERROR("Unable to get custom frame transform: frame transform name is NULL!"); 
    return false; 
  }

  // Find default frame transform 
  std::vector<CustomFrameFieldPair>::iterator customFrameTransform; 
  for ( customFrameTransform = this->CustomFrameFieldList.begin(); customFrameTransform != this->CustomFrameFieldList.end(); customFrameTransform++ )
  {
    if ( STRCASECMP(customFrameTransform->first.c_str(), frameTransformName) == 0)
    {
      std::istringstream transformFieldValue(customFrameTransform->second); 
      double item; 
      int i = 0; 
      while ( transformFieldValue >> item )
      {
        if ( i < 16 )
          transform[i++] = item; 
      }
      return true;
    }
  }

  LOG_ERROR("Unable to find custom transform (" << frameTransformName << ") custom frame field list!"); 
  return false; 
}


//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameTransform(std::string frameTransformName, double transform[16]) 
{
  std::ostringstream strTransform; 
  for ( int i = 0; i < 16; ++i )
  {
    strTransform << transform[ i ] << " ";
  }

  CustomFrameFieldPair pair(frameTransformName, strTransform.str()); 
  this->CustomFrameFieldList.push_back(pair); 
}

//----------------------------------------------------------------------------
void TrackedFrame::SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform) 
{
  double dTransform[ 16 ];
  vtkMatrix4x4::DeepCopy( dTransform, transform );
  this->SetCustomFrameTransform(frameTransformName, dTransform); 
}

//----------------------------------------------------------------------------
TrackerStatus TrackedFrame::GetStatus() 
{
  return this->Status;
}


//----------------------------------------------------------------------------
void TrackedFrame::SetStatus(TrackerStatus status)
{
  this->Status=status;
}

//----------------------------------------------------------------------------
TrackerStatus TrackedFrame::GetStatusFromString(const char* statusStr) 
{
  TrackerStatus status = TR_OK;
  std::string strFlag(statusStr); 
  if ( strFlag.find("OK") != std::string::npos )
  {
    status = TR_OK;
  }
  else if ( strFlag.find("TR_MISSING") != std::string::npos )
  {
    status = TR_MISSING;
  }
  else if ( strFlag.find("TR_OUT_OF_VIEW") != std::string::npos )
  {
    status = TR_OUT_OF_VIEW;
  }
  else if ( strFlag.find("TR_OUT_OF_VOLUME") != std::string::npos )
  {
    status = TR_OUT_OF_VOLUME;
  }
  else if ( strFlag.find("TR_REQ_TIMEOUT") != std::string::npos )
  {
    status = TR_REQ_TIMEOUT;
  }
  return status;
}

//----------------------------------------------------------------------------
PlusStatus TrackedFrame::WriteToFile(std::string &filename, vtkMatrix4x4* mImageToTracker)
{
  typedef unsigned char			PixelType; 
  typedef itk::Image< PixelType, 2 > Image2dType; 
  typedef itk::Image< PixelType, 3 > Image3dType; 

  typedef itk::CastImageFilter< Image2dType, Image3dType> CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New(); 
  castFilter->SetInput(this->ImageData.GetDisplayableImage());
  castFilter->Update();
  Image3dType::Pointer image=castFilter->GetOutput();     

  double origin[3]=
  {
    mImageToTracker->Element[0][3],
    mImageToTracker->Element[1][3],
    mImageToTracker->Element[2][3],
  };
  image->SetOrigin(origin);

  Image3dType::DirectionType dir;
  for (int r=0; r<3; r++)
  {
    for (int c=0; c<3; c++)
    {
      dir.GetVnlMatrix().put(r,c, mImageToTracker->Element[r][c]);
    }
  }
  image->SetDirection(dir);

  typedef itk::ImageFileWriter< Image3dType > WriterType; 
  WriterType::Pointer writeImage = WriterType::New();  

  writeImage->SetFileName(filename.c_str());  
  writeImage->SetInput( image );
  try
  {
    writeImage->Update(); 
  }
  catch (itk::ExceptionObject & err) 
  {		
    LOG_ERROR(" Exception! writer did not update. Error: "<< err); 
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}



//----------------------------------------------------------------------------
// ************************* vtkTrackedFrameList *****************************
//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTrackedFrameList, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTrackedFrameList); 

//----------------------------------------------------------------------------
vtkTrackedFrameList::vtkTrackedFrameList()
{
  this->SetMaxNumOfFramesToWrite(500); 
  this->SetNumberOfUniqueFrames(5); 
  this->SetFrameSize(0,0); 

  this->MinRequiredTranslationDifferenceMm=0.0;
  this->MinRequiredAngleDifferenceDeg=0.0;
  this->MaxAllowedTranslationSpeedMmPerSec=0.0;
  this->MaxAllowedRotationSpeedDegPerSec=0.0;

}

//----------------------------------------------------------------------------
vtkTrackedFrameList::~vtkTrackedFrameList()
{
  this->Clear(); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::Clear()
{
  for ( int i = 0; i < this->TrackedFrameList.size(); i++ )
  {
    if (this->TrackedFrameList[i] != NULL )
    {
      delete this->TrackedFrameList[i]; 
      this->TrackedFrameList[i] = NULL; 
    }
  }
  this->TrackedFrameList.clear(); 
}

//----------------------------------------------------------------------------
void vtkTrackedFrameList::PrintSelf(std::ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
TrackedFrame* vtkTrackedFrameList::GetTrackedFrame(int frameNumber)
{
  if ( frameNumber >= this->GetNumberOfTrackedFrames() )
  {
    LOG_ERROR("vtkTrackedFrameList::GetTrackedFrame requested a non-existing frame (framenumber="<<frameNumber);
    return NULL; 
  }
  return this->TrackedFrameList[frameNumber];   
}

//----------------------------------------------------------------------------
int vtkTrackedFrameList::AddTrackedFrame(TrackedFrame *trackedFrame)
{
  TrackedFrame* pTrackedFrame = new TrackedFrame(*trackedFrame); 
  this->TrackedFrameList.push_back(pTrackedFrame); 
  return (this->TrackedFrameList.size() - 1); 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateData(TrackedFrame* trackedFrame, bool validateTimestamp/*=true*/, bool validateStatus/*=true*/, 
                                       bool validatePosition/*=true*/, const char* frameTransformName /*= NULL*/, bool validateSpeed/*=true*/)
{
  if ( validateTimestamp )
  {
    if (! this->ValidateTimestamp(trackedFrame))
    {
      return false;
    }
  }

  if ( validateStatus )
  {
    if (! this->ValidateStatus(trackedFrame))
    {
      return false;
    }
  }

  if ( validatePosition )
  {
    if (! this->ValidatePosition(trackedFrame, frameTransformName))
    {
      return false;
    }
  }

  if ( validateSpeed )
  {
    if (! this->ValidateSpeed(trackedFrame))
    {
      return false;
    }
  }

  return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateTimestamp(TrackedFrame* trackedFrame)
{
  if ( this->TrackedFrameList.size() > 0 )
  {
    const bool isTimestampUnique = std::find_if(this->TrackedFrameList.begin(), this->TrackedFrameList.end(), TrackedFrameTimestampFinder(trackedFrame) ) == this->TrackedFrameList.end(); 

    if ( !isTimestampUnique )
    {
      LOG_DEBUG("Tracked frame timestamp validation result: we've already inserted this frame to container!"); 
      return false; 
    }

    return true; 
  }

  return true; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidatePosition(TrackedFrame* trackedFrame, const char* frameTransformName)
{
  TrackedFrameListType::iterator searchIndex; 
  const int containerSize = this->TrackedFrameList.size(); 
  if (containerSize < this->NumberOfUniqueFrames )
  {
    searchIndex = this->TrackedFrameList.begin(); 
  }
  else
  {
    searchIndex =this->TrackedFrameList.end() - this->NumberOfUniqueFrames; 
  }

  if (std::find_if(searchIndex, this->TrackedFrameList.end(), TrackedFramePositionFinder(trackedFrame, frameTransformName,
    this->MinRequiredTranslationDifferenceMm, this->MinRequiredAngleDifferenceDeg) ) != this->TrackedFrameList.end() )
  {
    // We've already inserted this frame 
    LOG_DEBUG("Tracked frame position validation result: we've already inserted this frame to container!"); 
    return false; 
  }

  return true; 	
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateStatus(TrackedFrame* trackedFrame)
{
  const bool isStatusValid = (trackedFrame->Status == 0); 
  if ( !isStatusValid )
  {
    LOG_DEBUG("Tracked frame status validation result: tracked frame status invalid!"); 
  }

  return isStatusValid; 
}

//----------------------------------------------------------------------------
bool vtkTrackedFrameList::ValidateSpeed(TrackedFrame* trackedFrame)
{
  if ( this->TrackedFrameList.size() < 1 ) {
    return true;
  }

  TrackedFrameListType::iterator latestFrameInList = this->TrackedFrameList.end() - 1;

  // Compute difference between the last two timestamps
  double diffTimeSec = fabs( trackedFrame->Timestamp - (*latestFrameInList)->Timestamp );
  if (diffTimeSec < 0.0001) 
  { 
    // the frames are almost acquired at the same time, cannot compute speed reliably
    // better to invalidate the frame
    return false;
  }

  // Get default frame transform of the input frame and the latest frame in the list
  vtkSmartPointer<vtkTransform> inputTransform = vtkSmartPointer<vtkTransform>::New(); 
  double inputTransformVector[16]={0}; 
  if ( trackedFrame->GetDefaultFrameTransform(inputTransformVector) )
  {
    inputTransform->SetMatrix(inputTransformVector); 
  }
  else
  {
    LOG_ERROR("Unable to get default frame transform for input frame!");
    return false;
  }

  vtkSmartPointer<vtkTransform> latestTransform = vtkSmartPointer<vtkTransform>::New(); 
  double latestTransformVector[16]={0}; 
  if ( (*latestFrameInList)->GetDefaultFrameTransform(latestTransformVector) )
  {
    latestTransform->SetMatrix(latestTransformVector); 
  }
  else
  {
    LOG_ERROR("Unable to get default frame transform for latest frame!");
    return false;
  }
 
  if ( this->MaxAllowedTranslationSpeedMmPerSec>0)
  {
    // Compute difference between the last two positions
    double diffPosition = PlusMath::GetPositionDifference( inputTransform->GetMatrix(), latestTransform->GetMatrix() );
    double velocityPositionMmPerSec = fabs( diffPosition / diffTimeSec );
    if (velocityPositionMmPerSec > this->MaxAllowedTranslationSpeedMmPerSec)
    {
      LOG_DEBUG("Tracked frame speed validation result: tracked frame position change too fast (VelocityPosition = " << velocityPositionMmPerSec << ">" << this->MaxAllowedTranslationSpeedMmPerSec << " mm/sec)"); 
      return false; 
    }
  }

  if ( this->MaxAllowedRotationSpeedDegPerSec>0)
  {
    // Compute difference between the last two orientations
    double diffOrientationDeg = PlusMath::GetOrientationDifference( inputTransform->GetMatrix(), latestTransform->GetMatrix() );
    double velocityOrientationDegPerSec = fabs( diffOrientationDeg / diffTimeSec );
    if (velocityOrientationDegPerSec > this->MaxAllowedRotationSpeedDegPerSec )
    {
      LOG_DEBUG("Tracked frame speed validation result: tracked frame angle change too fast VelocityOrientation = " << velocityOrientationDegPerSec << ">" << this->MaxAllowedRotationSpeedDegPerSec << " deg/sec)"); 
      return false; 
    }
  }

  return true; 
}

//----------------------------------------------------------------------------
int* vtkTrackedFrameList::GetFrameSize()
{
  if ( this->GetNumberOfTrackedFrames() > 0 )
  {
    this->SetFrameSize(this->GetTrackedFrame(0)->GetFrameSize()); 
  }
  else
  {
    LOG_WARNING("Unable to get frame size: there is no frame in the tracked frame list!"); 
  }

  return this->FrameSize; 
}

//----------------------------------------------------------------------------
int vtkTrackedFrameList::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerPixel = 0; 
  if ( this->GetNumberOfTrackedFrames() > 0 )
  {
    numberOfBitsPerPixel = this->GetTrackedFrame(0)->GetNumberOfBitsPerPixel(); 
  }
  else
  {
    LOG_WARNING("Unable to get frame size: there is no frame in the tracked frame list!"); 
  }

  return numberOfBitsPerPixel; 
}

//----------------------------------------------------------------------------
std::string vtkTrackedFrameList::GetDefaultFrameTransformName()
{
  std::string defaultFrameTransformName; 
  if ( !TrackedFrameList.empty() )
  {
    defaultFrameTransformName = this->GetTrackedFrame(0)->DefaultFrameTransformName; 
  }

  return defaultFrameTransformName; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::ReadFromSequenceMetafile(const char* trackedSequenceDataFileName)
{
  itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New();
  if (ReadFromSequenceMetafileGeneric<unsigned char>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<char>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<unsigned short>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<short>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<unsigned int>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<int>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<unsigned long>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<long>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<float>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else if (ReadFromSequenceMetafileGeneric<double>(trackedSequenceDataFileName)==PLUS_SUCCESS) {}
  else
  {
    LOG_ERROR("Unsupported pixel type: " << TrackedFrameList[0]->ImageData.GetITKScalarPixelType());
    return PLUS_FAIL;
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
template <class OutputPixelType>
PlusStatus vtkTrackedFrameList::ReadFromSequenceMetafileGeneric(const char* trackedSequenceDataFileName)
{
  typedef itk::Image< OutputPixelType, 3 > ImageSequenceType;
  typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;
  itk::MetaImageSequenceIO::Pointer readerMetaImageSequenceIO = itk::MetaImageSequenceIO::New(); 
  ImageSequenceReaderType::Pointer reader = ImageSequenceReaderType::New(); 

  // Set the image IO 
  reader->SetImageIO(readerMetaImageSequenceIO); 
  reader->SetFileName(trackedSequenceDataFileName);

  try
  {
    reader->Update(); 
  }
  catch (itk::ExceptionObject & err) 
  {		
    LOG_ERROR(" Sequence image reader couldn't update: " <<  err.GetDescription() ); 
    return PLUS_FAIL;
  }	

  ImageSequenceType::Pointer imageSeq = reader->GetOutput();

  US_IMAGE_ORIENTATION usImageOrientation = UsImageConverterCommon::GetUsImageOrientationFromString(readerMetaImageSequenceIO->GetUltrasoundImageOrientation()); 

  const int frameSizeInPx[2] = {imageSeq->GetLargestPossibleRegion().GetSize()[0], imageSeq->GetLargestPossibleRegion().GetSize()[1]};  
  const unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	  
  const int pixelSizeInBytes = sizeof(ImageSequenceType::PixelType); 
  unsigned int frameSizeInBytes=frameSizeInPx[0]*frameSizeInPx[1]*pixelSizeInBytes;
  unsigned char* imageSeqData = reinterpret_cast<unsigned char*>(imageSeq->GetBufferPointer()); // pointer to the image pixel buffer

  for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
  {
    TrackedFrame trackedFrame; 
    unsigned char* currentFrameImageData = imageSeqData + imgNumber * frameSizeInBytes;

    // Get Default transform name 
    std::string defaultFrameTransformName = readerMetaImageSequenceIO->GetDefaultFrameTransformName(); 
    trackedFrame.DefaultFrameTransformName = defaultFrameTransformName; 

    // Get custom fields 
    std::vector<std::string> customFieldNames = readerMetaImageSequenceIO->GetCustomFieldNames(); 
    for ( int i = 0; i < customFieldNames.size(); i++ )
    {
      TrackedFrame::CustomFieldPair field; 
      field.first = customFieldNames[i]; 
      field.second = readerMetaImageSequenceIO->GetCustomString(customFieldNames[i].c_str()); 
      trackedFrame.CustomFieldList.push_back(field); 
    }

    // Get custom frame fields 
    std::vector<std::string> customFrameFieldNames = readerMetaImageSequenceIO->GetCustomFrameFieldNames(); 
    for ( int i = 0; i < customFrameFieldNames.size(); i++ )
    {
      TrackedFrame::CustomFrameFieldPair field; 
      field.first = customFrameFieldNames[i]; 
      field.second = readerMetaImageSequenceIO->GetCustomFrameString(imgNumber,customFrameFieldNames[i].c_str()); 
      trackedFrame.CustomFrameFieldList.push_back(field); 
    }

    itk::Image<OutputPixelType, 2>::Pointer itkImage = itk::Image<OutputPixelType, 2>::New(); 
    if ( UsImageConverterCommon::GetMFOrientedImage(currentFrameImageData, usImageOrientation, frameSizeInPx, pixelSizeInBytes*8, itkImage) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get MF oriented image from sequence metafile (frame number: " << imgNumber << ")!"); 
      continue; 
    }
    trackedFrame.ImageData.SetITKImageBase(itkImage);

    const char* cFlag = trackedFrame.GetCustomFrameField("Status"); 
    TrackerStatus status = TR_OK;
    if ( cFlag != NULL )
    {
      status=TrackedFrame::GetStatusFromString(cFlag);
    }
    else
    {
      LOG_TRACE("Unable to read Status field of image #" << imgNumber); 
    }
    trackedFrame.SetStatus(status);


    this->AddTrackedFrame(&trackedFrame); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/)
{
  switch (TrackedFrameList[0]->ImageData.GetITKScalarPixelType())
  {
  case itk::ImageIOBase::UCHAR: return SaveToSequenceMetafileGeneric<unsigned char>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::CHAR: return SaveToSequenceMetafileGeneric<char>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::USHORT: return SaveToSequenceMetafileGeneric<unsigned short>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::SHORT: return SaveToSequenceMetafileGeneric<short>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::UINT: return SaveToSequenceMetafileGeneric<unsigned int>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::INT: return SaveToSequenceMetafileGeneric<int>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::ULONG: return SaveToSequenceMetafileGeneric<unsigned long>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::LONG: return SaveToSequenceMetafileGeneric<long>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::FLOAT: return SaveToSequenceMetafileGeneric<float>(outputFolder, sequenceDataFileName, extension , useCompression);
  case itk::ImageIOBase::DOUBLE: return SaveToSequenceMetafileGeneric<double>(outputFolder, sequenceDataFileName, extension , useCompression);
  default:
    LOG_ERROR("Unsupported pixel type: " << TrackedFrameList[0]->ImageData.GetITKScalarPixelType());
    return PLUS_FAIL;
  }
}

//----------------------------------------------------------------------------
template <class OutputPixelType>
PlusStatus vtkTrackedFrameList::SaveToSequenceMetafileGeneric(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/)
{
  LOG_TRACE("vtkTrackedFrameList::SaveToSequenceMetafile - outputFolder: " << outputFolder << "  sequenceDataFileName: " << sequenceDataFileName); 

  if ( TrackedFrameList.empty() )
  {
    LOG_ERROR("Unable to save tracked frame list to sequence metafile - tracked frame list empty!"); 
    return PLUS_FAIL; 
  }

  if ( !vtksys::SystemTools::FileExists(outputFolder, false) )
  {
    vtksys::SystemTools::MakeDirectory(outputFolder); 
    LOG_DEBUG("Created new folder: " << outputFolder ); 
  }

  const int numberOfFilesToWrite = ceil( (1.0 * TrackedFrameList.size()) / (1.0 * this->MaxNumOfFramesToWrite) ); 

  typedef itk::Image< OutputPixelType, 3 > ImageSequenceType;
  typedef itk::ImageFileWriter< ImageSequenceType > ImageSequenceWriterType;

  for ( int fileNumber = 1; fileNumber <= numberOfFilesToWrite; fileNumber++ )
  {

    ImageSequenceType::Pointer imageDataSequence = ImageSequenceType::New();

    int numberOfFrames(0); 
    if ( TrackedFrameList.size() - (fileNumber - 1) * this->MaxNumOfFramesToWrite > this->MaxNumOfFramesToWrite )
    {
      numberOfFrames = this->MaxNumOfFramesToWrite; 
    }
    else
    {
      numberOfFrames = TrackedFrameList.size() - (fileNumber - 1) * this->MaxNumOfFramesToWrite; 
    }

    int frameSize[2];
    TrackedFrameList[0]->ImageData.GetFrameSize(frameSize);

    ImageSequenceType::SizeType size = {frameSize[0], frameSize[1], numberOfFrames };
    ImageSequenceType::IndexType start = {0,0,0};
    ImageSequenceType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imageDataSequence->SetRegions(region);

    try
    {
      imageDataSequence->Allocate();
    }
    catch (itk::ExceptionObject & err) 
    {		
      LOG_ERROR("Unable to allocate memory for image sequence : " << err.GetDescription() );
      return PLUS_FAIL; 
    }	

    itk::MetaImageSequenceIO::Pointer writerMetaImageSequenceIO = itk::MetaImageSequenceIO::New();

    unsigned char* imageData = reinterpret_cast<unsigned char*>(imageDataSequence->GetBufferPointer()); // pointer to the image pixel buffer

    unsigned int pixelSizeInBytes=TrackedFrameList[0]->ImageData.GetNumberOfBytesPerPixel();
    if (pixelSizeInBytes!=sizeof(OutputPixelType))
    {
      LOG_ERROR("Output pixel size ("<<sizeof(OutputPixelType)<<" does not match the actual pixel size ("<<pixelSizeInBytes<<")");
      return PLUS_FAIL;
    }
    unsigned int frameSizeInBytes=TrackedFrameList[0]->ImageData.GetFrameSizeInBytes();

    for ( int i = 0 ; i < numberOfFrames; i++ ) 
    {
      int trackedFrameListItem = (fileNumber - 1) * this->MaxNumOfFramesToWrite + i; 
      unsigned char *currentFrameImageData = imageData + i * frameSizeInBytes;

      void* mfOrientedImageBufferPtr = TrackedFrameList[trackedFrameListItem]->ImageData.GetBufferPointer(); 
      if ( mfOrientedImageBufferPtr!=NULL )
      {
        memcpy(currentFrameImageData, mfOrientedImageBufferPtr, frameSizeInBytes); 
      }
      else
      {
        memset(currentFrameImageData, NULL, frameSizeInBytes); 
      }

      // Write custom fields only once 
      if ( i == 0 )
      {
        // Set default frame transform name
        writerMetaImageSequenceIO->SetDefaultFrameTransformName( TrackedFrameList[trackedFrameListItem]->DefaultFrameTransformName ); 

        // Set ultrasound image orientation to the internal MF orientation 
        writerMetaImageSequenceIO->SetUltrasoundImageOrientation( "MF" ); 

        for( int field = 0; field < TrackedFrameList[trackedFrameListItem]->CustomFieldList.size(); field++ )
        {
          writerMetaImageSequenceIO->SetCustomString(TrackedFrameList[trackedFrameListItem]->CustomFieldList[field].first.c_str(), TrackedFrameList[trackedFrameListItem]->CustomFieldList[field].second.c_str() ); 
        }
      }

      for( int field = 0; field < TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList.size(); field++ )
      {
        writerMetaImageSequenceIO->SetCustomFrameString(i, TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList[field].first.c_str(), TrackedFrameList[trackedFrameListItem]->CustomFrameFieldList[field].second.c_str() ); 
      }
    }


    ImageSequenceWriterType::Pointer writer = ImageSequenceWriterType::New(); 

    std::ostringstream sequenceDataFilePath; 

    switch (extension)
    {
    case SEQ_METAFILE_MHA:
      if ( numberOfFilesToWrite == 1 )
      {
        sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << ".mha"; 
      }
      else
      {
        sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << "_" << std::setfill('0') << std::setw(2) << fileNumber << ".mha"; 
      }
      break; 
    case SEQ_METAFILE_MHD:
      if ( numberOfFilesToWrite == 1 )
      {
        sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << ".mhd"; 
      }
      else
      {
        sequenceDataFilePath << outputFolder << "/" << sequenceDataFileName << "_" << std::setfill('0') << std::setw(2) << fileNumber << ".mhd"; 
      }
      break; 
    }

    writer->SetFileName(sequenceDataFilePath.str().c_str());
    writer->SetInput(imageDataSequence); 
    writer->SetImageIO(writerMetaImageSequenceIO); 
    writer->SetUseCompression(useCompression); 

    try
    {
      writer->Update(); 
    }
    catch (itk::ExceptionObject & err) 
    {		
      LOG_ERROR(" Unable to update sequence writer: " << err.GetDescription() );
      return PLUS_FAIL; 
    }	
  }

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
PlusStatus vtkTrackedFrameList::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkTrackedFrameList::ReadConfiguration"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to read configuration from video source! (XML data element is NULL)"); 
		return PLUS_FAIL; 
	}

  vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_DEBUG("Cannot find USDataCollection element in XML tree. Use default values");
		return PLUS_SUCCESS;
	}

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_DEBUG("Cannot find Tracker element in XML tree. Use default values");
		return PLUS_SUCCESS;
  }

	if ( !trackerConfig->GetScalarAttribute("MinRequiredTranslationDifferenceMm", this->MinRequiredTranslationDifferenceMm) )
	{
    LOG_DEBUG("ImageAcquisition MinRequiredTranslationDifferenceMm attribute is not defined, use default value"); 
  }
  if ( !trackerConfig->GetScalarAttribute("MinRequiredAngleDifferenceDeg", this->MinRequiredAngleDifferenceDeg) )
	{
    LOG_DEBUG("ImageAcquisition MinRequiredAngleDifferenceDeg attribute is not defined, use default value"); 
  }

	if ( !trackerConfig->GetScalarAttribute("MaxAllowedTranslationSpeedMmPerSec", this->MaxAllowedTranslationSpeedMmPerSec) )
  {
    LOG_DEBUG("ImageAcquisition MaxAllowedTranslationSpeedMmPerSec attribute is not defined, use default value"); 
  }
	if ( !trackerConfig->GetScalarAttribute("MaxAllowedRotationSpeedDegPerSec", this->MaxAllowedRotationSpeedDegPerSec) )
  {
    LOG_DEBUG("ImageAcquisition MaxAllowedRotationSpeedDegPerSec attribute is not defined, use default value"); 
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusCommon::ITKScalarPixelType vtkTrackedFrameList::GetPixelType()
{
  if ( this->GetNumberOfTrackedFrames() < 1 )
  {
    LOG_ERROR("Unable to get pixel type size: there is no frame in the tracked frame list!"); 
    return itk::ImageIOBase::UNKNOWNCOMPONENTTYPE;
  }

  return this->GetTrackedFrame(0)->ImageData.GetITKScalarPixelType();
}
