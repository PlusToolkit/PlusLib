#include "PlusConfigure.h"
#include "ProstateBiopsyGuidanceGUI.h"
#include "ProstateBiopsyGuidance.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTrackerTool.h"
#include "vtkCommand.h"
#include "vtkRenderWindow.h"
#include <string>
#include <stdio.h>
#include <conio.h>
#include <time.h>

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
/*************************************** Define variables**********************************/
vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();					
std::string inputConfigFileName("Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");	// configuration file name
std::string outputFolder("./RFDATA");																		// output folder name
std::string outputVideoBufferSequenceFileName("VideoBufferMetafile");										// output file name
double inputAcqTimeLength(3);																				// Saving data time
vtkVideoBuffer *buffer_BMode = vtkVideoBuffer::New();	
vtkVideoBuffer *buffer_RF = vtkVideoBuffer::New();
vtkVideoBuffer *buffer_RF2 = vtkVideoBuffer::New();
VibroLib::AudioCard::DirectSoundBuffer dsb;
#define RF_Buffer1 1
#define RF_Buffer2 2
#define BMode_Buffer 3
/*****************************************************************************************/
PlusStatus ProstateBiopsyGuidanceGUI::startShaker()
{
		HRESULT hr = 0;
		
		std::vector<double> frequencies;															// define Shaker Frequencies.
		frequencies.push_back((double)20);
		frequencies.push_back((double)92);
		frequencies.push_back((double)110);

		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);	// Generate 
		VibroLib::AudioCard::Wave Wv;
		//Wv.FromData(sig, signal.size(), bits_per_sample, samples_per_sec);
		Wv.FromSignal(signal);
		VibroLib::AudioCard::DirectSoundInstance dsi;
		dsi.Initialize();

		if (dsi.SetPriority(GetConsoleWindow(), DSSCL_PRIORITY)!=PLUS_SUCCESS)
		{
			LOG_ERROR( "Unable to set cooperative level. HRESULT:" << hr );
//			return PLUS_FAIL;
		}

		
		if ((hr = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != DS_OK)
		{
//			LOG_ERROR( "Unable to initialize buffer. HRESULT:" << hr);
//			return PLUS_FAIL;
		}
		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
		{
			LOG_ERROR( "Failed to play buffer. HRESULT: " << hr );
//			return PLUS_FAIL;
		}

		else
		{
			LOG_INFO("\nRunning...\n");
			return PLUS_SUCCESS;
		}

return PLUS_SUCCESS;
}
PlusStatus ProstateBiopsyGuidanceGUI::StopShaker()
{	
	dsb->Stop();
	return PLUS_SUCCESS;
}
PlusStatus ProstateBiopsyGuidanceGUI::SaveBModeData()
{

	LOG_INFO("write Bmode Data to file");												// Save Data To File specified
	printf("write Bmode Data to file \n");
	outputVideoBufferSequenceFileName="B_Mode";
	dataCollector->WriteVideoBufferToMetafile( buffer_BMode, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true); 

	return PLUS_SUCCESS;
}

PlusStatus ProstateBiopsyGuidanceGUI::AcquireBModeData()
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
}


PlusStatus ProstateBiopsyGuidanceGUI::AcquireRFData(int RF_Step)
{
	LOG_INFO("press a to start\n" );
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
	LOG_INFO("Copy RF Data to Buffer\n" );													// Copy Data From Video Buffer
	printf("Copy RF Data to Buffer \n");
	if(RF_Step==1)
	{dataCollector->CopyVideoBuffer(buffer_RF);}
	else if(RF_Step==2)
	{dataCollector->CopyVideoBuffer(buffer_RF2);}
	return PLUS_SUCCESS;
}
PlusStatus ProstateBiopsyGuidanceGUI::StopBModeDataAquisition()
{
//	buffer_BMode->Delete();
	dataCollector->Stop();
	VTK_LOG_TO_CONSOLE_OFF; 
	LOG_INFO( "Exit !\n" );
	printf("Exit !\n");

	return PLUS_SUCCESS;
}
PlusStatus ProstateBiopsyGuidanceGUI::StopRFModeDataAquisition()
{


	dataCollector->Stop();
	VTK_LOG_TO_CONSOLE_OFF; 
	LOG_INFO( "Exit !\n" );
	printf("Exit !\n");
	return PLUS_SUCCESS;
}
PlusStatus ProstateBiopsyGuidanceGUI::SaveRFData(int RF_Step)
{


	LOG_INFO("write RF Data to file \n");												// Save Data To File specified
	printf("write RF Data to file \n");
	if(RF_Step==1)
	{	outputVideoBufferSequenceFileName="RF_Data1"; // change not prof.
		dataCollector->WriteVideoBufferToMetafile( buffer_RF, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true);
	}
	else if(RF_Step==2)
	{	outputVideoBufferSequenceFileName="RF_Data2";   // change not prof.
		dataCollector->WriteVideoBufferToMetafile( buffer_RF, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true);
	}

	return PLUS_SUCCESS;

}
PlusStatus ProstateBiopsyGuidanceGUI::Initialize()
{
	vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;
	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
	if (inputConfigFileName.empty())											// Check if configuration file is required.
	{
		LOG_INFO( "input-config-file-name is required" );
		exit(EXIT_FAILURE);
	}
	VTK_LOG_TO_CONSOLE_ON; 
  if (configRootElement == NULL) {												// Check if configuration can be read.
    std::cerr << "Unable to read configuration from file " << inputConfigFileName;
		exit(EXIT_FAILURE);
  }
  if ( dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS )	// Check if configuration file can be read
  {
    std::cerr << "Failed to read configuration!";
	exit(EXIT_FAILURE);
  }
	dataCollector->Initialize();												// Initialize Prostate Biopsy Guidance for Data Collection as defined in configuration File.
	return PLUS_SUCCESS;
}


