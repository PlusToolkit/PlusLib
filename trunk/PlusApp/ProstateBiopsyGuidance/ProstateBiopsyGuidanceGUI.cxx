#include "PlusConfigure.h"
#include "ProstateBiopsyGuidanceGUI.h"
#include "ProstateBiopsyGuidance.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTrackerTool.h"
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include "vtkPlusVideoSource.h"
#include "vtkTrackedFrameList.h"
#include <string>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <sstream>

// QT includes
#include <qapplication.h>
#include <qstring.h>			
#include <qfont.h>
#include <qlabel.h>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <qlayout.h>

const QString LABEL_RECORDING_FRAME_RATE("Recording Frame Rate:");
const QString LABEL_SYNC_VIDEO_OFFSET("Video offset:");

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkDataCollector.h"
#include "vtkVideoBuffer.h"
#include "vtkXMLUtilities.h"
/*************************************** vibro test**********************************/
#include <vector>
#include <math.h>
#include "PlusConfigure.h"
#include "Wave.h"
#include "DirectSoundInstance.h"
#include "DirectSoundBuffer.h"
#include "DirectSoundCapture.h"
#include "DirectSoundCaptureBuffer.h"
#include "SignalGenerator.h"
/*************************************** vibro test****************************************/
#include <QtGui>
#include "ProstateBiopsyGuidanceGUI.h"



#include "ProstateBiopsyGuidanceGUI.h"
#include "ui_ProstateBiopsyGuidance.h"
#include "DeviceSetSelectorWidget.h"
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include "vtkConditionVariable.h"
#include "vtkMultiThreader.h"
#include <stdlib.h>

/*************************************** Define variables**********************************/
vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();					
std::string inputConfigFileName("");	// configuration file name
std::string outputFolder("/RFDATA");																		// output folder name
std::string outputVideoBufferSequenceFileName("VideoBufferMetafile");										// output file name
double inputAcqTimeLength(3);																				// Saving data time
vtkVideoBuffer *buffer_BMode = vtkVideoBuffer::New();	
vtkVideoBuffer *buffer_RF = vtkVideoBuffer::New();
vtkVideoBuffer *buffer_RF2 = vtkVideoBuffer::New();
VibroLib::AudioCard::DirectSoundBuffer dsb;
vtkMultiThreader* threadPool = vtkMultiThreader::New();
/*****************************************************************************************/

