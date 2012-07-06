// BK library includes
#include "IAcquisitionDataReceiver.h"
#include "libbmode.h"

class vtkBkProFocusVideoSource;

class PlusBkProFocusReceiver : public IAcquisitionDataReceiver
{
public:
  enum ImagingModeType
  {
    BMode,
    RfMode
  };

  PlusBkProFocusReceiver();
  virtual ~PlusBkProFocusReceiver();

  virtual void SetPlusVideoSource(vtkBkProFocusVideoSource *videoSource);

  virtual void SetImagingMode(ImagingModeType imagingMode);

  virtual bool Prepare(int samples, int lines, int pitch);
  virtual bool DataAvailable(int lines, int pitch, void const* frameData);
  virtual bool Cleanup();  

protected:
  vtkBkProFocusVideoSource* CallbackVideoSource;

  static const int MaxNumLines  = 256;
  static const int MaxNumSamples = 512;

  ImagingModeType ImagingMode;

  TBModeParams params;
  unsigned char* frame;
  unsigned char* bmodeFrame;
  unsigned char* rfFrame;
  int decimation;
  int numRfSamples;
};