PlusStatus ProstateBiopsyGuidanceGUI::DeletBuffer(int RF_Step)
{
	if(RF_Step == RF_Buffer1)
	{	buffer_RF->Delete();}
	else if (RF_Step == RF_Buffer2)
	{	buffer_RF2->Delete();}
	else if (RF_Step == BMode_Buffer)
	{	buffer_BMode->Delete();}
		return PLUS_SUCCESS;
}

PlusStatus ProstateBiopsyGuidanceGUI::StartBiopsyprocess()
{
	Initialize();
//	AcquireBModeData();
//	SaveBModeData();
//	DeletBuffer(BMode_Buffer);
//	StopBModeDataAquisition();
//	buffer_BMode->Delete();
	startShaker();
	AcquireRFData(RF_Buffer1);
	StopShaker();
	SaveRFData(RF_Buffer1);
//	DeletBuffer(RF_Buffer1);

	AcquireRFData(RF_Buffer1);
	SaveRFData(RF_Buffer2);
	DeletBuffer(RF_Buffer1);

	StopRFModeDataAquisition();
	return PLUS_SUCCESS;

}
 ProstateBiopsyGuidanceGUI::ProstateBiopsyGuidanceGUI(QWidget *parent)
     : QDialog(parent)
 {

	// /*
	//% 2011-9-16, Code by Saman %
	//We need to read some configuration parameters from XmlConfig file
	//There should be a node in second layer of Xml file with node name of "ProstateBiopsy".
	//Parameters are preset there as xml node attributes:
	//Example:	
	//	<ProstateBiopsy AcquisitionTime="3" />
	//*/
	//#pragma region Reading Config File
	//std::string inputConfigFileName("C:/Saman/Work/Source/PLUS/PlusApp-bin/bin/Debug/Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");
	//vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
	//if (configRootElement == NULL) {	
	//	std::cerr << "Unable to read configuration from file " << inputConfigFileName;
	//	exit(EXIT_FAILURE);
	//}

	//vtkSmartPointer<vtkXMLDataElement> prostateBiopsyConfig = configRootElement->FindNestedElementWithName("ProstateBiopsy");
	//if (prostateBiopsyConfig == NULL)
	//{
	//	LOG_ERROR("Cannot find ProstateBiopsy element in XML tree!");
	//	exit( PLUS_FAIL);
	//}
	//#pragma endregion

	//double inputAcqTimeLength =atof(prostateBiopsyConfig->GetAttribute("AcquisitionTime"));
		ifstream inFile("inputAcqTimeLength.txt");					// Read the acquistion time from text file.
		double n;													
		inFile >> n;
		inputAcqTimeLength = n;
		inFile.close();
		label = new QLabel(tr("Acquisition Time: "));				// Add label to the text editor
		lineEdit = new QLineEdit;									
		label->setBuddy(lineEdit);									// When the user presses the shortcut key indicated by this label, the keyboard focus is transferred to the label's buddy widget. (http://www.greyc.ensicaen.fr/ensicaen/Docs/Qt4/qlabel.html#setBuddy)						
		QString valueAsString = QString::number(inputAcqTimeLength);// Change the Acquistion time read to String
		lineEdit->setText(valueAsString);							// Set the text in the edit box to the value required
		findButton = new QPushButton(tr("Start"));				// Provide Push Button
		findButton->setDefault(true);								
		buttonBox = new QDialogButtonBox(Qt::Vertical);				// The QDialogButtonBox class is a widget that presents buttons in a layout that is appropriate to the current widget style.(http://doc.qt.nokia.com/stable/qdialogbuttonbox.html#details)
	    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole); // Adds the specified button
		connect ( findButton, SIGNAL( clicked() ), this, SLOT( StartBiopsyprocess() ) );	// Connect SaveRFData to button to run when clicked
		QHBoxLayout *topLeftLayout = new QHBoxLayout;				// class lines up widgets horizontally (http://doc.qt.nokia.com/4.7/qhboxlayout.html#details).
		topLeftLayout->addWidget(label);							// Adds widget to the end of this box layout, with a stretch factor of stretch and alignment alignment.(http://doc.qt.nokia.com/latest/qboxlayout.html#addWidget)
		topLeftLayout->addWidget(lineEdit);							
		QVBoxLayout *leftLayout = new QVBoxLayout;
		leftLayout->addLayout(topLeftLayout);
		leftLayout->addStretch(1);
		QGridLayout *mainLayout = new QGridLayout;
		mainLayout->setSizeConstraint(QLayout::SetFixedSize);       //This property holds the resize mode of the layout.(http://doc.qt.nokia.com/latest/qlayout.html#sizeConstraint-prop)
		mainLayout->addLayout(leftLayout, 0, 0);
		mainLayout->addWidget(buttonBox, 0, 1);
		setLayout(mainLayout);
		setWindowTitle(tr("Save RF Data"));							// Set teh Title to Save RF Data
 }