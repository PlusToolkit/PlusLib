#ifndef ProstateBiopsyGuidanceGUI_H
#define ProstateBiopsyGuidanceGUI_H

#include "PlusConfigure.h"
// QT parental class includes
#include "ui_ProstateBiopsyGuidance.h"
#include <QWidget>

class ProstateBiopsyGuidance; 
class QProgressDialog; 
class QCloseEvent;
class QTimer;
class DeviceSetSelectorWidget;
class ToolStateDisplayWidget; 
class StatusIcon; 

class ProstateBiopsyGuidanceGUI : public QWizard, public Ui::Wizard
{
	Q_OBJECT

public:
	static ProstateBiopsyGuidanceGUI* New();
	// Description:
	// Return the singleton instance with no reference counting.
	static ProstateBiopsyGuidanceGUI* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(ProstateBiopsyGuidanceGUI *instance);

	std::string GetProgramPath(); 
	
	PlusStatus Initialize(); 

	// Constructor
	ProstateBiopsyGuidanceGUI(QWidget* parent = 0);

	//Destructor
	virtual ~ProstateBiopsyGuidanceGUI();

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

	ProstateBiopsyGuidance* m_USCapturing; 

	QTimer* m_RecordingTimer; 

  //! Device set selector widget
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

  //! Tool state display widget
	ToolStateDisplayWidget*		m_SyncToolStateDisplayWidget;
  ToolStateDisplayWidget*		m_RecordingToolStateDisplayWidget;

	bool Initialized; 

private:

	ProstateBiopsyGuidanceGUI(const ProstateBiopsyGuidanceGUI&);
	void operator=(const ProstateBiopsyGuidanceGUI&);
	static ProstateBiopsyGuidanceGUI* Instance;

	static void UpdateProgressBarRequestCallback( int percent ); 
	static void UpdateRequestCallback(); 
}; 

#endif //ProstateBiopsyGuidanceGUI_H