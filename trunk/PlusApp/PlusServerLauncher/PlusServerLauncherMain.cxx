/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#ifdef _WIN32
  #include <shellapi.h>
  #include <signal.h>
#endif

#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
// TODO: uncomment this when VTK is updated to a version that contains vtksys::Encoding
//#include "vtksys/Encoding.hxx"

#include <QtGui/QApplication>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTextEdit>
#include <QTextStream>
#include "PlusServerLauncherMainWindow.h"

#include "vtkXMLUtilities.h"

#ifdef _WIN32

class QMessageBoxResize: public QMessageBox
{
public:
  QMessageBoxResize() 
  {
    setMouseTracking(true);
    setSizeGripEnabled(true);
  }
private:
  virtual bool event(QEvent *e) 
  {
    bool res = QMessageBox::event(e);
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
      setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
      if (QWidget *textEdit = findChild<QTextEdit *>())
      {
        textEdit->setMaximumHeight(QWIDGETSIZE_MAX);
      }
    }
    return res;
  }
};

void DisplayMessage(QString msg, QString detail, bool isError)
{
  QMessageBoxResize msgBox;
  if (isError)
  {
    msgBox.setIcon(QMessageBox::Critical);
  }

  // Set width to half of screen size
  QRect rec = QApplication::desktop()->screenGeometry();
  QSpacerItem* horizontalSpacer = new QSpacerItem(0.5*rec.width(), 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
  QGridLayout* layout = (QGridLayout*)msgBox.layout();
  layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

  msgBox.setText(msg);
  if (!detail.isEmpty())
  {
    msgBox.setDetailedText(detail);
    if (QWidget *textEdit = msgBox.findChild<QTextEdit *>())
    {
      QFont font;
      font.setFamily("Courier");
      font.setFixedPitch(true);
      font.setPointSize(8);
      textEdit->setFont(font);
    }
  }
  msgBox.exec();
}

#else

void DisplayMessage(QString msg, QString detail, bool isError)
{
  if (isError)
  {
    QTextStream ts( stderr );
    ts << "ERROR: " << msg << "\n";
    if (!detail.isEmpty())
    {
      ts << detail << "\n";
    }
  }
  else
  {
    QTextStream ts( stdout );
    ts << msg << "\n";
    if (!detail.isEmpty())
    {
      ts << detail << "\n";
    }  }  
}

#endif

int appMain(int argc, char *argv[])
{
  QApplication app(argc, argv);

  bool printHelp(false);
  std::string deviceSetConfigurationDirectoryPath;
  std::string inputConfigFileName;
  bool autoConnect=false;

  if (argc > 1)
  {
    int verboseLevel = -1;

	  vtksys::CommandLineArguments cmdargs;
	  cmdargs.Initialize(argc, argv);

	  bool printHelp(false);

    cmdargs.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
	  cmdargs.AddArgument("--device-set-configuration-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceSetConfigurationDirectoryPath, "Device set configuration directory path");
	  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
    cmdargs.AddBooleanArgument("--connect", &autoConnect, "Automatically connect after the application is started");
	  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	  if ( !cmdargs.Parse() )
	  {
      QString cmdArgsString;
      for (int i=0; i<argc; i++)
      {
        cmdArgsString.append(argv[i]);
        cmdArgsString.append(" ");
      }
      QString msg = QString("<html><b>Problem parsing command-line argument [%1]: %2</b><p>Complete command line:<br>%3</html>").arg(cmdargs.GetLastArgument()+1).arg(argv[cmdargs.GetLastArgument()+1]).arg(cmdArgsString);
      QString details;
      details.append("Command-line options:\n");
      details.append(cmdargs.GetHelp());
		  DisplayMessage(msg, details, true);
		  exit(EXIT_FAILURE);
	  }

    if ( printHelp )
    {
      QString msg;
      msg.append("<html><b>Command-line options:</b><p><pre>");
      msg.append(cmdargs.GetHelp());
      msg.append("</pre></html>");
      QString details;
		  DisplayMessage(msg, details, false);
      exit(EXIT_SUCCESS); 
    }

    if (!deviceSetConfigurationDirectoryPath.empty())
    {
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationDirectory(deviceSetConfigurationDirectoryPath.c_str());
    }
    if (!inputConfigFileName.empty())
    {
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(inputConfigFileName.c_str());
    }

    if (verboseLevel > -1)
    {
  	  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
    }
  }

  // Start the application  
  PlusServerLauncherMainWindow PlusServerLauncherMainWindow(0,0,autoConnect);
	PlusServerLauncherMainWindow.show();

	int retValue=app.exec();

  return retValue;
}

#ifdef _WIN32

// TODO: remove these two functions when VTK is updated to a version that contains vtksys::Encoding
size_t vtksys_Encoding_wcstombs(char* dest, const wchar_t* str, size_t n)
{
  if(str == 0)
    {
    return (size_t)-1;
    }
  return WideCharToMultiByte(CP_ACP, 0, str, -1, dest, (int)n, NULL, NULL) - 1;
}
vtksys_stl::string vtksys_Encoding_ToNarrow(const vtksys_stl::wstring& wcstr)
{
  vtksys_stl::string str;
  size_t length = vtksys_Encoding_wcstombs(0, wcstr.c_str(), 0) + 1;
  if(length > 0)
    {
    std::vector<char> chars(length);
    if(vtksys_Encoding_wcstombs(&chars[0], wcstr.c_str(), length) > 0)
      {
      str = &chars[0];
      }
    }
  return str; 
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  Q_UNUSED(hInstance);
  Q_UNUSED(hPrevInstance);
  Q_UNUSED(nShowCmd);

  // CommandLineToArgvW has no narrow-character version, so we get the arguments in wide strings
  // and then convert to regular string.
  int argc=0;
  LPWSTR* argvStringW = CommandLineToArgvW(GetCommandLineW(), &argc);

  std::vector< const char* > argv(argc); // usual const char** array used in main() functions
  std::vector< std::string > argvString(argc); // this stores the strings that the argv pointers point to
  for(int i=0; i<argc; i++)
    {
    // TODO: replace this by vtksys::Encoding::ToNarrow when VTK is updated to a version that contains vtksys::Encoding
    argvString[i] = vtksys_Encoding_ToNarrow(argvStringW[i]);
    argv[i] = argvString[i].c_str();
    }

  LocalFree(argvStringW);

  return appMain(argc, const_cast< char** >(&argv[0]));
}

#else

int main(int argc, char *argv[])
{
  return appMain(argc, argv);
}

#endif