//----------------------------------------------------------------------------
// Main constuctor (is needed to be the first method within this file (according to UI object)
ProstateBiopsyGuidanceGUI::ProstateBiopsyGuidanceGUI(QWidget *parent)
:  QMainWindow(parent),    ui(new Ui::ProstateBiopsyGuidance)
{
	ui->setupUi(this);

  outputFolder=std::string(vtkPlusConfig::GetInstance()->GetProgramDirectory())+outputFolder;
  vtkPlusConfig::GetInstance()->SetOutputDirectory(outputFolder.c_str()); 

	// Create device set selector widget
	this->m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);

	// Make connections
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
	connect(ui->butStop,SIGNAL(clicked()),this,SLOT(butStop_Click()));

	// Setup device set selector widget
	this->m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->m_DeviceSetSelectorWidget->SetComboBoxMinWidth(0); 
	this->m_DeviceSetSelectorWidget->resize(this->width(), this->height()); 
	this->m_DeviceSetSelectorWidget->setGeometry(QRect(15, 30, 790, 200));
	// m_DeviceSetSelectorWidget->ui.pushButton_Connect->setText("Load");
	this->ui->butStop->setEnabled(false);
	this->ui->butInitialize->setVisible(false);

	// Set Log Level
	vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO); 
	vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_INFO);


	////double inputAcqTimeLength =atof(prostateBiopsyConfig->GetAttribute("AcquisitionTime"));
	//ifstream inFile("inputAcqTimeLength.txt");					// Read the acquistion time from text file.
	//double n;													
	//inFile >> n;
	//inputAcqTimeLength = n;
	//inFile.close();
	//label = new QLabel(tr("Acquisition Time: "));				// Add label to the text editor
	//lineEdit = new QLineEdit;									
	//label->setBuddy(lineEdit);									// When the user presses the shortcut key indicated by this label, the keyboard focus is transferred to the label's buddy widget. (http://www.greyc.ensicaen.fr/ensicaen/Docs/Qt4/qlabel.html#setBuddy)						
	//QString valueAsString = QString::number(inputAcqTimeLength);// Change the Acquistion time read to String
	//lineEdit->setText(valueAsString);							// Set the text in the edit box to the value required
	//findButton = new QPushButton(tr("Start"));				// Provide Push Button
	//findButton->setDefault(true);								
	//buttonBox = new QDialogButtonBox(Qt::Vertical);				// The QDialogButtonBox class is a widget that presents buttons in a layout that is appropriate to the current widget style.(http://doc.qt.nokia.com/stable/qdialogbuttonbox.html#details)
	//buttonBox->addButton(findButton, QDialogButtonBox::ActionRole); // Adds the specified button
	//connect ( findButton, SIGNAL( clicked() ), this, SLOT( startBiopsyProcess() ) );	// Connect SaveRFData to button to run when clicked
	//QHBoxLayout *topLeftLayout = new QHBoxLayout;				// class lines up widgets horizontally (http://doc.qt.nokia.com/4.7/qhboxlayout.html#details).
	//topLeftLayout->addWidget(label);							// Adds widget to the end of this box layout, with a stretch factor of stretch and alignment alignment.(http://doc.qt.nokia.com/latest/qboxlayout.html#addWidget)
	//topLeftLayout->addWidget(lineEdit);							
	//QVBoxLayout *leftLayout = new QVBoxLayout;
	//leftLayout->addLayout(topLeftLayout);
	//leftLayout->addStretch(1);
	//QGridLayout *mainLayout = new QGridLayout;
	//mainLayout->setSizeConstraint(QLayout::SetFixedSize);       //This property holds the resize mode of the layout.(http://doc.qt.nokia.com/latest/qlayout.html#sizeConstraint-prop)
	//mainLayout->addLayout(leftLayout, 0, 0);
	//mainLayout->addWidget(buttonBox, 0, 1);
	//setLayout(mainLayout);
	//setWindowTitle(tr("Save RF Data"));							// Set teh Title to Save RF Data
}

