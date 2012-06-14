// BK library includes
#include "IAcquisitionDataReceiver.h"
#include "libbmode.h"

class PlusBKOEMReceiver : public IAcquisitionDataReceiver
{
	Q_OBJECT
public:
	PlusBKOEMReceiver(QWidget *parent = 0);

    virtual bool DataAvailable(int lines, int pitch, void const* frameData);

    virtual bool Prepare(int samples, int lines, int pitch);

    virtual bool Cleanup();

	virtual int SetCallback(int *CallbackFunction);

	virtual void DoCallback();

	virtual ~PlusBKOEMReceiver();

	void SetOkToReceiveData(bool ok);

public slots:

protected:
	//void paintEvent(QPaintEvent *event);



public:
static const int MaxNumLines  = 256;
static const int MaxNumSamples = 512;

private:
	int decimation;

	bool okToReceiveData;

	unsigned char* frame;
	unsigned char* bmodeFrame;

	TBModeParams params;

   // CHANGE:
	 vtkMutexLock mutexOkToRecvData;
};
