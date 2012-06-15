// BK library includes
#include "IAcquisitionDataReceiver.h"
#include "libbmode.h"

class vtkBkProFocusVideoSource;

class PlusBkProFocusReceiver : public IAcquisitionDataReceiver
{
public:
  PlusBkProFocusReceiver();
  virtual ~PlusBkProFocusReceiver();

  virtual void SetPlusVideoSource(vtkBkProFocusVideoSource *videoSource);

  virtual bool Prepare(int samples, int lines, int pitch);
  virtual bool DataAvailable(int lines, int pitch, void const* frameData);
  virtual bool Cleanup();

  

protected:
  vtkBkProFocusVideoSource* CallbackVideoSource;

  TBModeParams params;
  unsigned char* frame;
  unsigned char* bmodeFrame;
  int decimation;
};
