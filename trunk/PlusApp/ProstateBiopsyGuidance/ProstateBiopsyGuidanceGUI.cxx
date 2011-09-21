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
/*************************************** vibro test**********************************/
 #include <QtGui>

 #include "ProstateBiopsyGuidanceGUI.h"
/*************************************** vibro test**********************************/

PlusStatus ProstateBiopsyGuidanceGUI::startShaker()
{
		HRESULT hr = 0;
		
		std::vector<double> frequencies;
		frequencies.push_back((double)20);
		frequencies.push_back((double)92);
		frequencies.push_back((double)110);

		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);
		VibroLib::AudioCard::Wave Wv;
		//Wv.FromData(sig, signal.size(), bits_per_sample, samples_per_sec);
		Wv.FromSignal(signal);
		VibroLib::AudioCard::DirectSoundInstance dsi;
		dsi.Initialize();

		if (dsi.SetPriority(GetConsoleWindow(), DSSCL_PRIORITY)!=PLUS_SUCCESS)
		{
			LOG_ERROR( "Unable to set cooperative level. HRESULT:" << hr );
			return PLUS_FAIL;
		}

		VibroLib::AudioCard::DirectSoundBuffer dsb;
		if ((hr = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != DS_OK)
		{
			LOG_ERROR( "Unable to initialize buffer. HRESULT:" << hr);
			return PLUS_FAIL;
		}
		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
		{
			LOG_ERROR( "Failed to play buffer. HRESULT: " << hr );
			return PLUS_FAIL;
		}

		else
		{
			LOG_INFO("\nRunning...\n");
//			system("PAUSE");
//			ifstream myfile;
//			myfile.open("duration.txt");
//			int duration;
//			myfile >> duration;
//			myfile.close();
//			cout<<duration<<endl;
			Sleep(6000);
			dsb->Stop();
			return PLUS_SUCCESS;
		}


}











	double inputAcqTimeLength(3);
PlusStatus ProstateBiopsyGuidanceGUI::SaveRFData()
{

	std::string inputConfigFileName("Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");//"Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");
	std::string outputFolder("./RFDATA");
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;


	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputConfigFileName.empty())
	{
		LOG_INFO( "input-config-file-name is required" );
		exit(EXIT_FAILURE);
	}

	///////////////

	VTK_LOG_TO_CONSOLE_ON; 

	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL) {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName;
		exit(EXIT_FAILURE);
  }

  if ( dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    std::cerr << "Failed to read configuration!";
		exit(EXIT_FAILURE);
  }
	dataCollector->Initialize();

	while (_getch() != 'a') // wait until a is pressed to start acquisition
	{;}
	dataCollector->Start();

	const double acqStartTime = vtkTimerLog::GetUniversalTime(); 

	while ( acqStartTime + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		//LOG_INFO("%f seconds left...", acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() );
		vtksys::SystemTools::Delay(1000); 
	}


	LOG_INFO("Copy video buffer" );
	vtkVideoBuffer *buffer = vtkVideoBuffer::New(); 
	dataCollector->CopyVideoBuffer(buffer); 

	LOG_INFO("write video buffer: "<< outputVideoBufferSequenceFileName );
	dataCollector->WriteVideoBufferToMetafile( buffer, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true); 

	buffer->Delete(); 
	dataCollector->Stop();
	VTK_LOG_TO_CONSOLE_OFF; 
	LOG_INFO( "RF_Data is Saved !\n" );

/*********************************************************************************/
	///////////////

	VTK_LOG_TO_CONSOLE_ON; 
	std::string outputVideoBufferSequenceFileName_V("VideoBufferMetafile_v"); 
	while (_getch() != 'a') // wait until a is pressed to start acquisition
	{;}
	startShaker();
	dataCollector->Start();

	const double acqStartTime_V = vtkTimerLog::GetUniversalTime(); 

	while ( acqStartTime_V + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		//LOG_INFO("%f seconds left...\n", acqStartTime_V + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() );
		vtksys::SystemTools::Delay(1000); 
	}
	LOG_INFO("Copy video buffer" );
	vtkVideoBuffer *v_buffer = vtkVideoBuffer::New(); 
	dataCollector->CopyVideoBuffer(v_buffer); 
	LOG_INFO("write video buffer: " << outputVideoBufferSequenceFileName_V );
	dataCollector->WriteVideoBufferToMetafile( v_buffer, outputFolder.c_str(), outputVideoBufferSequenceFileName_V.c_str(), true); 

	v_buffer->Delete(); 
	dataCollector->Stop();
	LOG_INFO( "Vibro_Data is Saved!");

	VTK_LOG_TO_CONSOLE_OFF;
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


		ifstream inFile("inputAcqTimeLength.txt");
		ofstream outFile("SAMAN_TEST.txt");
		outFile.close();
		double n;
		inFile >> n;
		inputAcqTimeLength = n;
		inFile.close();
		label = new QLabel(tr("Acquisition Time: "));
		lineEdit = new QLineEdit;
		label->setBuddy(lineEdit);
		QString valueAsString = QString::number(inputAcqTimeLength);
		lineEdit->setText(valueAsString);

		findButton = new QPushButton(tr("Initialize"));
		findButton->setDefault(true);


		buttonBox = new QDialogButtonBox(Qt::Vertical);
	    buttonBox->addButton(findButton, QDialogButtonBox::ActionRole);

		connect ( findButton, SIGNAL( clicked() ), this, SLOT( SaveRFData() ) );

		QHBoxLayout *topLeftLayout = new QHBoxLayout;
		topLeftLayout->addWidget(label);
		topLeftLayout->addWidget(lineEdit);

		QVBoxLayout *leftLayout = new QVBoxLayout;
		leftLayout->addLayout(topLeftLayout);
		leftLayout->addStretch(1);
		QGridLayout *mainLayout = new QGridLayout;
		mainLayout->setSizeConstraint(QLayout::SetFixedSize);
		mainLayout->addLayout(leftLayout, 0, 0);
		mainLayout->addWidget(buttonBox, 0, 1);
		setLayout(mainLayout);

     setWindowTitle(tr("Save RF Data"));

//     extension->hide();
 }