//----------------------------------------------------------------------------
// Deconstructor to release memory:
ProstateBiopsyGuidanceGUI::~ProstateBiopsyGuidanceGUI()
{
	delete ui;

	if ( this->TrackedFrameContainer != NULL )
	{
		this->TrackedFrameContainer->Clear(); 
		this->TrackedFrameContainer->Delete(); 
		this->TrackedFrameContainer = NULL; 
	}
}
//----------------------------------------------------------------------------
typedef struct {
	vtkMutexLock* Lock;
	vtkVideoBuffer *Buffers[2];
	int BufferInUseIndex;
	bool FirstBufferFull;
	bool SavingFinished;
	bool Done;
	vtkDataCollector* DataCollectorInstance;
} vtkThreadUserData;
//----------------------------------------------------------------------------
PlusStatus WriteVideoBufferToMetafile( vtkVideoBuffer* videoBuffer, const char* outputFolder, const char* metaFileName, bool useCompression /*=false*/ )
{
  if ( videoBuffer == NULL )
  {
    LOG_ERROR("Unable to dump video buffer if it's NULL!"); 
    return PLUS_FAIL; 
  }

  const int numberOfFrames = videoBuffer->GetNumberOfItems(); 
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  // Set default transform name
  const char* defaultFrameTransformName="IdentityTransform";
  trackedFrameList->SetDefaultFrameTransformName(defaultFrameTransformName); 

  PlusStatus status=PLUS_SUCCESS;

  for ( BufferItemUidType frameUid = videoBuffer->GetOldestItemUidInBuffer(); frameUid <= videoBuffer->GetLatestItemUidInBuffer(); ++frameUid ) 
  {

    VideoBufferItem videoItem; 
    if ( videoBuffer->GetVideoBufferItem(frameUid, &videoItem) != ITEM_OK )
    {
      LOG_ERROR("Unable to get frame from buffer with UID: " << frameUid); 
      status=PLUS_FAIL;
      continue; 
    }

    TrackedFrame trackedFrame;
    trackedFrame.SetImageData(videoItem.GetFrame());

    // Add transform 
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    matrix->Identity(); 
    trackedFrame.SetCustomFrameTransform(defaultFrameTransformName, matrix); 

    // Add filtered timestamp
    double filteredTimestamp = videoItem.GetFilteredTimestamp( videoBuffer->GetLocalTimeOffset() ); 
    std::ostringstream timestampFieldValue; 
    timestampFieldValue << std::fixed << filteredTimestamp; 
    trackedFrame.SetCustomFrameField("Timestamp", timestampFieldValue.str()); 

    // Add unfiltered timestamp
    double unfilteredTimestamp = videoItem.GetUnfilteredTimestamp( videoBuffer->GetLocalTimeOffset() ); 
    std::ostringstream unfilteredtimestampFieldValue; 
    unfilteredtimestampFieldValue << std::fixed << unfilteredTimestamp; 
    trackedFrame.SetCustomFrameField("UnfilteredTimestamp", unfilteredtimestampFieldValue.str()); 

    // Add frame number
    unsigned long frameNumber = videoItem.GetIndex();  
    std::ostringstream frameNumberFieldValue; 
    frameNumberFieldValue << std::fixed << frameNumber; 
    trackedFrame.SetCustomFrameField("FrameNumber", frameNumberFieldValue.str()); 

    // Add tracked frame to the list
    trackedFrameList->AddTrackedFrame(&trackedFrame); 
  }

  // Save tracked frames to metafile
  if ( trackedFrameList->SaveToSequenceMetafile(outputFolder, metaFileName, vtkTrackedFrameList::SEQ_METAFILE_MHA, useCompression) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    return PLUS_FAIL;
  }

  return status;
}
//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE saveDataAsync( void* arg )
{
	if ( !vtksys::SystemTools::FileExists(outputFolder.c_str(), false) )
		vtksys::SystemTools::MakeDirectory(outputFolder.c_str()); 

	//Preparing data:
	vtkThreadUserData* threadData = static_cast<vtkThreadUserData*>(static_cast<vtkMultiThreader::ThreadInfo*>(arg)->UserData );

	// Just wait for the first buffer to fill:
	while( !threadData->Done && !threadData->FirstBufferFull){};

	int _bufferReadyToWriteIndex=0;

	while( !threadData->Done){
		if(!threadData->SavingFinished)//threadData->BufferInUseIndex!=_bufferInUseIndex)
		{
			LOG_INFO("DATA-SAVE Thread: save attempt at frame #"<<threadData->DataCollectorInstance->GetVideoSource()->GetFrameNumber() <<" on buffer index #" <<_bufferReadyToWriteIndex  );
			// Save Data to Disk:
			_bufferReadyToWriteIndex=(threadData->BufferInUseIndex+1)%2;
			vtkVideoBuffer* buff= threadData->Buffers[_bufferReadyToWriteIndex];
			std::stringstream _fileName;
			_fileName << "RF_BLOCK_"<<threadData->DataCollectorInstance->GetVideoSource()->GetFrameNumber();
			WriteVideoBufferToMetafile(buff, outputFolder.c_str(),_fileName.str().c_str(), false);
			threadData->Buffers[_bufferReadyToWriteIndex]->Clear();
			threadData->SavingFinished=true;
		}
	}

	LOG_INFO("DATA-SAVE Thread: Finished");
	return VTK_THREAD_RETURN_VALUE;
}
//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE getDataAsync( void* arg )
{
	LOG_INFO("DATA-GET Thread: Started");
	
	//Preparing data:
	vtkThreadUserData* threadData = static_cast<vtkThreadUserData*>(static_cast<vtkMultiThreader::ThreadInfo*>(arg)->UserData );

	bool _readyToSave(false);
	while ( !threadData->Done )	
	{
		_readyToSave=_readyToSave || threadData->DataCollectorInstance->GetVideoSource()->GetFrameNumber()%45==0;
		if(_readyToSave && threadData->SavingFinished)
		{
			threadData->DataCollectorInstance->CopyVideoBuffer(threadData->Buffers[threadData->BufferInUseIndex]);
			threadData->DataCollectorInstance->GetVideoSource()->GetBuffer()->Clear();
			threadData->FirstBufferFull=true;
			
			//Switch the index of in-use buffer:
			threadData->BufferInUseIndex=(threadData->BufferInUseIndex+1)%2;
			threadData->SavingFinished=false;
			_readyToSave=false;
		}
	}


	threadData->Done=true;
	LOG_INFO("DATA-GET Thread: Finished");
	return VTK_THREAD_RETURN_VALUE;
}



