#include "vtkTrackedFrameList.h" 
#include "PlusMath.h"

#include <math.h>
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include "itkFixedArray.h"


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
  if ( this->ImageData.IsNotNull() )
  {
    const int w = this->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
    const int h = this->ImageData->GetLargestPossibleRegion().GetSize()[1]; 
    this->FrameSize[0] = w; 
    this->FrameSize[1] = h; 
  }

  return this->FrameSize; 
}


//----------------------------------------------------------------------------
int TrackedFrame::GetNumberOfBitsPerPixel()
{
  int numberOfBitsPerPixel(0); 
  if ( this->ImageData.IsNotNull() )
  {
    numberOfBitsPerPixel = this->ImageData->GetNumberOfComponentsPerPixel() * sizeof(PixelType)*8;
  }

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

  // TODO from configuration file
  this->SetVelocityPositionThreshold(10.0);
  this->SetVelocityOrientationThreshold(5.0);
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
  if ( frameNumber < this->GetNumberOfTrackedFrames() )
  {
    return this->TrackedFrameList[frameNumber]; 
  }

  return NULL; 
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
  bool validationResult(true); 

  if ( validateTimestamp )
  {
    validationResult &= this->ValidateTimestamp(trackedFrame); 
  }

  if ( validateStatus )
  {
    validationResult &= this->ValidateStatus(trackedFrame); 
  }

  if ( validatePosition )
  {
    validationResult &= this->ValidatePosition(trackedFrame, frameTransformName); 
  }

  if ( validateSpeed )
  {
    validationResult &= this->ValidateSpeed(trackedFrame);
  }

  return validationResult; 
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

  if (std::find_if(searchIndex, this->TrackedFrameList.end(), TrackedFramePositionFinder(trackedFrame, frameTransformName) ) != this->TrackedFrameList.end() )
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
  if ( this->TrackedFrameList.size() <= 2 ) {
    return true;
  }

  TrackedFrameListType::iterator latestFrame = this->TrackedFrameList.end() - 1;
  TrackedFrameListType::iterator secondLatestFrame = this->TrackedFrameList.end() - 2;

  // Compute difference between the last two timestamps
  double diffTime = fabs( (*latestFrame)->Timestamp - (*secondLatestFrame)->Timestamp );

  // Get default frame transform of latest and second latest frames
  vtkSmartPointer<vtkTransform> latestTransform = vtkSmartPointer<vtkTransform>::New(); 
  double latestTransformVector[16]={0}; 
  if ( (*latestFrame)->GetDefaultFrameTransform(latestTransformVector) )
  {
    latestTransform->SetMatrix(latestTransformVector); 
  }
  else
  {
    LOG_ERROR("Unable to get default frame transform for latest frame!");
    return false;
  }

  vtkSmartPointer<vtkTransform> secondLatestTransform = vtkSmartPointer<vtkTransform>::New(); 
  double secondLatestTransformVector[16]={0}; 
  if ( (*secondLatestFrame)->GetDefaultFrameTransform(secondLatestTransformVector) )
  {
    secondLatestTransform->SetMatrix(secondLatestTransformVector); 
  }
  else
  {
    LOG_ERROR("Unable to get default frame transform for second latest frame!");
    return false;
  }

  // Compute difference between the last two positions
  double diffPosition = PlusMath::GetPositionDifference( latestTransform->GetMatrix(), secondLatestTransform->GetMatrix() );

  // Compute difference between the last two orientations
  double diffOrientation = PlusMath::GetOrientationDifference( latestTransform->GetMatrix(), secondLatestTransform->GetMatrix() );

  // Compute position and orientation speed and decide if they are acceptable
  double velocityPosition = diffPosition / diffTime;
  double velocityOrientation = fabs( diffOrientation / diffTime );

  if ( velocityPosition > this->VelocityPositionThreshold || velocityOrientation > this->VelocityOrientationThreshold )
  {
    LOG_DEBUG("Tracked frame speed validation result: tracked frame change too fast (VelocityPosition = " << velocityPosition << ">" << this->VelocityPositionThreshold << " and/or VelocityOrientation = " << velocityOrientation << ">" << this->VelocityOrientationThreshold << ")"); 
    return false; 
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
  typedef itk::Image< TrackedFrame::PixelType, 3 > ImageSequenceType;
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

  const int frameSizeInPx[2] = {imageSeq->GetLargestPossibleRegion().GetSize()[0], imageSeq->GetLargestPossibleRegion().GetSize()[1]};  
  const unsigned long numberOfFrames = imageSeq->GetLargestPossibleRegion().GetSize()[2];	
  const int pixelSizeInBits = sizeof(TrackedFrame::PixelType)*8; 
  US_IMAGE_ORIENTATION usImageOrientation = UsImageConverterCommon::GetUsImageOrientationFromString(readerMetaImageSequenceIO->GetUltrasoundImageOrientation()); 
  
  unsigned int frameSizeInBytes=frameSizeInPx[0]*frameSizeInPx[1]*sizeof(TrackedFrame::PixelType);

  TrackedFrame::PixelType* imageSeqData = imageSeq->GetBufferPointer(); // pointer to the image pixel buffer

  for ( int imgNumber = 0; imgNumber < numberOfFrames; imgNumber++ )
  {

    TrackedFrame trackedFrame; 
    TrackedFrame::PixelType *currentFrameImageData= imageSeqData + imgNumber * frameSizeInBytes;

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

    trackedFrame.ImageData = TrackedFrame::ImageType::New(); 
    if ( UsImageConverterCommon::GetMFOrientedImage(currentFrameImageData, usImageOrientation, frameSizeInPx, pixelSizeInBits, trackedFrame.ImageData) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to get MF oriented image from sequence metafile (frame number: " << imgNumber << ")!"); 
      continue; 
    }

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

  typedef itk::Image< TrackedFrame::PixelType, 3 > ImageSequenceType;
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

    unsigned long ImageWidthInPixels(1), ImageHeightInPixels(1); 
    if ( TrackedFrameList[0]->ImageData.IsNotNull() )
    {
      ImageWidthInPixels = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[0]; 
      ImageHeightInPixels = TrackedFrameList[0]->ImageData->GetLargestPossibleRegion().GetSize()[1]; 
    }

    ImageSequenceType::SizeType size = {ImageWidthInPixels, ImageHeightInPixels, numberOfFrames };
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

    TrackedFrame::PixelType* imageData = imageDataSequence->GetBufferPointer(); // pointer to the image pixel buffer



    unsigned int frameSizeInBytes=ImageWidthInPixels*ImageHeightInPixels*sizeof(TrackedFrame::PixelType);

    for ( int i = 0 ; i < numberOfFrames; i++ ) 
    {
      int trackedFrameListItem = (fileNumber - 1) * this->MaxNumOfFramesToWrite + i; 
      TrackedFrame::PixelType *currentFrameImageData = imageData + i * frameSizeInBytes;

      TrackedFrame::ImageType::Pointer mfOrientedImage = TrackedFrameList[trackedFrameListItem]->ImageData; 
      if ( mfOrientedImage.IsNotNull() )
      {
        memcpy(currentFrameImageData, mfOrientedImage->GetBufferPointer(), frameSizeInBytes); 
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

