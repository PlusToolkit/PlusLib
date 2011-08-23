#ifndef TRACKEDULTRASOUNDCAPTURINGGUI_H
#define TRACKEDULTRASOUNDCAPTURINGGUI_H

#include "PlusConfigure.h"
// QT parental class includes
#include "ui_TrackedUltrasoundCapturing.h"
#include <QWidget>

class TrackedUltrasoundCapturing; 
class QProgressDialog; 
class QCloseEvent;
class QTimer;
class DeviceSetSelectorWidget;
class ToolStateDisplayWidget; 
class StatusIcon; 

class TrackedUltrasoundCapturingGUI : public QWizard, public Ui::Wizard
{
	Q_OBJECT

public:
	static TrackedUltrasoundCapturingGUI* New();
	// Description:
	// Return the singleton instance with no reference counting.
	static TrackedUltrasoundCapturingGUI* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(TrackedUltrasoundCapturingGUI *instance);

	std::string GetProgramPath(); 
	
	PlusStatus Initialize(); 

	// Constructor
	TrackedUltrasoundCapturingGUI(QWidget* parent = 0);

	//Destructor
	virtual ~TrackedUltrasoundCapturingGUI();

	QProgressBar* GetProgressBar( int pageID ); 

	QVTKWidget* GetRenderer( int pageID ); 

	virtual bool validateCurrentPage(); 

protected:

	virtual void DisableWizardButtons(); 

	virtual void EnableWizardButtons(); 

	virtual void initializePage ( int id ); 

	virtual int previousId(); 

	virtual int nextId() const; 

	virtual void cleanupPage ( int id ); 

	// QT events 
	virtual void closeEvent(QCloseEvent *);

// QT slots
public slots:
	virtual void done ( int r ); 
	
protected slots:

	void UpdateWidgets(); 

	void UpdateProgressBarRequest( int percent ); 

	void RecordTrackedFrame(); 
	
	void SnapshotButtonClicked(); 

	void RecordButtonClicked(); 

	void StopButtonClicked(); 

	void SaveButtonClicked(); 
	
	void SaveAsButtonClicked(); 

	void SynchronizeButtonClicked(); 
	
	void CancelSyncButtonClicked(); 

	void ZeroOffsetButtonClicked(); 

	void OpenOutputFolderButtonClicked(); 

	void ResetBufferButtonClicked(); 

	void ChangeLogLevel(); 

	void SetFrameRate(); 

  void SetConfigurationDirectory(std::string aDirectory); 

  void ConnectToDevicesByConfigFile(std::string aConfigFile); 

signals: 
	void UpdateProgressBar(int); 
	
	void Update(); 
	
protected:

	TrackedUltrasoundCapturing* m_USCapturing; 

	QTimer* m_RecordingTimer; 

  //! Device set selector widget
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

  //! Tool state display widget
	ToolStateDisplayWidget*		m_SyncToolStateDisplayWidget;
  ToolStateDisplayWidget*		m_RecordingToolStateDisplayWidget;

	bool Initialized; 

private:

	TrackedUltrasoundCapturingGUI(const TrackedUltrasoundCapturingGUI&);
	void operator=(const TrackedUltrasoundCapturingGUI&);
	static TrackedUltrasoundCapturingGUI* Instance;

	static void UpdateProgressBarRequestCallback( int percent ); 
	static void UpdateRequestCallback(); 
}; 

#endif //TrackedUltrasoundCapturingGUI_H