//Slots implemention
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::startShaker()
{
		HRESULT hr = 0;
		// define Shaker Frequencies.
		std::vector<double> frequencies;															
		frequencies.push_back((double)20);
		frequencies.push_back((double)92);
		frequencies.push_back((double)110);
		// Generate multiple frequencies: frequencies: Input frequency you defined, double sampling_frequency, int nsamples.
		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);
		VibroLib::AudioCard::Wave Wv;
		Wv.FromSignal(signal);
		VibroLib::AudioCard::DirectSoundInstance dsi;
		dsi.Initialize();

		// Sets the application to the priority level.
		// Applications with this cooperative level can call the DirectSoundBuffer.setFormat 
		// and DirectSound.compact methods (http://timgolden.me.uk/pywin32-docs/directsound_DSSCL_PRIORITY.html).
		if (dsi.SetPriority(GetConsoleWindow(), DSSCL_PRIORITY)!=PLUS_SUCCESS)
		{
			LOG_ERROR( "Unable to set cooperative level. HRESULT:" << hr );
			return PLUS_FAIL;
		}

		// initialize the wave with this properties DSBCAPS_GLOBALFOCUS & DSBCAPS_STICKYFOCUS : Continue to play sound in buffer even if the user switch to another application.
		if ((hr = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != DS_OK)
		{
//			LOG_ERROR( "Unable to initialize buffer. HRESULT:" << hr);
//			return PLUS_FAIL;
		}
		// Play the wave previously initialized and return error if could not
		// DSBPLAY_LOOPING: Will restart playing once the Audio buffer reachs end. Until stopped		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
		{
			LOG_ERROR( "Failed to play buffer. HRESULT: " << hr );
			return PLUS_FAIL;
		}

		else
		{
			LOG_INFO("\nRunning...\n");
			return PLUS_SUCCESS;
		}

return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::stopShaker()
{	
	dsb->Stop();
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
/*PlusStatus ProstateBiopsyGuidanceGUI::SaveBModeData()
{

	LOG_INFO("write Bmode Data to file");												// Save Data To File specified
	printf("write Bmode Data to file \n");
	outputVideoBufferSequenceFileName="B_Mode";
	dataCollector->WriteVideoBufferToMetafile( buffer_BMode, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true); 

	return PLUS_SUCCESS;
}*/

/*PlusStatus ProstateBiopsyGuidanceGUI::AcquireBModeData()
{
	LOG_INFO("press a to start" );
	printf("press a to start\n");
	while (_getch() != 'a')															// wait until a is pressed to start acquisition
	{;}
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime();					// Get PC Time 

	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )	// Save Time for the period specified in inputAcqTimeLength
	{
		//LOG_INFO("%f seconds left...", acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() );
		vtksys::SystemTools::Delay(1000);											// Delay for one second
	}
	LOG_INFO("Copy Bmode Data to Buffer" );													// Copy Data From Video Buffer
	printf("Copy RF Data to Buffer \n");								

	return PLUS_SUCCESS;
}*/

// when we figure out how to save Bmode data we will use the type, I put it as int we can change that
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::acquireData(vtkVideoBuffer *Data,int type,std::string savedBufferName ) 
{
	LOG_INFO("press a to start\n" );
	//printf("press a to start\n");
	// wait until a is pressed to start acquisition
	while (_getch() != 'a')															
	{;}
	dataCollector->Start();

#pragma region Start Save Thread
	vtkThreadUserData data;
	data.Lock = vtkMutexLock::New();
	data.Lock->Unlock();
	data.FirstBufferFull=false;
	data.Done = false;
	data.SavingFinished=true;
	for(int k=0;k<2;k++)
		data.Buffers[k]=vtkVideoBuffer::New();
	data.BufferInUseIndex=0;
	data.DataCollectorInstance=dataCollector;

	threadPool->SetNumberOfThreads( 2 );
	threadPool->SetMultipleMethod(0, getDataAsync , &data);
	threadPool->SetMultipleMethod(1, saveDataAsync , &data);
	threadPool->MultipleMethodExecute();
#pragma endregion

	
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::stopDataAquisition()
{
	dataCollector->Stop();
	// we have to know what is this
	LOG_INFO( "Exit !\n" );
	//printf("Exit !\n");
	return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::saveData(vtkVideoBuffer *Data,std::string BufferFileName)
{
	// Save Data To File specified
	LOG_INFO("write Data to file \n"<<BufferFileName.c_str());												
	//printf("write Data to file: %s \n",BufferFileName.c_str());
	dataCollector->WriteVideoBufferToMetafile( Data, outputFolder.c_str(), BufferFileName.c_str(), false);
	return PLUS_SUCCESS;

}
//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::initialize()
{
	vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;
	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
	// Check if configuration file is required.
	if (inputConfigFileName.empty())											
	{
		LOG_INFO( "input-config-file-name is required" );
		exit(EXIT_FAILURE);
	}
  // Check if configuration can be read.
  if (configRootElement == NULL) {												
    std::cerr << "Unable to read configuration from file " << inputConfigFileName;
		exit(EXIT_FAILURE);
  }
  // Check if configuration file can be read
  if ( dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS )	
  {
    std::cerr << "Failed to read configuration!";
	exit(EXIT_FAILURE);
  }
	// Initialize Prostate Biopsy Guidance for Data Collection as defined in configuration File.
	dataCollector->Initialize();												
	return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::deleteBuffer(vtkVideoBuffer *Data)
{

		Data->Delete();
		return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::startBiopsyProcess()
{
	initialize();				
/****************************************************************/
	acquireData(buffer_BMode,1,"BMode");
/****************************************************************/
	startShaker();				
	acquireData(buffer_RF,1,"RF1");	
	stopShaker();				
/****************************************************************/
	acquireData(buffer_RF2,1,"RF2");
/****************************************************************/
	saveData(buffer_RF,"RF_Data1");
	deleteBuffer(buffer_RF);
/****************************************************************/
	saveData(buffer_RF2,"RF_Data2");
	deleteBuffer(buffer_RF2);
/****************************************************************/
	saveData(buffer_BMode,"BMode_Data");
	deleteBuffer(buffer_BMode);
/****************************************************************/
	stopDataAquisition();
/****************************************************************/
	return PLUS_SUCCESS;

}
//----------------------------------------------------------------------------
void ProstateBiopsyGuidanceGUI::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
	LOG_INFO("Loading config file ...");

	vtkXMLDataElement* configXmlRoot=NULL;

  if (ReadPBGConfigData(aConfigFile, &configXmlRoot)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to read config file "<<aConfigFile.c_str());
    return;
  }
	m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

	if ( this->TrackedFrameContainer == NULL )
	{
		this->TrackedFrameContainer = vtkTrackedFrameList::New(); 
		this->TrackedFrameContainer->ReadConfiguration(configXmlRoot);
	}

  configXmlRoot->Delete();

	this->ui->butStop->setEnabled(true);
	startBiopsyProcess();
}

//----------------------------------------------------------------------------
PlusStatus ProstateBiopsyGuidanceGUI::ReadPBGConfigData(std::string aConfigFile, vtkXMLDataElement **configRootElement)
{
	
	inputConfigFileName = aConfigFile;

	/*
	% 2011-9-16, Code by Saman %
	We need to read some configuration parameters from XmlConfig file
	There should be a node in second layer of Xml file with node name of "ProstateBiopsy".
	Parameters are preset there as xml node attributes:
	Example:	
	<ProstateBiopsy AcquisitionTime="3" />
	*/

	std::string inputConfigFileName(aConfigFile);
	(*configRootElement) = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
	if (configRootElement == NULL) {	
		LOG_ERROR("Unable to read configuration from file " << inputConfigFileName);
		return PLUS_FAIL;
	}

	vtkXMLDataElement* prostateBiopsyConfig = (*configRootElement)->FindNestedElementWithName("ProstateBiopsy");
	if (prostateBiopsyConfig == NULL)
	{
		LOG_ERROR("Cannot find ProstateBiopsy element in XML tree!");
		return PLUS_FAIL;
	}

	inputAcqTimeLength=atof(prostateBiopsyConfig->GetAttribute("AcquisitionTime"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void ProstateBiopsyGuidanceGUI::butStop_Click(){
	LOG_INFO("Stop Clicked");
	this->ui->butStop->setEnabled(false);

}



//----------------------------------------------------------------------------
