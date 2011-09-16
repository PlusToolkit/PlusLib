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
#include "vtkPlusConfig.h"
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
//#include "Wave.h"
//#include "DirectSoundInstance.h"
//#include "DirectSoundBuffer.h"
//#include "DirectSoundCapture.h"
//#include "DirectSoundCaptureBuffer.h"
//#include "SignalGenerator.h"
/*************************************** vibro test**********************************/
 #include <QtGui>

 #include "ProstateBiopsyGuidanceGUI.h"
/*************************************** vibro test**********************************/

/*void ProstateBiopsyGuidanceGUI::startShaker(void)
{


		std::vector<double> frequencies;
		frequencies.push_back((double)"20");
		frequencies.push_back((double)"92");
		frequencies.push_back((double)"110");

		std::vector<double> signal = VibroLib::GenerateMultiFrequency(frequencies, 8192.0, 20000);
		VibroLib::AudioCard::Wave Wv;
		//Wv.FromData(sig, signal.size(), bits_per_sample, samples_per_sec);
		Wv.FromSignal(signal);
		VibroLib::AudioCard::DirectSoundInstance dsi;
		dsi.Initialize();

		if ((hr=dsi.SetPriority(GetConsoleWindow(), DSSCL_PRIORITY)) != DS_OK)
		{
			printf("Unable to set cooperative level: HRESULT: %d\n", hr);
			system("PAUSE");
		}

		VibroLib::AudioCard::DirectSoundBuffer dsb;
		if ((hr = dsb.Initialize(&dsi, Wv, DSBCAPS_GLOBALFOCUS | DSBCAPS_STICKYFOCUS , false )) != DS_OK)
		{
			printf("Unable to initialize buffer... HRESULT: %d\n", hr);
			system("PAUSE");
		}
		
		if ((hr = dsb->Play(0,0,DSBPLAY_LOOPING)) != DS_OK)
			std::cout << "Failed to play buffer... HRESULT: " << hr << "\n";
		else
		{
			printf("\nRunning...\n");
//			system("PAUSE");
//			ifstream myfile;
//			myfile.open("duration.txt");
//			int duration;
//			myfile >> duration;
//			myfile.close();
//			cout<<duration<<endl;
			Sleep(6000);
			dsb->Stop();
		}


}*/











	double inputAcqTimeLength(3);
void ProstateBiopsyGuidanceGUI::SaveRFData(void)
{

	std::string inputConfigFileName("Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");//"Test_PlusConfiguration_DataCollectionOnly_SonixVideo_FakeTracker.xml");
	std::string outputFolder("./RFDATA");
	std::string outputVideoBufferSequenceFileName("VideoBufferMetafile"); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;


	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if (inputConfigFileName.empty())
	{
		std::cerr << "input-config-file-name is required" << std::endl;
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
		printf("%f seconds left...\n", acqStartTime + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() );
		vtksys::SystemTools::Delay(1000); 
	}


	printf("Copy video buffer\n" );
	vtkVideoBuffer *buffer = vtkVideoBuffer::New(); 
	dataCollector->CopyVideoBuffer(buffer); 

	printf("write video buffer: %s \n",outputVideoBufferSequenceFileName );
	dataCollector->WriteVideoBufferToMetafile( buffer, outputFolder.c_str(), outputVideoBufferSequenceFileName.c_str(), true); 

	buffer->Delete(); 
	dataCollector->Stop();
	VTK_LOG_TO_CONSOLE_OFF; 
	std::cout << "RF_Data is Saved !\n" << std::endl;

/*********************************************************************************/
	///////////////

	VTK_LOG_TO_CONSOLE_ON; 
	std::string outputVideoBufferSequenceFileName_V("VideoBufferMetafile_v"); 
	while (_getch() != 'a') // wait until a is pressed to start acquisition
	{;}
//	startShaker();
	dataCollector->Start();

	const double acqStartTime_V = vtkTimerLog::GetUniversalTime(); 

	while ( acqStartTime_V + inputAcqTimeLength > vtkTimerLog::GetUniversalTime() )
	{
		printf("%f seconds left...\n", acqStartTime_V + inputAcqTimeLength - vtkTimerLog::GetUniversalTime() );
		vtksys::SystemTools::Delay(1000); 
	}
	printf("Copy video buffer\n" );
	vtkVideoBuffer *v_buffer = vtkVideoBuffer::New(); 
	dataCollector->CopyVideoBuffer(v_buffer); 
	printf("write video buffer: %s \n",outputVideoBufferSequenceFileName_V );
	dataCollector->WriteVideoBufferToMetafile( v_buffer, outputFolder.c_str(), outputVideoBufferSequenceFileName_V.c_str(), true); 

	v_buffer->Delete(); 
	dataCollector->Stop();
	std::cout << "Vibro_Data is Saved !\n" << std::endl;

	VTK_LOG_TO_CONSOLE_OFF; 

}

 ProstateBiopsyGuidanceGUI::ProstateBiopsyGuidanceGUI(QWidget *parent)
     : QDialog(parent)
 {

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

